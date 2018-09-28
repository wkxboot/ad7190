#ifndef  __AD7190_H__
#define  __AD7190_H__
#include "stdint.h"

#ifdef   __cplusplus
#define  AD7190_BEGIN    extern "C" {
#define  AD7190_END      }
#else
#define  AD7190_BEGIN   
#define  AD7190_END     
#endif



AD7190_BEGIN



#define  RESERVED_VALUE                    0

#define  GENERAL_ENABLE                    1
#define  GENERAL_DISABLE                   0

/*COMMUNICATION REG*/
#define  CR_REG_SELECT_COMM                0   
#define  CR_REG_SELECT_STATUS              0
#define  CR_REG_SELECT_MODE                1
#define  CR_REG_SELECT_CONFIG              2
#define  CR_REG_SELECT_DATA                3
#define  CR_REG_SELECT_ID                  4
#define  CR_REG_SELECT_GPOCON              5
#define  CR_REG_SELECT_OFFSET              6
#define  CR_REG_SELECT_FULL_SCALE          7

#define  CR_RW_READ                        1
#define  CR_RW_WRITE                       0

#define  CR_WEN_ENABLE                     0
#define  CR_WEN_DISABLE                    1

/*状态寄存器*/
#define  SR_RDY                            0
#define  SR_RDY_NOT                        1

#define  SR_ERR                            1
#define  SR_ERR_NO                         0

#define  SR_NOREF_OK                       0
#define  SR_NOREF_ERR                      1

#define  SR_PARITY_ODD                     1
#define  SR_PARITY_EVEN                    0

#define  SR_RESERVED3                      0

#define  SR_CHNEL_AIN1_2                   0
#define  SR_CHNEL_AIN3_4                   1
#define  SR_CHNEL_TEMPERATURE              2
#define  SR_CHNEL_AIN2_2                   3
#define  SR_CHNEL_AIN1_COM                 4
#define  SR_CHNEL_AIN2_COM                 5
#define  SR_CHNEL_AIN3_COM                 6
#define  SR_CHNEL_AIN4_COM                 7


/*MODE  REG*/
#define  MR_MODE_CONTINUE                  0
#define  MR_MODE_SINGLE                    1
#define  MR_MODE_IDLE                      2
#define  MR_MODE_PWR_DOWN                  3
#define  MR_MODE_IZSC                      4
#define  MR_MODE_IFSC                      5
#define  MR_MODE_SZSC                      6
#define  MR_MODE_SFSC                      7

#define  MR_CLK_SELECT_EC_MCLK12           0
#define  MR_CLK_SELECT_EC_MCLK1            1
#define  MR_CLK_SELECT_IC492MHZ_NONE       2
#define  MR_CLK_SELECT_IC492MHZ_MCLK       3


#define  SINC3_FILTER                      3
#define  SINC4_FILTER                      4


/*CONFIG REG*/

#define  CR_GAIN_1                         0 /*+- 5000mv*/
#define  CR_GAIN_8                         3 /*+- 625mv*/
#define  CR_GAIN_16                        4 /*+- 312.5mv*/
#define  CR_GAIN_32                        5 /*+- 156.2mv*/
#define  CR_GAIN_64                        6 /*+- 78mv*/
#define  CR_GAIN_128                       7 /*+- 39.06mv*/

#define  CR_CHNEL_AIN1_2                   1
#define  CR_CHNEL_AIN3_4                   2
#define  CR_CHNEL_TEMPERATURE              4
#define  CR_CHNEL_AIN2_2                   8
#define  CR_CHNEL_AIN1_COM                 16
#define  CR_CHNEL_AIN2_COM                 32
#define  CR_CHNEL_AIN3_COM                 64
#define  CR_CHNEL_AIN4_COM                 128

#define  CR_REF_SELECT_1P_1N               0
#define  CR_REF_SELECT_2P_2N               1





#define  MODULATOR_FREQUENCY               (4920000)   



#ifndef  TRUE              
#define  TRUE                      (1)
#endif


#ifndef  FALSE              
#define  FALSE                     (0)
#endif

typedef struct
{
void (*cs_set)(void);
void (*cs_clr)(void);
void (*write_byte)(uint8_t byte);
uint8_t (*read_byte)(void);
uint8_t is_registered;
}ad7190_io_driver_t;


int ad7190_register_io_driver(ad7190_io_driver_t *io_driver);

int ad7190_init();

int ad7190_internal_zero_scale_calibrate();
int ad7190_internal_full_scale_calibrate();
int ad7190_system_zero_scale_calibrate();
int ad7190_system_full_scale_calibrate();
int ad7190_read_status();
uint8_t ad7190_is_adc_rdy();
uint8_t ad7190_is_adc_err();
uint8_t ad7190_get_channel();
int ad7190_read_id(uint8_t *id);
int ad7190_pwr_down_switch_close(uint8_t bpdsw);
int ad7190_read_conversion_result(uint32_t *buffer);
int ad7190_channel_config(uint8_t chn,uint8_t chop,uint8_t ub,uint8_t gain);
int ad7190_convert_start(uint8_t mode,uint8_t sinc,uint16_t rate);











AD7190_END





#endif












