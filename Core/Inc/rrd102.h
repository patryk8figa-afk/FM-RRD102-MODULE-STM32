//
// Created by Pat on 09/07/2026.
//

#ifndef FM_RRD102_MODULE_RRD102_H
#define FM_RRD102_MODULE_RRD102_H

#include "stm32f7xx_hal.h"
#include "stdint.h"
#include "stdbool.h"

#define RRD102_I2C_ADDR (0x10 << 1)
#define REG06_OPEN_MODE (1<<13)


typedef enum {
    REG_02H = 0,
    REG_03H,
    REG_04H,
    REG_05H,
    REG_06H,
    REG_07H,
    REG_COUNT
}RRD102_Registers;
/*
typedef enum {
    REG_0AH = 0,
    REG_0BH = 1,
    REG_0CH = 2,
    REG_0DH = 3,
    REG_0EH = 4,
    REG_0FH = 5
}RRD102_RegistersRec;
*/
typedef enum {
    REG02_ENABLE     = 0,
    REG02_SOFT_RESET = 1,
    REG02_NEW_METHOD = 2,
    REG02_RDS_EN     = 3,
    REG02_CLKMODE    = 4,
    REG02_SEEK       = 8,
    REG02_SEEKUP     = 9,
    REG02_SKMODE     = 10,
    REG02_BASS       = 12,
    REG02_MONO       = 13,
    REG02_DMUTE      = 14,
    REG02_DHIZ       = 15
}RRD102_REG02_Bits_t;

typedef enum {
    REG03_SPACE       = 0,
    REG03_BAND        = 2,
    REG03_TUNE        = 4,
    REG03_DIRECT_MODE = 5,
    REG03_CHAN        = 6
}RRD102_REG03_Bits_t;

typedef enum {
    REG04_AFDC     = 8,
    REG04_SOFTMUTE = 9,
    REG04_DE       = 11
}RRD102_REG04_Bits_t;

typedef enum {
    REG05_VOLUME = 0,
    REG05_SEEKTH = 8,
    REG05_INT_MODE = 15,
}RRD102_REG05_Bits_t;

typedef enum {
    REG07_FREQ_MODE = 0,
    REG07_SOFTBLEND_EN = 1,
    REG07_65M_50M_MODE = 9,
    REG07_TH_SOFRBLEND = 10,
}RRD102_REG07_Bits_t;


typedef enum {
    RRD_FM_BAND_87_108_EU = 0x00,
    RRD_FM_BAND_76_91_JP = 0x01,
    RRD_FM_BAND_76_108_WW = 0x02,
    RRD_FM_BAND_65_76_EEU = 0x03,
} RRD102_FM_BAND;

typedef enum {
    RDA_CLK_32_768KHZ = 0x00, // 000
    RDA_CLK_12MHZ     = 0x01, // 001
    RDA_CLK_13MHZ     = 0x02, // 010
    RDA_CLK_19_2MHZ   = 0x03, // 011
    RDA_CLK_24MHZ     = 0x05, // 101
    RDA_CLK_26MHZ     = 0x06, // 110
    RDA_CLK_38_4MHZ   = 0x07  // 111
} RDA5807_ClkMode_t;

typedef enum {
    RRD_FREQ_SPACE_100KHZ = 0x00,
    RRD_FREQ_SPACE_200KHZ   = 0x01,
    RRD_FREQ_SPACE_50KHZ   = 0x02,
    RRD_FREQ_SPACE_25KHZ  = 0x03
}RRD102_FREQ_SPACE;

typedef enum {
    SEEK_DOWN = 0x00,
    SEEK_UP   = 0x01
}RRD102_SEEK_MODE; //direction

typedef enum {
    WRAP = 0x00,
    STOP_AT_END = 0x01
}RRD102_SKMODE;

typedef enum {
    OFF = 0x00,
    ON = 0x01
} RRD102_EN;

typedef struct {
    float freq;
    uint8_t volume;
    bool DEemphasis;//zamienic na enum
    RRD102_FM_BAND band;
    RDA5807_ClkMode_t clk_mode;
    RRD102_FREQ_SPACE freq_space;
}RRD102_ConfigTypeDef;

typedef struct {//moze to nie bedzie potrzebne
    //Reg0A
    uint16_t readChan;
    bool st;
    bool blk_e;
    bool rdss;
    bool sf;
    bool stc;
    bool rdsr;

    //Reg0B
    uint8_t blerb;
    uint8_t blera;
    bool abcd_e;
    bool fm_ready;
    bool fm_true;
    uint8_t rssi;

}RRD102_StatusTypeDef;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    uint16_t reg[REG_COUNT];
    //uint16_t regRec[6];
    RRD102_ConfigTypeDef config;
    RRD102_StatusTypeDef status;

}RRD102_HandleTypeDef;





#endif //FM_RRD102_MODULE_RRD102_H
