//
// Created by Pat on 09/07/2026.
//
//zrobic przerwanie na gpio ze radio poda kiedy przyszedl rds
//

#include "main.h"
#include "rrd102.h"

#include "i2c.h"


static HAL_StatusTypeDef RDD102_send_setReg(RDD102_HandleTypeDef *hrrd102) {
    uint8_t buffer_tx[REG_COUNT*2];
    for (uint8_t i=0; i<REG_COUNT; i++)
    {
       buffer_tx[i*2] = (hrrd102->reg[i] >> 8) & 0xFF;
       buffer_tx[i*2+1] = hrrd102->reg[i] & 0xFF;
    }
    return HAL_I2C_Master_Transmit(hrrd102->hi2c, RRD102_I2C_ADDR, buffer_tx, REG_COUNT*2, 100);
}

static uint16_t RDD102_CalcChan(const RDD102_ConfigTypeDef *config)
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

// sprawdzic czy nie lepiej do rejestru wpisac jakis domyslny wartos niz bity po kolei
HAL_StatusTypeDef RDD102_init(RDD102_HandleTypeDef *hrrd102, const RDD102_ConfigTypeDef *config) {
    //hrrd102->reg[REG_02H] = 0;
    hrrd102->reg[REG_02H] = (1<<REG02_ENABLE)     |
                            (1<<REG02_NEW_METHOD) |
                            (1<<REG02_RDS_EN)     |
                            (1<<REG02_DMUTE)      |
                            (1<<REG02_DHIZ);

    hrrd102->reg[REG_03H] &= ~((0x03U<<REG03_SPACE) | (0x03U<<REG03_BAND));
    hrrd102->reg[REG_03H] |= (config->band << REG03_BAND);
    hrrd102->reg[REG_03H] |= (config->freq_space << REG03_SPACE);

    //dsotosowac do funkcji wyzej
    uint16_t chan = RDD102_CalcChan(config);
    hrrd102->reg[REG_03H] &= ~(0x3FFU << REG03_CHAN);
    hrrd102->reg[REG_03H] |= (chan << REG03_CHAN);

    hrrd102->reg[REG_04H] = 0;
    hrrd102->reg[REG_04H] |= (1 << REG04_SOFTMUTE);
    hrrd102->reg[REG_04H] |= (1 << REG04_DE);

}





