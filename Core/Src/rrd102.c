//
// Created by Pat on 09/07/2026.
//
//zrobic przerwanie na gpio ze radio poda kiedy przyszedl rds
// volume

#include "main.h"
#include "rrd102.h"

#include "i2c.h"


static HAL_StatusTypeDef RRD102_send_setReg(RRD102_HandleTypeDef *hrrd102) {
    uint8_t buffer_tx[REG_COUNT*2];
    for (uint8_t i=0; i<REG_COUNT; i++)
    {
       buffer_tx[i*2] = (hrrd102->reg[i] >> 8) & 0xFF;
       buffer_tx[i*2+1] = hrrd102->reg[i] & 0xFF;
    }
    return HAL_I2C_Master_Transmit(hrrd102->hi2c, RRD102_I2C_ADDR, buffer_tx, REG_COUNT*2, 100);
}

static uint16_t RRD102_CalcChan(const RRD102_ConfigTypeDef *config)
{
    uint32_t space=0;
    space = (config->freq_space == RRD_FREQ_SPACE_100KHZ) ? 100 :
        (config->freq_space == RRD_FREQ_SPACE_200KHZ) ? 200 :
        (config->freq_space == RRD_FREQ_SPACE_50KHZ)  ? 50 : 25;
    uint32_t band=0;
    band = (config->band == RRD_FM_BAND_87_108_EU) ? 87000 :
           (config->band == RRD_FM_BAND_76_91_JP || config->band == RRD_FM_BAND_76_108_WW) ? 76000 : 65000;

    uint32_t freq_khz = (uint32_t)(config->freq*1000.0f + 0.5f);
    return (uint16_t)((freq_khz-band)/space);
}

//zagniezdzic config w hrrd102
// sprawdzic czy nie lepiej do rejestru wpisac jakis domyslny wartos niz bity po kolei
HAL_StatusTypeDef RDD102_init(RRD102_HandleTypeDef *hrrd102) {
    //hrrd102->reg[REG_02H] = 0;
    hrrd102->reg[REG_02H] = (1<<REG02_ENABLE)     |
                            (1<<REG02_NEW_METHOD) |
                            (1<<REG02_RDS_EN)     |
                            (1<<REG02_DMUTE)      |
                            (1<<REG02_DHIZ);

    hrrd102->reg[REG_03H] &= ~((0x03U<<REG03_SPACE) | (0x03U<<REG03_BAND));
    hrrd102->reg[REG_03H] |= (hrrd102->config.band << REG03_BAND);
    hrrd102->reg[REG_03H] |= (hrrd102->config.freq_space << REG03_SPACE);

    //dsotosowac do funkcji wyzej
    uint16_t chan = RRD102_CalcChan(&hrrd102->config);
    hrrd102->reg[REG_03H] &= ~(0x03FFU << REG03_CHAN);//poprawic format
    hrrd102->reg[REG_03H] |= (chan << REG03_CHAN);
    hrrd102->reg[REG_03H] |= (1<< REG03_TUNE);

    hrrd102->reg[REG_04H] = 0xA00U;//poprawic format
   // hrrd102->reg[REG_04H] |= (1 << REG04_SOFTMUTE);
    //hrrd102->reg[REG_04H] |= (1 << REG04_DE);
    hrrd102->reg[REG_05H] = 0x880FU;
    hrrd102->reg[REG_06H] = 0x0000U;
    hrrd102->reg[REG_07H] = 0x4202U;
    return RRD102_send_setReg(hrrd102);
}

HAL_StatusTypeDef RRD102_setFreq(RRD102_HandleTypeDef *hrrd102, float freq_mhz) {
    hrrd102->config.freq = freq_mhz;
    uint16_t chan = RRD102_CalcChan(&hrrd102->config);
    hrrd102->reg[REG_03H] &= ~(0x03FFU << REG03_CHAN);
    hrrd102->reg[REG_03H] |= (chan << REG03_CHAN);
    hrrd102->reg[REG_03H] |= (1<< REG03_TUNE);
    return RRD102_send_setReg(hrrd102);
}

//seek seekup skmode
HAL_StatusTypeDef RRD102_Seek(RRD102_HandleTypeDef *hrrd102, RRD102_SEEK_MODE direction, RRD102_SKMODE sk_mode, RRD102_EN seek_en) {

    if (direction == SEEK_DOWN) hrrd102->reg[REG_02H] &= ~(1<<REG02_SEEKUP);
    else if (direction == SEEK_UP) hrrd102->reg[REG_02H] |= (1<<REG02_SEEKUP);

    if (seek_en == OFF) hrrd102->reg[REG_02H] &= ~(1<<REG02_SEEK);
    else if (seek_en == ON) hrrd102->reg[REG_02H] |=(1<<REG02_SEEK);

    if (sk_mode == WRAP) hrrd102->reg[REG_02H] &= ~(1<<REG02_SKMODE);
    else if (sk_mode == STOP_AT_END) hrrd102->reg[REG_02H] |= (1<<REG02_SKMODE);

    return RRD102_send_setReg(hrrd102);
}

HAL_StatusTypeDef RRD102_Bass_boost(RRD102_HandleTypeDef *hrrd102, RRD102_EN boost_mode) {
    if (boost_mode == OFF) hrrd102->reg[REG_02H] &= ~(1<<REG02_BASS);
    else if (boost_mode == ON) hrrd102->reg[REG_02H] |= (1<<REG02_BASS);

    return RRD102_send_setReg(hrrd102);
}

HAL_StatusTypeDef RRD102_CLK_MODE(RRD102_HandleTypeDef *hrrd102, RDA5807_ClkMode_t clkMode) {
    hrrd102->reg[REG_02H] &= ~(0x0007U << REG02_CLKMODE);
    switch (clkMode) {
        case RDA_CLK_32_768KHZ:
            hrrd102->reg[REG_02H] |= (RDA_CLK_32_768KHZ << REG02_CLKMODE);
            break;
        case RDA_CLK_12MHZ:
            hrrd102->reg[REG_02H] |= (RDA_CLK_12MHZ<< REG02_CLKMODE);
            break;
        case RDA_CLK_13MHZ:
            hrrd102->reg[REG_02H] |= (RDA_CLK_13MHZ << REG02_CLKMODE);
            break;
        case RDA_CLK_19_2MHZ:
            hrrd102->reg[REG_02H] |= (RDA_CLK_19_2MHZ << REG02_CLKMODE);
            break;
        case RDA_CLK_24MHZ:
            hrrd102->reg[REG_02H] |= (RDA_CLK_24MHZ << REG02_CLKMODE);
            break;
        case RDA_CLK_26MHZ:
            hrrd102->reg[REG_02H] |= (RDA_CLK_26MHZ << REG02_CLKMODE);
            break;
        case RDA_CLK_38_4MHZ:
            hrrd102->reg[REG_02H] |= (RDA_CLK_38_4MHZ << REG02_CLKMODE);
            break;

        default:
            hrrd102->reg[REG_02H] |= (RDA_CLK_32_768KHZ << REG02_CLKMODE);
            break;
    }

}
