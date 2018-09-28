#include "ad7190.h"


static ad7190_io_driver_t *io;



#define  ASSERT_NULL_POINTER(x)      \
{                                    \
if((x)== (void*)0){                  \
	return -1;                       \
}                                    \
}

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
uint32_t reserved21_22:2;
uint32_t chop:1;
uint32_t reserved24_31:8;
}con_reg;

uint32_t data_reg;
uint8_t  id_reg;

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



static ad7190_t ad7190;


static int ad7190_write_result_check(const uint8_t *check,uint8_t rs,uint8_t cnt);
static int ad7190_writes(uint8_t *buffer,uint8_t cnt);
static int ad7190_reads(uint8_t *buffer,uint8_t cnt);



int ad7190_register_io_driver(ad7190_io_driver_t *io_driver)
{
 ASSERT_NULL_POINTER(io_driver);
 ASSERT_NULL_POINTER(io_driver->cs_clr);
 ASSERT_NULL_POINTER(io_driver->cs_set);
 ASSERT_NULL_POINTER(io_driver->write_byte);
 ASSERT_NULL_POINTER(io_driver->read_byte);

 io=io_driver;
 io->is_registered=TRUE;
 
 return 0;
}


static int ad7190_writes(uint8_t *buffer,uint8_t cnt)
{
uint8_t i;

for(i=0;i<cnt;i++){
io->write_byte(*buffer--);
}

return 0;
}


static int ad7190_reads(uint8_t *buffer,uint8_t cnt)
{
uint8_t i;
	
for(i=0;i<cnt;i++){
*buffer-- =io->read_byte();
}
	
return 0;

}

#define  IF_RESET_TIMEOUT           20000
static int ad7190_if_reset()
{
uint8_t i;
uint32_t timeout= IF_RESET_TIMEOUT;
for(i=0;i<5;i++){
io->write_byte(0xff);
}

while(timeout-- > 0);
  
return 0;
}


static int ad7190_write_result_check(const uint8_t *check,uint8_t rs,uint8_t cnt)
{
uint8_t reg[3];
uint8_t i;

if(cnt>3){
cnt=3;
}

ad7190.comm_reg.rw=CR_RW_READ;
ad7190.comm_reg.rs=rs;

ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_reads(reg+cnt-1,cnt);

for(i=0;i<cnt;i++){
if(reg[i]!=check[i]){
return -1;
}
}

return 0;
}


int ad7190_internal_zero_scale_calibrate()
{
int result;
ad7190.comm_reg.rw=CR_RW_WRITE;
ad7190.comm_reg.rs=CR_REG_SELECT_MODE;

ad7190.mode_reg.md = MR_MODE_IZSC;

ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_writes(((uint8_t *)&ad7190.mode_reg)+2,3);

result =ad7190_write_result_check((uint8_t *)&ad7190.mode_reg,CR_REG_SELECT_MODE,3);
if(result !=0){
return -1;
}

return 0;

}
int ad7190_internal_full_scale_calibrate()
{
int result;
ad7190.comm_reg.rw=CR_RW_WRITE;
ad7190.comm_reg.rs=CR_REG_SELECT_MODE;
	
ad7190.mode_reg.md = MR_MODE_IFSC;
	
ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_writes(((uint8_t *)&ad7190.mode_reg)+2,3);

result =ad7190_write_result_check((uint8_t *)&ad7190.mode_reg,CR_REG_SELECT_MODE,3);
if(result !=0){
return -1;
}

return 0;
}

int ad7190_system_zero_scale_calibrate()
{
int result;
ad7190.comm_reg.rw=CR_RW_WRITE;
ad7190.comm_reg.rs=CR_REG_SELECT_MODE;

ad7190.mode_reg.md = MR_MODE_SZSC;

ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_writes(((uint8_t *)&ad7190.mode_reg)+2,3);

result =ad7190_write_result_check((uint8_t *)&ad7190.mode_reg,CR_REG_SELECT_MODE,3);
if(result !=0){
return -1;
}

return 0;

}
int ad7190_system_full_scale_calibrate()
{
int result;
ad7190.comm_reg.rw=CR_RW_WRITE;
ad7190.comm_reg.rs=CR_REG_SELECT_MODE;
	
ad7190.mode_reg.md = MR_MODE_SFSC;
	
ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_writes(((uint8_t *)&ad7190.mode_reg)+2,3);

result =ad7190_write_result_check((uint8_t *)&ad7190.mode_reg,CR_REG_SELECT_MODE,3);
if(result !=0){
return -1;
}

return 0;
}

int ad7190_read_status()
{
ad7190.comm_reg.rw=CR_RW_READ;
ad7190.comm_reg.rs=CR_REG_SELECT_STATUS;

ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_reads((uint8_t *)&ad7190.status_reg,1); 

return 0;
}


uint8_t ad7190_get_channel()
{
 return  ad7190.status_reg.chd;
}

uint8_t ad7190_is_adc_err()
{
 if(ad7190.status_reg.noref  == SR_NOREF_ERR || \
    ad7190.status_reg.err == SR_ERR){ 
   return TRUE;
}
return FALSE;
}


uint8_t ad7190_is_adc_rdy()
{
if(ad7190.status_reg.rdy == SR_RDY){
return TRUE;
}

return FALSE;
}


int ad7190_read_id(uint8_t *id)
{
ad7190.comm_reg.rw=CR_RW_READ;
ad7190.comm_reg.rs=CR_REG_SELECT_ID;

ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_reads((uint8_t *)&ad7190.id_reg,1);
*id = ad7190.id_reg;
return 0;
}


int ad7190_read_conversion_result(uint32_t *buffer)
{
ad7190.comm_reg.wen=CR_WEN_ENABLE;
ad7190.comm_reg.rw=CR_RW_READ;
ad7190.comm_reg.rs=CR_REG_SELECT_DATA;

ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_reads((uint8_t*)&ad7190.data_reg+2,3);
ad7190_reads((uint8_t*)&ad7190.status_reg,1);

*buffer = ad7190.data_reg;
return 0;
}

int ad7190_channel_config(uint8_t chn,uint8_t chop,uint8_t ub,uint8_t gain)
{
int result;


ad7190.comm_reg.rw=CR_RW_WRITE;
ad7190.comm_reg.rs=CR_REG_SELECT_CONFIG;

ad7190.con_reg.chop = chop;
ad7190.con_reg.chn = chn;
ad7190.con_reg.ub = ub;
ad7190.con_reg.gain = gain;


ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_writes(((uint8_t *)&ad7190.con_reg)+2,3);

result =ad7190_write_result_check((uint8_t *)&ad7190.con_reg,CR_REG_SELECT_CONFIG,3);
if(result !=0){
return -1;
}

return 0;
}

int ad7190_pwr_down_switch_close(uint8_t bpdsw)
{
int result;
ad7190.comm_reg.rw=CR_RW_WRITE;
ad7190.comm_reg.rs=CR_REG_SELECT_GPOCON;  

if(bpdsw == GENERAL_ENABLE){
ad7190.gpocon_reg.bpdsw = GENERAL_ENABLE;
}else{
ad7190.gpocon_reg.bpdsw = GENERAL_DISABLE;
}

ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_writes((uint8_t *)&ad7190.gpocon_reg,1);

result =ad7190_write_result_check((uint8_t *)&ad7190.gpocon_reg,CR_REG_SELECT_GPOCON,1);
if(result !=0){
return -1;
}

return 0;
}


int ad7190_init()
{
int result;
io->cs_set();
/*cs 一直保持低电平*/
io->cs_clr();


ad7190.comm_reg.wen=CR_WEN_ENABLE;
ad7190.comm_reg.rw=CR_RW_WRITE;
ad7190.comm_reg.rs=CR_REG_SELECT_MODE;

ad7190.con_reg.refsel = CR_REF_SELECT_1P_1N;
ad7190.con_reg.buff = GENERAL_ENABLE;
ad7190.con_reg.refdet = GENERAL_ENABLE;

ad7190.mode_reg.clk = MR_CLK_SELECT_IC492MHZ_NONE;
ad7190.mode_reg.single = GENERAL_DISABLE;
ad7190.mode_reg.rej60 = GENERAL_ENABLE;
ad7190.mode_reg.enpar = GENERAL_DISABLE;
ad7190.mode_reg.dat_sta = GENERAL_ENABLE;

ad7190_if_reset();

ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_writes(((uint8_t *)&ad7190.mode_reg)+2,3);

result =ad7190_write_result_check((uint8_t *)&ad7190.mode_reg,CR_REG_SELECT_MODE,3);
if(result !=0){
return -1;
}

return 0;

}



int ad7190_convert_start(uint8_t mode,uint8_t sinc,uint16_t rate)
{
int result;
ad7190.comm_reg.rw=CR_RW_WRITE;
ad7190.comm_reg.rs=CR_REG_SELECT_MODE;

if(mode == MR_MODE_CONTINUE){
ad7190.mode_reg.md = MR_MODE_CONTINUE;
}else{
ad7190.mode_reg.md = MR_MODE_SINGLE;
}


ad7190.mode_reg.sinc3 = sinc;

if(ad7190.con_reg.chop == GENERAL_ENABLE){
if(ad7190.mode_reg.sinc3 == GENERAL_ENABLE){	
ad7190.mode_reg.fs = MODULATOR_FREQUENCY/1024/3/rate;
}else{
ad7190.mode_reg.fs = MODULATOR_FREQUENCY/1024/4/rate;
}
}else{
ad7190.mode_reg.fs = MODULATOR_FREQUENCY/1024/rate;
}

ad7190_writes((uint8_t *)&ad7190.comm_reg,1);
ad7190_writes(((uint8_t *)&ad7190.mode_reg)+2,3);

result =ad7190_write_result_check((uint8_t *)&ad7190.mode_reg,CR_REG_SELECT_MODE,3);
if(result !=0){
return -1;
}

return 0;

}



