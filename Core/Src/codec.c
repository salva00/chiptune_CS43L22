/**
  ******************************************************************************
  * @file           : codec.c
  * @brief          : CS43L22 Codec driver implementation - HAL version
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "codec.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/
static I2C_HandleTypeDef *codec_i2c = NULL;
static uint8_t codec_initialized = 0;

/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef CS43L22_PowerDown(I2C_HandleTypeDef *hi2c);
static HAL_StatusTypeDef CS43L22_PowerUp(I2C_HandleTypeDef *hi2c);

/* Public functions ----------------------------------------------------------*/

/**
  * @brief  Initialize the CS43L22 audio codec
  * @param  hi2c: I2C handle
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_Init(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef status;
    uint8_t regValue;

    /* Store I2C handle for later use */
    codec_i2c = hi2c;

    /* 1. Hardware reset */
    CS43L22_Reset();
    HAL_Delay(100);

    /* 2. Check if device is ready */
    status = HAL_I2C_IsDeviceReady(hi2c, CS43L22_ADDRESS, 3, 1000);
    if (status != HAL_OK)
    {
        return status;
    }

    /* 3. Keep codec powered down during configuration */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_PLAYBACK_CTL1, 0x01);
    if (status != HAL_OK) return status;

    /* 4. Begin initialization sequence (as per datasheet page 32) */
    status = CS43L22_WriteRegister(hi2c, 0x00, 0x99);
    if (status != HAL_OK) return status;

    status = CS43L22_WriteRegister(hi2c, 0x47, 0x80);
    if (status != HAL_OK) return status;

    /* 5. Write 0x80 to register 0x32 */
    regValue = CS43L22_ReadRegister(hi2c, 0x32);
    status = CS43L22_WriteRegister(hi2c, 0x32, regValue | 0x80);
    if (status != HAL_OK) return status;

    /* 6. Write 0x00 to register 0x32 */
    regValue = CS43L22_ReadRegister(hi2c, 0x32);
    status = CS43L22_WriteRegister(hi2c, 0x32, regValue & (~0x80));
    if (status != HAL_OK) return status;

    /* 7. Write 0x00 to register 0x00 */
    status = CS43L22_WriteRegister(hi2c, 0x00, 0x00);
    if (status != HAL_OK) return status;
    /* End of initialization sequence */

    /* 8. Configure power control */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_POWER_CTL2, 0xAF);  /* Headphone channels always on */
    if (status != HAL_OK) return status;

    /* 9. Set sequential play */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_PLAYBACK_CTL1, 0x70);
    if (status != HAL_OK) return status;

    /* 10. Clock configuration (auto-detect) */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_CLOCKING_CTL, 0x81);
    if (status != HAL_OK) return status;

    /* 11. Interface control - I2S, up to 16-bit data */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_INTERFACE_CTL1, 0x07);
    if (status != HAL_OK) return status;

    /* 12. Passthrough analog inputs A & B */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_PASSTHR_A_SELECT, 0x00);
    if (status != HAL_OK) return status;
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_PASSTHR_B_SELECT, 0x00);
    if (status != HAL_OK) return status;

    /* 13. Digital volume settings */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_PCMA_VOL, 0x0A);
    if (status != HAL_OK) return status;
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_PCMB_VOL, 0x0A);
    if (status != HAL_OK) return status;

    /* 14. Configure limiter */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_LIMIT_CTL1, 0x00);
    if (status != HAL_OK) return status;

    /* 15. Configure tone control */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_TONE_CTL, 0x0F);
    if (status != HAL_OK) return status;

    /* 16. Set volume to default */
    status = CS43L22_SetVolume(hi2c, 0x80);  /* Mid-level volume */
    if (status != HAL_OK) return status;

    /* 17. Power up the codec */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_POWER_CTL1, 0x9E);
    if (status != HAL_OK) return status;

    codec_initialized = 1;

    return HAL_OK;
}

/**
  * @brief  Deinitialize the CS43L22 audio codec
  * @param  hi2c: I2C handle
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_Deinit(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef status;

    /* Power down the codec */
    status = CS43L22_PowerDown(hi2c);

    /* Reset the codec */
    CS43L22_Reset();

    codec_initialized = 0;

    return status;
}

/**
  * @brief  Start audio playback
  * @param  hi2c: I2C handle
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_Play(I2C_HandleTypeDef *hi2c)
{
    /* Unmute the output */
    return CS43L22_SetMute(hi2c, 0);
}

/**
  * @brief  Pause audio playback
  * @param  hi2c: I2C handle
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_Pause(I2C_HandleTypeDef *hi2c)
{
    /* Mute the output */
    return CS43L22_SetMute(hi2c, 1);
}

/**
  * @brief  Resume audio playback
  * @param  hi2c: I2C handle
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_Resume(I2C_HandleTypeDef *hi2c)
{
    /* Unmute the output */
    return CS43L22_SetMute(hi2c, 0);
}

/**
  * @brief  Stop audio playback
  * @param  hi2c: I2C handle
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_Stop(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef status;

    /* Mute the output first */
    status = CS43L22_SetMute(hi2c, 1);
    if (status != HAL_OK) return status;

    /* Power down the codec */
    return CS43L22_PowerDown(hi2c);
}

/**
  * @brief  Set audio volume
  * @param  hi2c: I2C handle
  * @param  volume: Volume level (0x00 = Mute, 0xFF = Max)
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_SetVolume(I2C_HandleTypeDef *hi2c, uint8_t volume)
{
    HAL_StatusTypeDef status;
    uint8_t convertedVol;

    /* Convert 0-100 to codec volume range if needed */
    if (volume > 0xE6)
    {
        convertedVol = volume - 0xE7;
    }
    else
    {
        convertedVol = volume + 0x19;
    }

    /* Set both channels to same volume */
    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_MASTER_A_VOL, convertedVol);
    if (status != HAL_OK) return status;

    status = CS43L22_WriteRegister(hi2c, CS43L22_REG_MASTER_B_VOL, convertedVol);
    if (status != HAL_OK) return status;

    return HAL_OK;
}

/**
  * @brief  Set mute on/off
  * @param  hi2c: I2C handle
  * @param  mute: 1 = mute, 0 = unmute
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_SetMute(I2C_HandleTypeDef *hi2c, uint8_t mute)
{
    HAL_StatusTypeDef status;
    uint8_t regValue;

    if (mute)
    {
        /* Soft mute on */
        regValue = CS43L22_ReadRegister(hi2c, CS43L22_REG_PLAYBACK_CTL2);
        status = CS43L22_WriteRegister(hi2c, CS43L22_REG_PLAYBACK_CTL2, regValue | 0x80);
    }
    else
    {
        /* Soft mute off */
        regValue = CS43L22_ReadRegister(hi2c, CS43L22_REG_PLAYBACK_CTL2);
        status = CS43L22_WriteRegister(hi2c, CS43L22_REG_PLAYBACK_CTL2, regValue & ~0x80);
    }

    return status;
}

/**
  * @brief  Set output device
  * @param  hi2c: I2C handle
  * @param  output: Output device selection
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_SetOutputMode(I2C_HandleTypeDef *hi2c, uint8_t output)
{
    HAL_StatusTypeDef status;

    switch(output)
    {
        case OUTPUT_DEVICE_HEADPHONE:
            status = CS43L22_WriteRegister(hi2c, CS43L22_REG_POWER_CTL2, 0xAF);
            break;

        case OUTPUT_DEVICE_SPEAKER:
            status = CS43L22_WriteRegister(hi2c, CS43L22_REG_POWER_CTL2, 0xFA);
            break;

        case OUTPUT_DEVICE_BOTH:
            status = CS43L22_WriteRegister(hi2c, CS43L22_REG_POWER_CTL2, 0xAA);
            break;

        default:
            status = HAL_ERROR;
            break;
    }

    return status;
}

/**
  * @brief  Hardware reset of CS43L22
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_Reset(void)
{
    /* Set reset pin low */
    HAL_GPIO_WritePin(AUDIO_RESET_GPIO_PORT, AUDIO_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);

    /* Set reset pin high */
    HAL_GPIO_WritePin(AUDIO_RESET_GPIO_PORT, AUDIO_RESET_PIN, GPIO_PIN_SET);
    HAL_Delay(10);

    return HAL_OK;
}

/**
  * @brief  Read CS43L22 ID
  * @param  hi2c: I2C handle
  * @retval Chip ID (should be 0xE0 for CS43L22)
  */
uint8_t CS43L22_ReadID(I2C_HandleTypeDef *hi2c)
{
    return CS43L22_ReadRegister(hi2c, CS43L22_REG_ID);
}

/**
  * @brief  Write register to CS43L22
  * @param  hi2c: I2C handle
  * @param  reg: Register address
  * @param  value: Value to write
  * @retval HAL status
  */
HAL_StatusTypeDef CS43L22_WriteRegister(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t value)
{
    return HAL_I2C_Mem_Write(hi2c, CS43L22_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000);
}

/**
  * @brief  Read register from CS43L22
  * @param  hi2c: I2C handle
  * @param  reg: Register address
  * @retval Register value
  */
uint8_t CS43L22_ReadRegister(I2C_HandleTypeDef *hi2c, uint8_t reg)
{
    uint8_t value = 0;
    HAL_I2C_Mem_Read(hi2c, CS43L22_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, 1000);
    return value;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Power down the codec
  * @param  hi2c: I2C handle
  * @retval HAL status
  */
static HAL_StatusTypeDef CS43L22_PowerDown(I2C_HandleTypeDef *hi2c)
{
    return CS43L22_WriteRegister(hi2c, CS43L22_REG_POWER_CTL1, 0x01);
}

/**
  * @brief  Power up the codec
  * @param  hi2c: I2C handle
  * @retval HAL status
  */
static HAL_StatusTypeDef CS43L22_PowerUp(I2C_HandleTypeDef *hi2c)
{
    return CS43L22_WriteRegister(hi2c, CS43L22_REG_POWER_CTL1, 0x9E);
}

/* Legacy compatibility functions --------------------------------------------*/

extern I2C_HandleTypeDef hi2c1;  /* From main.c */

/**
  * @brief  Legacy codec initialization
  */
void codec_init(void)
{
    /* Already handled by HAL MSP init functions */
}

/**
  * @brief  Legacy codec control initialization
  */
void codec_ctrl_init(void)
{
    CS43L22_Init(&hi2c1);
}

/**
  * @brief  Legacy send codec control
  */
void send_codec_ctrl(uint8_t controlBytes[], uint8_t numBytes)
{
    if (numBytes == 2)
    {
        CS43L22_WriteRegister(&hi2c1, controlBytes[0], controlBytes[1]);
    }
    else
    {
        /* Multi-byte write */
        HAL_I2C_Master_Transmit(&hi2c1, CS43L22_ADDRESS, controlBytes, numBytes, 1000);
    }
}

/**
  * @brief  Legacy read codec register
  */
uint8_t read_codec_register(uint8_t mapByte)
{
    return CS43L22_ReadRegister(&hi2c1, mapByte);
}
