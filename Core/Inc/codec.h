/**
  ******************************************************************************
  * @file           : codec.h
  * @brief          : CS43L22 Codec driver header - HAL version
  ******************************************************************************
  */

#ifndef __CODEC_H
#define __CODEC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* CS43L22 I2C Address */
#define CS43L22_ADDRESS                 0x94  /* 8-bit address (0x4A << 1) */

/* CS43L22 Registers mapping */
#define CS43L22_REG_ID                  0x01
#define CS43L22_REG_POWER_CTL1          0x02
#define CS43L22_REG_POWER_CTL2          0x04
#define CS43L22_REG_CLOCKING_CTL        0x05
#define CS43L22_REG_INTERFACE_CTL1      0x06
#define CS43L22_REG_INTERFACE_CTL2      0x07
#define CS43L22_REG_PASSTHR_A_SELECT    0x08
#define CS43L22_REG_PASSTHR_B_SELECT    0x09
#define CS43L22_REG_ANALOG_ZC_SR_SETT   0x0A
#define CS43L22_REG_PASSTHR_GANG_CTL    0x0C
#define CS43L22_REG_PLAYBACK_CTL1       0x0D
#define CS43L22_REG_MISC_CTL            0x0E
#define CS43L22_REG_PLAYBACK_CTL2       0x0F
#define CS43L22_REG_PASSTHR_A_VOL       0x14
#define CS43L22_REG_PASSTHR_B_VOL       0x15
#define CS43L22_REG_PCMA_VOL            0x1A
#define CS43L22_REG_PCMB_VOL            0x1B
#define CS43L22_REG_BEEP_FREQ_ON_TIME   0x1C
#define CS43L22_REG_BEEP_VOL_OFF_TIME   0x1D
#define CS43L22_REG_BEEP_TONE_CFG       0x1E
#define CS43L22_REG_TONE_CTL            0x1F
#define CS43L22_REG_MASTER_A_VOL        0x20
#define CS43L22_REG_MASTER_B_VOL        0x21
#define CS43L22_REG_HEADPHONE_A_VOL     0x22
#define CS43L22_REG_HEADPHONE_B_VOL     0x23
#define CS43L22_REG_SPEAKER_A_VOL       0x24
#define CS43L22_REG_SPEAKER_B_VOL       0x25
#define CS43L22_REG_CH_MIXER_SWAP       0x26
#define CS43L22_REG_LIMIT_CTL1          0x27
#define CS43L22_REG_LIMIT_CTL2          0x28
#define CS43L22_REG_LIMIT_ATTACK_RATE   0x29
#define CS43L22_REG_OVF_CLK_STATUS      0x2E
#define CS43L22_REG_BATT_COMPENSATION   0x2F
#define CS43L22_REG_VP_BATTERY_LEVEL    0x30
#define CS43L22_REG_SPEAKER_STATUS      0x31
#define CS43L22_REG_TEMPMONITOR_CTL     0x32
#define CS43L22_REG_THERMAL_FOLDBACK    0x33
#define CS43L22_REG_CHARGE_PUMP_FREQ    0x34

/* CS43L22 Power Control */
#define CS43L22_PWRCTL1_POWER_UP        0x9E
#define CS43L22_PWRCTL1_POWER_DOWN      0x01

/* CS43L22 Output Devices */
#define OUTPUT_DEVICE_SPEAKER           0x01
#define OUTPUT_DEVICE_HEADPHONE         0x02
#define OUTPUT_DEVICE_BOTH              0x03
#define OUTPUT_DEVICE_AUTO              0x04

/* Volume Levels */
#define DEFAULT_VOLMIN                  0x00
#define DEFAULT_VOLMAX                  0xFF
#define DEFAULT_VOLSTEP                 0x04

/* CS43L22 GPIO Pins */
#define AUDIO_RESET_PIN                 GPIO_PIN_4
#define AUDIO_RESET_GPIO_PORT           GPIOD

/* Exported types ------------------------------------------------------------*/
typedef enum {
    CODEC_OK = 0,
    CODEC_ERROR = 1,
    CODEC_TIMEOUT = 2
} CODEC_StatusTypeDef;

/* Exported functions --------------------------------------------------------*/
HAL_StatusTypeDef CS43L22_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef CS43L22_Deinit(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef CS43L22_Play(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef CS43L22_Pause(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef CS43L22_Resume(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef CS43L22_Stop(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef CS43L22_SetVolume(I2C_HandleTypeDef *hi2c, uint8_t volume);
HAL_StatusTypeDef CS43L22_SetMute(I2C_HandleTypeDef *hi2c, uint8_t mute);
HAL_StatusTypeDef CS43L22_SetOutputMode(I2C_HandleTypeDef *hi2c, uint8_t output);
HAL_StatusTypeDef CS43L22_Reset(void);
uint8_t CS43L22_ReadID(I2C_HandleTypeDef *hi2c);

/* Low level functions */
HAL_StatusTypeDef CS43L22_WriteRegister(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t value);
uint8_t CS43L22_ReadRegister(I2C_HandleTypeDef *hi2c, uint8_t reg);

/* Legacy compatibility */
void codec_init(void);
void codec_ctrl_init(void);
void send_codec_ctrl(uint8_t controlBytes[], uint8_t numBytes);
uint8_t read_codec_register(uint8_t mapByte);

#ifdef __cplusplus
}
#endif

#endif /* __CODEC_H */
