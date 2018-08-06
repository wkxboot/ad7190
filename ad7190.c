#include "ad7190.h"



typedef struct
{
void (*cs_set)(void);
void (*cs_clr)(void);
uint8_t (*read_rdy_bit)(void);
void (*write_byte)(uint8_t byte);
uint8_t (*read_byte)(void);
uint8_t is_registered;

}ad7190_io_driver_t;

static ad7190_io_driver_t *io;



#define  ASSERT_NULL_POINTER(x)      \
{                                    \
if((x)== (void*)0){                  \
	return -1;                       \
}                                    \
}

#ifndef  TRUE              
#define  TRUE                      (1)
#endif


#ifndef  FALSE              
#define  FALSE                     (0)
#endif

typedef struct
{
struct {
uint8_t reserved0_1:2;
uint8_t cread:1;
uint8_t rs:3;
uint8_t rw:1;
uint8_t wen:1;
}comm_reg;

struct {
uint8_t chd:3;
uint8_t reserved3:1;
uint8_t parity:1;
uint8_t noref:1;
uint8_t err:1;
uint8_t rdy:1;
}status_reg;

struct {
uint32_t fs:10;
uint32_t rej60:1;
uint32_t single:1;
uint32_t reserved12:1;
uint32_t enpar:1;
uint32_t reserved14:1;
uint32_t sinc3:1;
uint32_t reserved16_17:2;
uint32_t clk:2;
uint32_t dat_sta:1;
uint32_t md:3;
uint32_t reserved24_31:8;
}mode_reg;


struct {
uint32_t gain:3;
uint32_t ub:1;
uint32_t buff:1;
uint32_t reserved5:1;
uint32_t refdet:1;
uint32_t burn:1;
uint32_t chn:8;
uint32_t reserved16_19:4;
uint32_t refsel:1;
uint32_t reserved21_22:4;
uint32_t chop:1;
uint32_t reserved24_31:8;
}con_reg;

uint8_t data_reg;
uint8_t id_reg;

struct {
uint8_t p0dat:1;
uint8_t p1dat:1;
uint8_t p2dat:1;
uint8_t p3dat:1;
uint8_t p10en:1;
uint8_t p32en:1;
uint8_t bpdsw:1;
uint8_t reserved:1;
}gpocon_reg;

uint8_t offset_reg;
uint8_t full_scale_reg;
}ad7190_t;


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


#define  MR_FILTER_SYNC3                   1
#define  MR_FILTER_SYNC4                   0


#define  MR_FS_4_7_HZ                      1023     
#define  MR_FS_7_5_HZ                      640    
#define  MR_FS_10_HZ                       480   
#define  MR_FS_50_HZ                       96    
#define  MR_FS_60_HZ                       80  
#define  MR_FS_150_HZ                      32    
#define  MR_FS_300_HZ                      16  
#define  MR_FS_960_HZ                      5  
#define  MR_FS_2400_HZ                     2
#define  MR_FS_4800_HZ                     1

/*CONFIG REG*/

#define  CR_GAIN_1                         0 /*+- 5000mv*/
#define  CR_GAIN_8                         3 /*+- 625mv*/
#define  CR_GAIN_16                        4 /*+- 312.5mv*/
#define  CR_GAIN_32                        5 /*+- 156.2mv*/
#define  CR_GAIN_64                        6 /*+- 78mv*/
#define  CR_GAIN_128                       7 /*+- 39.06mv*/

#define  CR_CHNEL_AIN1_2                   0
#define  CR_CHNEL_AIN3_4                   1
#define  CR_CHNEL_TEMPERATURE              2
#define  CR_CHNEL_AIN2_2                   3
#define  CR_CHNEL_AIN1_COM                 4
#define  CR_CHNEL_AIN2_COM                 5
#define  CR_CHNEL_AIN3_COM                 6
#define  CR_CHNEL_AIN4_COM                 7

#define  CR_REF_SELECT_1P_1N               0
#define  CR_REF_SELECT_2P_2N               1







int ad7190_register_io_driver(ad7190_io_driver_t *io_driver)
{
 ASSERT_NULL_POINTER(io_driver);
 ASSERT_NULL_POINTER(io_driver->cs_clr);
 ASSERT_NULL_POINTER(io_driver->cs_set);
 ASSERT_NULL_POINTER(io_driver->read_rdy_bit);
 ASSERT_NULL_POINTER(io_driver->write_byte);
 ASSERT_NULL_POINTER(io_driver->read_byte);

 io=io_driver;
 io->is_registered=TRUE;
}


int ad7190_writes(uint8_t *buffer,uint8_t cnt)
{
uint8_t i;

for(uint8_t i=0;i<cnt;i++){
io->write_byte(*buffer++);
}

return 0;
}


int ad7190_config(uint8_t gain,uint8_t rate,uint8_t chn)
{

}

int ad7190_write_comm_reg(uint8_t reg)
{
if(io->is_registered != TRUE){
	return -1;
}

io->cs_clr();
io



}












































