/**
  ******************************************************************************
  * @file           : chiptune.h
  * @brief          : Chiptune engine header - HAL version
  ******************************************************************************
  */

#ifndef __CHIPTUNE_H
#define __CHIPTUNE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
#define AUDIO_BUFFER_SIZE       512
#define DMA_BUFFER_SIZE        256
#define CHIPTUNE_CHANNELS      4
#define TRACKLEN               32
#define SONGLEN                0x37
#define MAXTRACK               0x92

/* Buffer half definitions for DMA */
#define FIRST_HALF             0
#define SECOND_HALF            1

/* Waveform types */
enum {
    WF_TRI = 0,
    WF_SAW,
    WF_PUL,
    WF_NOI
};

/* Exported types ------------------------------------------------------------*/
typedef struct {
    uint16_t freq;
    uint16_t phase;
    uint16_t duty;
    uint8_t  waveform;
    uint8_t  volume;  // 0-255
} oscillator_t;

struct trackline {
    uint8_t note;
    uint8_t instr;
    uint8_t cmd[2];
    uint8_t param[2];
};

struct track {
    struct trackline line[TRACKLEN];
};

struct unpacker {
    uint16_t nextbyte;
    uint8_t  buffer;
    uint8_t  bits;
};

struct channel {
    struct unpacker trackup;
    uint8_t  tnum;
    int8_t   transp;
    uint8_t  tnote;
    uint8_t  lastinstr;
    uint8_t  inum;
    uint16_t iptr;
    uint8_t  iwait;
    uint8_t  inote;
    int8_t   bendd;
    int16_t  bend;
    int8_t   volumed;
    int16_t  dutyd;
    uint8_t  vdepth;
    uint8_t  vrate;
    uint8_t  vpos;
    int16_t  inertia;
    uint16_t slur;
};

/* Exported variables --------------------------------------------------------*/
extern volatile uint8_t timetoplay;
extern volatile uint8_t callbackwait;
extern volatile uint16_t lastsample16;
extern volatile oscillator_t osc[4];

/* Exported functions --------------------------------------------------------*/
void Chiptune_Init(void);
void Chiptune_Process(void);
void Chiptune_AudioCallback(void);
void Chiptune_FillBuffer(uint8_t half);
uint16_t* getAudioBuffer(void);

/* Legacy compatibility functions */
void chiptune_callback(void);
int mainloop(void);

#ifdef __cplusplus
}
#endif

#endif /* __CHIPTUNE_H */
