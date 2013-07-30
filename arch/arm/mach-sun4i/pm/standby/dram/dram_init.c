/*
*********************************************************************************************************
* File    : dram_init.c
* By      : Berg.Xing
* Date    : 2011-06-01
* Descript: dram for AW1623 chipset;
* Update  : date                auther      ver     notes
*           2011-06-01                  Berg        1.0     create file
*           2011-06-15      			Berg        1.1     change pad drive mode to dynamic mode0;
*															change to automatic IDDQ mode when entry into
*															self-refresh or power down for saving power
*           2011-07-01      			Berg        1.2     add mctl_disable_dll() function
*															add odt control bit and odt impendance
*															increase dram pll delay time
*           2011-07-27      			Berg        1.3     change pad to high speed mode and
*															select DQS passive window mode
*															tune port priority level
*			2011-08-05					Berg		1.4		change mctl_ddr3_reset() for different die
*			2011-09-16					Berg		1.5		disable dqs drfit compensation for low tempature
*     2012-06-15      Daniel      1.6     Change itm_disable to Add Delay to CKE after Clock Stable
*     2012-06-15      Daniel      1.7     Move ZQ Write before Clock Enable
*     2012-06-15      Daniel      1.8     Adjust Initial Delay(including relation among RST/CKE/CLK)
*********************************************************************************************************
*/
#include "dram_i.h"

typedef struct dram_para_t  __dram_para_t;


/*
*********************************************************************************************************
*                                   DRAM INIT
*
* Description: dram init function
*
* Arguments  : para     dram config parameter
*
*
* Returns    : result
*
* Note       :
*********************************************************************************************************
*/
void mctl_ddr3_reset(void)
{
    __u32 reg_val;
    __u32 i=0;

    mctl_write_w(TIMER_CPU_CFG_REG, 0);
    reg_val = mctl_read_w(TIMER_CPU_CFG_REG);
    reg_val >>=6;
    reg_val &=0x3;
    if(reg_val == 0)
    {
        reg_val = mctl_read_w(SDR_CR);
        reg_val &= ~(0x1<<12);
        mctl_write_w(SDR_CR, reg_val);
        standby_delay(0x100);
        reg_val = mctl_read_w(SDR_CR);
        reg_val |= (0x1<<12);
        mctl_write_w(SDR_CR, reg_val);
    }
    else
    {
        reg_val = mctl_read_w(SDR_CR);
        reg_val |= (0x1<<12);
        mctl_write_w(SDR_CR, reg_val);
        standby_delay(0x100);
        reg_val = mctl_read_w(SDR_CR);
        reg_val &= ~(0x1<<12);
        mctl_write_w(SDR_CR, reg_val);
    }
}

void mctl_set_drive(void)
{
    __u32 reg_val;

    reg_val = mctl_read_w(SDR_CR);
    reg_val |= (0x6<<12);
		reg_val |= 0xFFC;
    reg_val &= ~0x3;
    mctl_write_w(SDR_CR, reg_val);
}

void mctl_itm_disable(void)
{
    __u32 reg_val = 0x0;

	reg_val = mctl_read_w(SDR_CCR);
	reg_val |= 0x1<<28;
	reg_val &= ~(0x1U<<31);          //danielwang, 2012-06-15
	mctl_write_w(SDR_CCR, reg_val);
}

void mctl_itm_enable(void)
{
    __u32 reg_val = 0x0;

    reg_val = mctl_read_w(SDR_CCR);
    reg_val &= ~(0x1<<28);
    mctl_write_w(SDR_CCR, reg_val);
}

void mctl_enable_dll0(void)
{
    __u32 i = 0;

    mctl_write_w(SDR_DLLCR0, mctl_read_w(SDR_DLLCR0) & ~0x40000000 | 0x80000000);
	standby_delay(0x100);

    mctl_write_w(SDR_DLLCR0, mctl_read_w(SDR_DLLCR0) & ~0xC0000000);
	standby_delay(0x1000);

    mctl_write_w(SDR_DLLCR0, mctl_read_w(SDR_DLLCR0) & ~0x80000000 | 0x40000000);
    standby_delay(0x1000);
}

void mctl_enable_dllx(void)
{
    __u32 i = 0;

    for(i=1; i<5; i++)
    {
        mctl_write_w(SDR_DLLCR0+(i<<2), mctl_read_w(SDR_DLLCR0+(i<<2)) & ~0x40000000 | 0x80000000);
    }

	standby_delay(0x100);

    for(i=1; i<5; i++)
    {
        mctl_write_w(SDR_DLLCR0+(i<<2), mctl_read_w(SDR_DLLCR0+(i<<2)) & ~0xC0000000);
    }

	standby_delay(0x1000);

    for(i=1; i<5; i++)
    {
        mctl_write_w(SDR_DLLCR0+(i<<2), mctl_read_w(SDR_DLLCR0+(i<<2)) & ~0x80000000 | 0x40000000);
    }
    standby_delay(0x1000);
}

void mctl_disable_dll(void)
{
	__u32 reg_val;

	reg_val = mctl_read_w(SDR_DLLCR0);
	reg_val &= ~(0x1<<30);
	reg_val |= 0x1U<<31;
	mctl_write_w(SDR_DLLCR0, reg_val);

	reg_val = mctl_read_w(SDR_DLLCR1);
	reg_val &= ~(0x1<<30);
	reg_val |= 0x1U<<31;
	mctl_write_w(SDR_DLLCR1, reg_val);

	reg_val = mctl_read_w(SDR_DLLCR2);
	reg_val &= ~(0x1<<30);
	reg_val |= 0x1U<<31;
	mctl_write_w(SDR_DLLCR2, reg_val);

	reg_val = mctl_read_w(SDR_DLLCR3);
	reg_val &= ~(0x1<<30);
	reg_val |= 0x1U<<31;
	mctl_write_w(SDR_DLLCR3, reg_val);

	reg_val = mctl_read_w(SDR_DLLCR4);
	reg_val &= ~(0x1<<30);
	reg_val |= 0x1U<<31;
	mctl_write_w(SDR_DLLCR4, reg_val);
}

__u32 hpcr_value[32] = {
    0x00000301,0x00000301,0x00000301,0x00000301,
    0x00000301,0x00000301,0x0,       0x0,
    	0x0,       0x0,       0x0,       0x0,
    0x0,       0x0,       0x0,       0x0,
			0x00001031,0x00001031,0x00000735,0x00001035,
			0x00001035,0x00000731,0x00001031,0x00000735,
			0x00001035,0x00001031,0x00000731,0x00001035,
			0x00001031,0x00000301,0x00000301,0x00000731,
};

void mctl_configure_hostport(void)
{
    __u32 i;

    for(i=0; i<32; i++)
    {
        mctl_write_w(SDR_HPCR + (i<<2), hpcr_value[i]);
    }
}

void mctl_setup_dram_clock(__u32 clk)
{
    __u32 i;
    __u32 reg_val;

    //setup DRAM PLL
    reg_val = mctl_read_w(DRAM_CCM_SDRAM_PLL_REG);
    reg_val &= ~0x3;
    reg_val |= 0x1;                     //m factor
    reg_val &= ~(0x3<<4);
    reg_val |= 0x1<<4;                  //k factor
    reg_val &= ~(0x1f<<8);
    reg_val |= (standby_uldiv((__u64)clk, 24)&0x1f)<<8;      //n factor
    reg_val &= ~(0x3<<16);
    reg_val |= 0x1<<16;                 //p factor
    reg_val &= ~(0x1<<29);                                         //PLL on
    reg_val |= (__u32)0x1<<31;          //PLL En
    mctl_write_w(DRAM_CCM_SDRAM_PLL_REG, reg_val);
    standby_delay(0x100000);
    reg_val = mctl_read_w(DRAM_CCM_SDRAM_PLL_REG);
	reg_val |= 0x1<<29;
    mctl_write_w(DRAM_CCM_SDRAM_PLL_REG, reg_val);

    //reset GPS
    reg_val = mctl_read_w(DRAM_CCM_GPS_CLK_REG);
    reg_val &= ~0x3;
    mctl_write_w(DRAM_CCM_GPS_CLK_REG, reg_val);
    reg_val = mctl_read_w(DRAM_CCM_AHB_GATE_REG);
    reg_val |= (0x1<<26);
    mctl_write_w(DRAM_CCM_AHB_GATE_REG, reg_val);
    standby_delay(0x20);
    reg_val = mctl_read_w(DRAM_CCM_AHB_GATE_REG);
    reg_val &= ~(0x1<<26);
    mctl_write_w(DRAM_CCM_AHB_GATE_REG, reg_val);

    //open DRAMC AHB clock
    //close it first
    reg_val = mctl_read_w(DRAM_CCM_AHB_GATE_REG);
    reg_val &= ~(0x1<<14);
    mctl_write_w(DRAM_CCM_AHB_GATE_REG, reg_val);
	standby_delay(0x1000);

    //then open it
    reg_val |= 0x1<<14;
    mctl_write_w(DRAM_CCM_AHB_GATE_REG, reg_val);
	standby_delay(0x1000);
}

__s32 DRAMC_init(__dram_para_t *para)
{
    __u32 i;
    __u32 reg_val;
    __s32 ret_val;

    //check input dram parameter structure
    if(!para)
    {
        //dram parameter is invalid
        return -1;
    }

    //setup DRAM relative clock
    mctl_setup_dram_clock(para->dram_clk);

    mctl_set_drive();

    //dram clock off
    DRAMC_clock_output_en(0);

    //select dram controller 1
    mctl_write_w(SDR_SCSR, 0x16237495);

    mctl_itm_disable();
    mctl_enable_dll0();

    //configure external DRAM
    reg_val = 0;
    if(para->dram_type == 3)
        reg_val |= 0x1;
    reg_val |= (para->dram_io_width>>3) <<1;

    if(para->dram_chip_density == 256)
        reg_val |= 0x0<<3;
    else if(para->dram_chip_density == 512)
        reg_val |= 0x1<<3;
    else if(para->dram_chip_density == 1024)
        reg_val |= 0x2<<3;
    else if(para->dram_chip_density == 2048)
        reg_val |= 0x3<<3;
    else if(para->dram_chip_density == 4096)
        reg_val |= 0x4<<3;
    else if(para->dram_chip_density == 8192)
        reg_val |= 0x5<<3;
    else
        reg_val |= 0x0<<3;
    reg_val |= ((para->dram_bus_width>>3) - 1)<<6;
    reg_val |= (para->dram_rank_num -1)<<10;
    reg_val |= 0x1<<12;
    reg_val |= ((0x1)&0x3)<<13;
    mctl_write_w(SDR_DCR, reg_val);

	//set odt impendance divide ratio
	reg_val = mctl_read_w(SDR_ZQCR0);
	reg_val &= ~(0xff<<20);
	reg_val |= ((para->dram_zq)&0xff)<<20;
	mctl_write_w(SDR_ZQCR0, reg_val);

    //Set CKE Delay to about 1ms
	  reg_val = mctl_read_w(SDR_IDCR);
	  reg_val |= 0x1ffff;
	  mctl_write_w(SDR_IDCR, reg_val);

    //dram clock on
    DRAMC_clock_output_en(1);
    //reset external DRAM
    mctl_ddr3_reset();

	standby_delay(0x10);
    while(mctl_read_w(SDR_CCR) & (0x1U<<31)) {};

		mctl_enable_dllx();

		//set I/O configure register
		reg_val = 0x00cc0000;
		reg_val |= (para->dram_odt_en)&0x3;
		reg_val |= ((para->dram_odt_en)&0x3)<<30;
		mctl_write_w(SDR_IOCR, reg_val);

		//set refresh period
    DRAMC_set_autorefresh_cycle(para->dram_clk);

    //set timing parameters
    mctl_write_w(SDR_TPR0, para->dram_tpr0);
    mctl_write_w(SDR_TPR1, para->dram_tpr1);
    mctl_write_w(SDR_TPR2, para->dram_tpr2);

    //set mode register
    if(para->dram_type==3)                  //ddr3
    {
        reg_val = 0x0;
        reg_val |= (para->dram_cas - 4)<<4;
        reg_val |= 0x5<<9;
    }
    else if(para->dram_type==2)             //ddr2
    {
        reg_val = 0x2;
        reg_val |= para->dram_cas<<4;
        reg_val |= 0x5<<9;
    }
    mctl_write_w(SDR_MR, reg_val);

    reg_val = 0x0;
    mctl_write_w(SDR_EMR, para->dram_emr1);
    reg_val = 0x0;
		mctl_write_w(SDR_EMR2, para->dram_emr2);
    reg_val = 0x0;
		mctl_write_w(SDR_EMR3, para->dram_emr3);

	//set DQS window mode
	reg_val = mctl_read_w(SDR_CCR);
	reg_val |= 0x1U<<14;
	reg_val &= ~(0x1U<<17);
	mctl_write_w(SDR_CCR, reg_val);

    //initial external DRAM
    reg_val = mctl_read_w(SDR_CCR);
    reg_val |= 0x1U<<31;
    mctl_write_w(SDR_CCR, reg_val);

    while(mctl_read_w(SDR_CCR) & (0x1U<<31)) {};

    //scan read pipe value
    mctl_itm_enable();
    ret_val = DRAMC_scan_readpipe();

    if(ret_val < 0)
    {
        return 0;
    }
    //configure all host port
    mctl_configure_hostport();

    return DRAMC_get_dram_size();
}

/*
*********************************************************************************************************
*                                   DRAM EXIT
*
* Description: dram exit;
*
* Arguments  : none;
*
* Returns    : result;
*
* Note       :
*********************************************************************************************************
*/
__s32 DRAMC_exit(void)
{
    return 0;
}

/*
*********************************************************************************************************
*                                   CHECK DDR READPIPE
*
* Description: check ddr readpipe;
*
* Arguments  : none
*
* Returns    : result, 0:fail, 1:success;
*
* Note       :
*********************************************************************************************************
*/
__s32 DRAMC_scan_readpipe(void)
{
    __u32 reg_val;

    //data training trigger
    reg_val = mctl_read_w(SDR_CCR);
    reg_val |= 0x1<<30;
    mctl_write_w(SDR_CCR, reg_val);

    //check whether data training process is end
    while(mctl_read_w(SDR_CCR) & (0x1<<30)) {};

    //check data training result
    reg_val = mctl_read_w(SDR_CSR);
    if(reg_val & (0x1<<20))
    {
        return -1;
    }

    return (0);
}


/*
*********************************************************************************************************
*                                   DRAM SCAN READ PIPE
*
* Description: dram scan read pipe
*
* Arguments  : none
*
* Returns    : result, 0:fail, 1:success;
*
* Note       :
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                   DRAM CLOCK CONTROL
*
* Description: dram get clock
*
* Arguments  : on   dram clock output (0: disable, 1: enable)
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void DRAMC_clock_output_en(__u32 on)
{
    __u32 reg_val;

    reg_val = mctl_read_w(DRAM_CCM_SDRAM_CLK_REG);

    if(on)
        reg_val |= 0x1<<15;
    else
        reg_val &= ~(0x1<<15);

    mctl_write_w(DRAM_CCM_SDRAM_CLK_REG, reg_val);
}
/*
*********************************************************************************************************
* Description: Set autorefresh cycle
*
* Arguments  : clock value in MHz unit
*
* Returns    : none
*
* Note       :
*********************************************************************************************************
*/
void DRAMC_set_autorefresh_cycle(__u32 clk)
{
    __u32 reg_val;
    __u32 dram_size;
    __u32 tmp_val;

    dram_size = mctl_read_w(SDR_DCR);
    dram_size >>=3;
    dram_size &= 0x7;

    if(clk < 600)
    {
        if(dram_size<=0x2)
            tmp_val = (131*clk)>>10;
        else
            tmp_val = (336*clk)>>10;
        reg_val = tmp_val;
        tmp_val = (7987*clk)>>10;
        tmp_val = tmp_val*9 - 200;
        reg_val |= tmp_val<<8;
        reg_val |= 0x8<<24;
        mctl_write_w(SDR_DRR, reg_val);
    }
    else
    {
        mctl_write_w(SDR_DRR, 0x0);
    }
}


/*
**********************************************************************************************************************
*                                               GET DRAM SIZE
*
* Description: Get DRAM Size in MB unit;
*
* Arguments  : None
*
* Returns    : 32/64/128
*
* Notes      :
*
**********************************************************************************************************************
*/
__u32 DRAMC_get_dram_size(void)
{
    __u32 reg_val;
    __u32 dram_size;
    __u32 chip_den;

    reg_val = mctl_read_w(SDR_DCR);
    chip_den = (reg_val>>3)&0x7;
    if(chip_den == 0)
        dram_size = 32;
    else if(chip_den == 1)
        dram_size = 64;
    else if(chip_den == 2)
        dram_size = 128;
    else if(chip_den == 3)
        dram_size = 256;
    else if(chip_den == 4)
        dram_size = 512;
    else
        dram_size = 1024;

    if( ((reg_val>>1)&0x3) == 0x1)
        dram_size<<=1;
    if( ((reg_val>>6)&0x7) == 0x3)
        dram_size<<=1;
    if( ((reg_val>>10)&0x3) == 0x1)
        dram_size<<=1;

    return dram_size;
}


__s32 dram_init(void)
{
    /* do nothing for dram init */
    return 0;
}

__s32 dram_exit(void)
{
    return DRAMC_exit();
}

__s32 dram_get_size(void)
{
    return DRAMC_get_dram_size();
}

void dram_set_clock(int clk)
{
    return mctl_setup_dram_clock(clk);
}

void dram_set_drive(void)
{
    mctl_set_drive();
}

void dram_set_autorefresh_cycle(__u32 clk)
{
    DRAMC_set_autorefresh_cycle(clk);
}

__s32 dram_scan_readpipe(void)
{
    return DRAMC_scan_readpipe();
}

