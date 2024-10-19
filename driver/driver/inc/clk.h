/**
 *******************************************************************************
 * @file    clk.h
 * @brief	Clock driver interface
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2015-11-05 10:46:11$
 *
 * @Copyright (C) 2015, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 ******************************************************************************* 
 */
 
 
/**
 * @addtogroup CLOCK
 * @{
 * @defgroup clk clk.h
 * @{
 */
 
#ifndef __CLK_H__
#define __CLK_H__
 
#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "spi_flash.h"
/**
 * CLK module switch macro define
 */
typedef enum __CLOCK_MODULE1_SWITCH
{
	MCLK_PLL_CLK0_EN = (1 << 0),		/**<MCLK PLL0 module clk switch */
	MCLK_PLL_CLK1_EN = (1 << 1),		/**<MCLK PLL1 module clk switch */
	FLASH_CONTROL_PLL_CLK_EN = (1 << 2),/**<Flash Control PLL module clk switch */
	USB_UART_PLL_CLK_EN = (1 << 3),		/**<USB/UART PLL module clk switch */
	AUDIO_DAC0_CLK_EN = (1 << 4),		/**<Audio DAC port0 module clk switch */
	AUDIO_ADC0_CLK_EN = (1 << 5),		/**<Audio ADC port0 module clk switch */
	I2S0_CLK_EN = (1 << 6),				/**<I2S port0 module clk switch */
	FLASH_CONTROL_CLK_EN = (1 << 7),	/**<Flash control module clk switch */
	USB_CLK_EN = (1 << 8),           	/**<USB module clk switch */
	UART0_CLK_EN = (1 << 9),           /**<UART0 module clk switch */
	DMA_CLK_EN = (1 << 10),   			/**<DMA control module clk switch */
	FFT_CLK_EN = (1 << 11),            	/**<FFT module clk switch */
	ADC_CLK_EN = (1 << 12),				/**<sarADC module clk switch */
	EFUSE_CLK_EN = (1 << 13),          	/**<EFUSE module clk switch */
	I2C_CLK_EN = (1 << 14),          	/**<I2C module clk switch */
	SPIM_CLK_EN = (1 << 15),          	/**<SPI master module clk switch */
	SDIO_CLK_EN = (1 << 16),          	/**<SDIO module clk switch */
	TIMER1_CLK_EN = (1 << 17),          /**<TIMER1 module clk switch */
	TIMER2_CLK_EN = (1 << 18),          /**<TIMER2 module clk switch */
	TIMER3_CLK_EN = (1 << 19),          /**<TIMER3 module clk switch */
	TIMER4_CLK_EN = (1 << 20),          /**<TIMER4 module clk switch */
	TIMER5_CLK_EN = (1 << 21),          /**<TIMER5 module clk switch */
	TIMER6_CLK_EN = (1 << 22),          /**<TIMER6 module clk switch */
	BTDM_HCLK_EN = (1 << 23),			/**< module clk switch */
	USB_REG_CLK_EN = (1 << 24),			/**< module clk switch */
	SDIO_REG_CLK_EN = (1 << 25),		/**< module clk switch */
	SPIM_REG_CLK_EN = (1 << 26),			/**< module clk switch */
	UART0_REG_CLK_EN = (1 << 27),		/**< module clk switch */
	FPU_CLK_EN = (1 << 28),				/**< module clk switch */
	BTDM_24M_CLK_EN = (1 << 29),		/**< module clk switch */
	CAN_24M_CLK_EN = (1 << 30),			/**< module clk switch */
	MDM_PLL_CLK_EN = (1 << 31),			/**< module clk switch */
	DEFAULT_MODULE1_CLK_SWITCH = (0x10002080), /**< default module clk switch */
	ALL_MODULE1_CLK_SWITCH = (0xFFFFFFFF),/**<all module clk SWITCH*/
} CLOCK_MODULE1_SWITCH;

typedef enum __CLOCK_MODULE2_SWITCH
{
	BT32K_CLK_EN = (1 << 0),			/**< module clk switch */
	BT_OR_PMU_32K_CLK_EN = (1 << 1),	/**< module clk switch */
	PMU_32K_CLK_EN = (1 << 2),			/**< module clk switch */
//	reserved_CLK2_EN = (1 << 3),		/**< module clk switch */
	MCLK0_APLL_CLK_EN = (1 << 4),		/**<MCLK0 APLL module clk switch */
	MCLK1_APLL_CLK_EN = (1 << 5),		/**<MCLK1 APLL module clk switch */
	SPDIF_APLL_CLK_EN = (1 << 6),  		/**<SPDIF APLL module clk switch */
	USB_APLL_CLK_EN = (1 << 7),         /**<USB module APL clk switch */
	MDM_APLL_CLK_EN = (1 << 8),			/**< module clk switch */
//	reserved_CLK2_EN = (1 << 9),		/**< module clk switch */
//	reserved_CLK2_EN = (1 << 10),		/**< module clk switch */
//	reserved_CLK2_EN = (1 << 11),		/**< module clk switch */
	MCLK0_DPLL_CLK_EN = (1 << 12),		/**<MCLK0 DPLL module clk switch */
	MCLK1_DPLL_CLK_EN = (1 << 13),		/**<MCLK1 DPLL module clk switch */
	SPDIF_DPLL_CLK_EN = (1 << 14),  	/**<SPDIF DPLL module clk switch */
	USB_DPLL_CLK_EN = (1 << 15),        /**<USB module DPLL clk switch */
	MDM_DPLL_CLK_EN = (1 << 16),		/**< module clk switch */
	DEFAULT_MODULE2_CLK_SWITCH = (0x1F1F7), /**< default module clk switch */
	ALL_MODULE2_CLK_SWITCH = (0x1F1F7),	/**<all module clk SWITCH*/
} CLOCK_MODULE2_SWITCH;

typedef enum __CLOCK_MODULE3_SWITCH
{
	SPDIF_PLL_CLK_EN = (1 << 0),		/**< module clk switch */
	SPDIF_CLK_EN = (1 << 1),			/**< module clk switch */
	AUDIO_ADC1_CLK_EN = (1 << 2),		/**<Audio ADC port1 module clk switch */
	I2S1_CLK_EN = (1 << 3),				/**<I2S port1 module clk switch */
	UART1_CLK_EN = (1 << 4),           /**<UART1 module clk switch */
	SPIS_REG_CLK_EN = (1 << 5),			/**< module clk switch */
	UART1_REG_CLK_EN = (1 << 6),		/**< module clk switch */
	SPDIF0_REG_CLK_EN = (1 << 7),		/**< module clk switch */
	MDM_12M_CLK_EN = (1 << 8),			/**< module clk switch */
	MDM_24M_CLK_EN = (1 << 9),			/**< module clk switch */
	MDM_48M_CLK_EN = (1 << 10),			/**< module clk switch */
	RTC32K_CLK_EN = (1 << 11),			/**< module clk switch */
	SPDIF1_CLK_EN = (1 << 12),			/**< module clk switch */
	SPDIF1_REG_CLK_EN = (1 << 13),		/**< module clk switch */
	BTDM_LP_CLK_EN = (1 << 14),			/**< module clk switch */
	I2S0_REG_CLK_EN = (1 << 15),		/**< module clk switch */
	I2S1_REG_CLK_EN = (1 << 16),		/**< module clk switch */
	MDAC_REG_CLK_EN = (1 << 17),		/**< module clk switch */
	TIMER7_CLK_EN = (1 << 18),          /**<TIMER7 module clk switch */
	TIMER8_CLK_EN = (1 << 19),          /**<TIMER8 module clk switch */
	DEFAULT_MODULE3_CLK_SWITCH = (0x3E080), /**< default module clk switch */
	ALL_MODULE3_CLK_SWITCH = (0xFFFFF),	/**<all module clk SWITCH*/
} CLOCK_MODULE3_SWITCH;


typedef enum __CLK_MODE{
	RC_CLK_MODE,	/*RC 12M source*/
	PLL_CLK_MODE,	/*DPLL source*/
	APLL_CLK_MODE,	/*APLL source*/
	SYSTEM_CLK_MODE /*System Clock*/
}CLK_MODE;

typedef enum _CLK_32K_MODE{
	HOSC_DIV_32K_CLK_MODE = 0,	/*HOSC DIV source,OSC24M_or_RC12M_DIV_32k_clk*/
	RC_CLK32_MODE,			/*RC32K source,bt_32k_clk */
	PMU_32K_MODE,			/*pmu_32k_clk*/
	LOSC_32K_MODE,			/*LOSC 32K source,RTC32K*/
}CLK_32K_MODE;


typedef enum __PLL_CLK_INDEX{
	PLL_CLK_1 = 0,	/**PLL��Ƶʱ����1,�����������Ϊ11.2896M����������޸�*/
	PLL_CLK_2		/**PLL��Ƶʱ����2,�����������Ϊ12.288M����������޸�*/
}PLL_CLK_INDEX;

typedef enum __AUDIO_MODULE{
	AUDIO_DAC0,
	AUDIO_ADC0,
	AUDIO_ADC1,
	AUDIO_I2S0,
	AUDIO_I2S1,
}AUDIO_MODULE;

typedef enum __MCLK_CLK_SEL{
	PLL_CLOCK1 = 0,	/**PLL��Ƶ1ʱ�ӣ��������PLL��Ƶʱ��������Ƶ��Ϊ11.2896M����������Ƶ��*/
	PLL_CLOCK2,		/**PLL��Ƶ2ʱ�ӣ��������PLL��Ƶʱ��������Ƶ��Ϊ12.288M����������Ƶ��*/
	GPIO_IN0,		/**GPIO MCLK in0����*/
	GPIO_IN1,       /**GPIO MCLK in1����*/
}MCLK_CLK_SEL;


typedef enum __CLOCK_GPIO_Port//���ʱ�ӹ۲�˿�
{
	GPIO_CLK_OUT_A0,		//GPIO A0   clk1_out_0
	GPIO_CLK_OUT_A1,		//GPIO A1   clk0_out_0
	GPIO_CLK_OUT_A29,		//GPIO A29  clk0_out_1
	GPIO_CLK_OUT_B6,		//GPIO AB6  clk0_out_2
}CLK_GPIO_Port;

typedef enum __CLOCK_OUT_MODE
{
	CLK0_ETTING_NONE = 0,	//0:   none
	MCLK_PLL_CLK0_DIV,	//1:  clk0   mclk_pll_clk0_div
	FSHC_CLK,			//2:  clk0    fshc_clk
	USB_CLK,			//3:  clk0    usb_clk
	APB_CLK,			//4:  clk0    apb_clk
	SPIM_CLK,			//5:  clk0    spim_clk
	SDIO_CLK,			//6:  clk0    sdio_clk
	RC12M_CLK,			//7:  clk0    rc12M_clk
	BTDM_HCLK,			//8:  clk0    btdm_hclk
	BTDM_24M_CLK,		//9:  clk0    btdm_24m_clk
	BT_PMU_32K_CLK,		//10: clk0    bt_or_pmu_32k_clk
	DPLL_CLK_DIV,		//11: clk0    pll_clk_div
	SPDIF_CLK,			//12: clk0    spdif_clk
	CODEC_CLK_IN,		//13: clk0    codec_clk_in
	MDACK0_CLK_SW,		//14: clk0    mdac0_clk_sw
	ASDM0_CLK_SW,		//15: clk0    asdm0_clk_sw
	ASDM1_CLK_SW,		//16: clk0    asdm1_clk_sw
	APLL_CLK_SW,		//17: clk0    apll_clk_div
	LOSC_32K,			//18: clk0    LOSC32K
	MDM_24M_CLK,		//19: clk0    mdm_24m_clk
	OTG_1KHz_CLK= 0x21,	    //1:   clk1  otg1khz_clk
	SPDIF_BLOCK_FLAG ,	    //2:   clk1  spdif_block_flag
	SPDIF_FRAME_FLAG,		//3:   clk1  spdif_frame_flag
	CLK_12M_MDM_MUX,		//4:   clk1  clk_12m_mdm_mux
	CLK_48M_MDM_MUX,		//5:   clk1  clk_48m_mdm_mux
}CLOCK_OUT_MODE;

typedef enum __CLOCK_SYNC_CTRL_MODULE{
	I2S_BCLK_SEL,
	MDAC_MCLK_SEL,
	START_EN,
	UPDATE_DONE_CLR,
	UPDATE_EN,
	UPDATE_DONE,
}CLOCK_SYNC_CTRL_MODULE;

typedef enum __CLOCK_CLK_CNT_MODULE{
	I2S_BCLK_CNT0,
	I2S_BCLK_CNT1,
	MDAC_MCLK_CNT0,
	MDAC_MCLK_CNT1,
}CLOCK_CLK_CNT_MODULE;

//����MCLK0 ����Ϊ11.2896M��MCLK1����Ϊ12.288M
#define		AUDIO_PLL_CLK1_FREQ		11289600//PLL1,11.2896MHz
#define		AUDIO_PLL_CLK2_FREQ		12288000//PLL2,12.288MHz
#define 	IsSelectMclkClk1(freq)  ((AUDIO_PLL_CLK1_FREQ%freq) == 0)


/**
 * @brief	ϵͳ�ο�ʱ��Դ����ѡ��
 * @param	IsOsc TURE������ʱ�ӣ�FALSE��XIN�˿���෽��ʱ��
 * @param   Freq  ϵͳ�ο�ʱ�ӵĹ���Ƶ�ʣ�32.768K,1M,2M,...40M,��λHZ
 * @return	��
 * @note	���ʹ��PLLʱ�������Ҫ�ȵ��øú���
 */
void Clock_Config(bool IsOsc, uint32_t Freq);

/**
 * @brief	��ȡCore������ʱ��Ƶ��
 * @param	��
 * @return	Core����Ƶ��
 * @note    Coreʱ�Ӻ�ϵͳʱ�ӿ���ͬƵ����ʱ�������120MHz
 */
uint32_t Clock_CoreClockFreqGet(void);

/**
 * @brief  ����Coreʱ�ӷ�Ƶϵ��
 * @param  DivVal [1-256]��Freq = Fpll/Div
 * @return ��
 * @note    Coreʱ�Ӻ�ϵͳʱ�ӿ���ͬƵ����ʱ�������144MHz
 */
void Clock_CoreClkDivSet(uint32_t DivVal);

/**
 * @brief   ��ȡCore��Ƶϵ��
 * @param   ��
 * @return  ϵͳ��Ƶϵ��[0-255]
 * @note    Coreʱ�Ӻ�ϵͳʱ�ӿ���ͬƵ����ʱ�������144MHz
 */
uint32_t Clock_CoreClkDivGet(void);

/**
 * @brief	��ȡϵͳ������ʱ��Ƶ��
 * @param	��
 * @return	ϵͳ����Ƶ��
 * @note    Coreʱ�Ӻ�ϵͳʱ�ӿ���ͬƵ����ʱ�������144MHz
 */
uint32_t Clock_SysClockFreqGet(void);

/**
 * @brief   ����ϵͳʱ�ӷ�Ƶϵ��
 * @param   DivVal [0-8]��0�رշ�Ƶ����, 1����Ƶ
 * @return  ��
 * @note    Coreʱ�Ӻ�ϵͳʱ�ӿ���ͬƵ����ʱ�������144MHz
 */
void Clock_SysClkDivSet(uint32_t DivVal);
    
/**
 * @brief   ��ȡϵͳʱ�ӷ�Ƶϵ��
 * @param   NONE
 * @return  ϵͳ��Ƶϵ��[1-8]
 */
uint32_t Clock_SysClkDivGet(void);

/**
 * @brief   ����APB���߷�Ƶϵ��������ϵͳ���߷�Ƶ
 * @param   DivVal [2-14]
 * @return  ��
 */
void Clock_ApbClkDivSet(uint32_t DivVal);

/**
 * @brief   ��ȡAPB���߷�Ƶϵ��������ϵͳ���߷�Ƶ
 * @param   ��
 * @return  APB���߷�Ƶϵ��[2-14]
 * @note	APBʱ�Ӳ��ܴ���37.5MHz
 */
uint32_t Clock_ApbClkDivGet(void);

/**
 * @brief   ����USBʱ�ӷ�Ƶϵ��������PLLʱ�ӷ�Ƶ
 * @param   DivVal [2-16]
 * @return  ��
 * #note	
 */
void Clock_USBClkDivSet(uint32_t DivVal);

/**
 * @brief   ��ȡUSBʱ�ӷ�Ƶϵ��������PLLʱ�ӷ�Ƶ
 * @param   ��
 * @return  USBʱ�ӷ�Ƶϵ��[2-16]
 */
uint32_t Clock_USBClkDivGet(void);

/**
 * @brief  	����SPI master����ʱ�ӷ�Ƶϵ������ϵͳʱ�ӷ�Ƶ
 * @param  	DivVal ��Ƶϵ��[2-15]
 * @return  ��
 * @note	SPIMʱ�Ӳ�����120M
 */
void Clock_SPIMClkDivSet(uint32_t DivVal);

/**
 * @brief  ��ȡSPI master����ʱ�ӷ�Ƶϵ������ϵͳʱ�ӷ�Ƶ
 * @param  ��
 * @return  ��Ƶϵ��[2-15]
 */
uint32_t Clock_SPIMClkDivGet(void);

/**
 * @brief   ����SDIO0����ʱ�ӷ�Ƶϵ������ϵͳʱ�ӷ�Ƶ
 * @param   DivVal ��Ƶϵ��[2-15]
 * @return  ��
 * @note	SDIOʱ�Ӳ��ܴ���60MHz
 */
void Clock_SDIOClkDivSet(uint32_t DivVal);

/**
 * @brief  ��ȡSDIO0����ʱ�ӷ�Ƶϵ������ϵͳʱ�ӷ�Ƶ
 * @param  ��
 * @return  ��Ƶϵ��[2-15]
 */
uint32_t Clock_SDIOClkDivGet(void);

/**
 * @brief	����OSC��Ƶϵ������Ƶ֮���ʱ���ṩ��RTCʹ��
 * @param	DivVal ��Ƶϵ��[1-2048]��
 * @return  ��
 */
void Clock_OSCClkDivSet(uint32_t DivVal);

/**
 * @brief	��ȡOSC��Ƶϵ������Ƶ֮���ʱ���ṩ��RTCʹ��
 * @param	��
 * @return  OSC��Ƶϵ��[1-2048]
 */
uint32_t Clock_OSCClkDivGet(void);

/**
 * @brief	����pll����Ƶ��,�ȴ�pll lock
 * @param	PllFreq pllƵ��,��λKHz[120000K-240000K]
 * @return  PLL�������  TRUE:���趨Ŀ��Ƶ������
 */
bool Clock_PllLock(uint32_t PllFreq);

/**
 * @brief	����APLL����Ƶ��,�ȴ�APLL lock,���Ƶ��Ϊ����Ƶ�ʣ�û�з�Ƶ
 * @param	APllFreq pllƵ��,��λKHz[120000K-240000K]
 * @return  APLL�������  TRUE:���趨Ŀ��Ƶ������
 */
bool Clock_APllLock(uint32_t PllFreq);


bool Clock_APllLock_Pro(uint32_t ApllLockFreq,uint8_t DIVMODE,uint8_t VCO_ISEL,uint8_t CP_ISEL);

/**
 * @brief	��ȡRC12MƵ��
 * @param	IsReCount �Ƿ��ٴλ�ȡӲ��������ֵ��TRUE���ٴ�����Ӳ����������FALSE����ȡ�ϴμ�¼ֵ��
 * @return  rcƵ�ʣ���λHz
 */	
uint32_t Clock_RcFreqGet(bool IsReCount);

/**
 * @brief	��ȡpllƵ��
 * @param	��
 * @return  pllƵ��
 * @note    �ú���������ҪӦ��ȷ��pll�Ѿ�lock�������������
 */
uint32_t Clock_PllFreqGet(void);

/**
 * @brief	��ȡpllƵ��
 * @param	Win ���������ڡ�3:2048��
 * @return  ��
 */
void Clock_HOSCCntWindowSet(uint32_t Win);

/**
 * @brief	�ر�rcƵ��Ӳ���Զ����¹���
 * @param	��
 * @return  ��
 */
void Clock_HOSCFreqAutoCntDisable(void);

/**
 * @brief	����rcƵ��Ӳ���Զ����¹���
 * @param	��
 * @return  ��
 */
void Clock_HOSCFreqAutoCntStart(void);

/**
 * @brief	��ȡrcƵ�ʣ�Ӳ���Զ����£�
 * @param	��
 * @return  ��
 */
uint32_t Clock_HOSCFreqAutoCntGet(void);

/**
 * @brief	����pll����Ƶ��,��������ģʽ���ȴ�pll lock
 * @param	PllFreq pllƵ��,��λKHz[240000K-480000K]
 * @param	K1 [0-15]
 * @param	OS [0-31]
 * @param	NDAC [0-4095]
 * @param	FC [0-2]
 * @param	Slope [0-16777216]
 * @return  PLL�������  TRUE:���趨Ŀ��Ƶ������
 */
bool Clock_PllQuicklock(uint32_t PllFreq, uint8_t K1, uint8_t OS, uint32_t NDAC, uint32_t FC, uint32_t Slope);

/**
 * @brief	����pll����Ƶ��,��������ģʽ(����Ҫ���壬����У׼)
 * @param	PllFreq pllƵ��,��λKHz[240000K-480000K]
 * @param	K1 [0-15]
 * @param	OS [0-31]
 * @param	NDAC [0-4095]
 * @param	FC [0-2]
 * @return  ��
 */
void Clock_PllFreeRun(uint32_t PllFreq, uint32_t K1, uint32_t OS, uint32_t NDAC, uint32_t FC);

/**
 * @brief	pll free run һ��ֻ��Ҫ���˺�������
 * @param	��
 * @return  TRUE, ���óɹ�
 */
bool Clock_PllFreeRunEfuse(void);

/**
 * @brief	DPLL USB�⾧��ʱ�ӹ�������
 * @param	Ŀ�� DPLL ʱ��Ƶ��
 * @return  ��
 */
void Clock_USBCrystaFreeSet(uint32_t target_freq);

/**
 * @brief	USB�⾧��ʱ��У׼����
 * @param	��
 * @return  У׼״̬
 */
uint8_t Clock_USBCrystaFreeAdjustProcess(void);

/**
 * @brief	pllģ��ر�
 * @param	��
 * @return  ��
 */
void Clock_PllClose(void);

/**
 * @brief	AudioPllģ��ر�
 * @param	��
 * @return  ��
 */
void Clock_APllClose(void);

/**
 * @brief	ѡ��ϵͳ����ʱ��
 * @param	ClkMode, RC_CLK_MODE: RC12Mʱ��; PLL_CLK_MODE:pllʱ��;
 * @return  �Ƿ�ɹ��л�ϵͳʱ�ӣ�TRUE�������л���FALSE���л�ʱ��ʧ�ܡ�
 * @note    ϵͳʱ�ӹ�����pllʱ��ʱ��Ϊpllʱ�ӵ�2��Ƶ
 */
bool Clock_SysClkSelect(CLK_MODE ClkMode);

/**
 * @brief	Uartģ��ʱ��ѡ��,pllʱ�ӻ���RCʱ��
 * @param	ClkMode ʱ��Դѡ�� 
 *   @arg	RC_CLK_MODE: RCʱ��;
 *   @arg	PLL_CLK_MODE:DPLLʱ�ӣ�PLL��Ƶ֮��
 *   @arg	APLL_CLK_MODE:APLLʱ�ӣ�APLL��Ƶ֮��
 * @return  ��
 * @note	ע���ʱ��Դ���ѡ��PLL����APLLʱ�ӣ��Ǻ�USBʱ��ԴͬԴ
 */
void Clock_UARTClkSelect(CLK_MODE ClkMode);

/**
 * @brief	Timer5ģ��ʱ��ѡ��,ϵͳʱ��ʱ�ӻ���RCʱ��
 * @param	ClkMode SYSTEM_CLK_MODE: ϵͳʱ��; RC_CLK_MODE:RC 12Mʱ��
 * @return  ��
 */
void Clock_Timer5ClkSelect(CLK_MODE ClkMode);

/**
 * @brief	BTDMģ��ʱ��ѡ��,ϵͳʱ��ʱ�ӻ���RCʱ��
 * @param	ClkMode BTDMģ��
 *   @arg	HOSC_DIV_32K_CLK_MODE: 24M�������RC12Mʱ�ӷ�Ƶ��32K;
 *   @arg	RC_CLK32_MODE: RC32K source,bt_32k_clk
 *   @arg	PMU_32K_MODE: pmu_32k_clk
 *   @arg	LOSC_32K_MODE: LOSC 32K source,RTC32K
 * @return  ��
 */
void Clock_BTDMClkSelect(CLK_32K_MODE ClkMode);

/**
 * @brief	Spdifģ��ʱ��ѡ��,DPLLʱ�ӻ���APLLʱ��
 * @param	ClkMode	ʱ��Դѡ��
 *   @arg	PLL_CLK_MODE:DPLLʱ�ӣ�PLL��Ƶ֮��
 *   @arg	APLL_CLK_MODE:APLLʱ�ӣ�APLL��Ƶ֮��
 * @return  ��
 */
void Clock_SpdifClkSelect(CLK_MODE ClkMode);


/**
 * @brief	USBģ��ʱ��ѡ��,DPLLʱ�ӻ���APLLʱ��
 * @param	ClkMode	ʱ��Դѡ��
 *   @arg	PLL_CLK_MODE:DPLLʱ�ӣ�PLL��Ƶ֮��
 *   @arg	APLL_CLK_MODE:APLLʱ�ӣ�APLL��Ƶ֮��
 * @return  ��
 * @note	ʱ��Ƶ�ʲ�Ҫ����60M��һ������Ϊ60M
 */
void Clock_USBClkSelect(CLK_MODE ClkMode);

/**
 * @brief	ģ��ʱ��ʹ��
 * @param	ClkSel ģ��Դ������CLOCK_MODULE1_SWITCHѡ��
 * @return  ��
 */
void Clock_Module1Enable(CLOCK_MODULE1_SWITCH ClkSel);

/**
 * @brief	ģ��ʱ�ӽ���
 * @param	ClkSel ģ��Դ������CLOCK_MODULE1_SWITCHѡ��
 * @return  ��
 */
void Clock_Module1Disable(CLOCK_MODULE1_SWITCH ClkSel);

/**
 * @brief	ģ��ʱ��ʹ��
 * @param	ClkSel ģ��Դ������CLOCK_MODULE2_SWITCHѡ��
 * @return  ��
 */
void Clock_Module2Enable(CLOCK_MODULE2_SWITCH ClkSel);

/**
 * @brief	ģ��ʱ�ӽ���
 * @param	ClkSel ģ��Դ������CLOCK_MODULE2_SWITCHѡ��
 * @return  ��
 */
void Clock_Module2Disable(CLOCK_MODULE2_SWITCH ClkSel);

/**
 * @brief	ģ��ʱ��ʹ��
 * @param	ClkSel ģ��Դ������CLOCK_MODULE3_SWITCHѡ��
 * @return  ��
 */
void Clock_Module3Enable(CLOCK_MODULE3_SWITCH ClkSel);

/**
 * @brief	ģ��ʱ�ӽ���
 * @param	ClkSel ģ��Դ������CLOCK_MODULE3_SWITCHѡ��
 * @return  ��
 */
void Clock_Module3Disable(CLOCK_MODULE3_SWITCH ClkSel);

/**
 * @brief	��ƵPLLʱ������
 * @param	CLK_MODE PLLԴ: PLL_CLK ���� APLL_CLK
 * @param	Index PLLԴ��PLL_CLK_1:11.2896M;PLL_CLK_2:12.288M;
 * @param	TargetFreq ��������ƵMCLKƵ�ʣ��Ƽ�����PLL1�� 11 2896M��PLL2: 12.288M��Unit:Hz
 * @return  ��
 */
void Clock_AudioPllClockSet(CLK_MODE ClkMode, PLL_CLK_INDEX Index, uint32_t TargetFreq);

/**
 * @brief	��Ƶģ����ʱ��Դѡ��
 * @param	Module ��Ƶģ��
 * @param	ClkSel ʱ����Դ����ѡ��
 * @return  ��
 */
void Clock_AudioMclkSel(AUDIO_MODULE Module, MCLK_CLK_SEL ClkSel);

/**
 * @brief	��ȡ��Ƶģ����ʱ��Դ
 * @param	Module ��Ƶģ��
 * @return  ʱ����Դ����
 */
MCLK_CLK_SEL Clock_AudioMclkGet(AUDIO_MODULE Module);

/**
 * @brief	��Ƶʱ��Դ������΢����PLL1��PLL2
 * @param	Index PLLԴ��PLL_CLK_1:11.2896M;PLL_CLK_2:12.288M;
 * @param	Sign  0��������1������
 * @param	Ppm ʱ��΢��������Ϊ0ʱӲ��ʱ����΢�����ܡ�
 * @return  ��
 */
void Clock_AudioPllClockAdjust(PLL_CLK_INDEX Index,uint8_t Sign, uint16_t Ppm);

/**
 * @brief	ʹ��DeepSleep����ʱ��ϵͳʱ��ѡ���flashʱ��ѡ��
 * @param	ClockSelect
 * @param	FlashClockSel
 * @param	IsEnterDeepSeep  TRUE:ϵͳ����DeepSleep  FALSE:ϵͳ����DeepSleep
 * @return  bool  TRUE:ʱ���л��ɹ�     FALSE:ʱ���л�ʧ��
 */
bool Clock_DeepSleepSysClkSelect(CLK_MODE SysClockSelect, FSHC_CLK_MODE FlashClockSel, bool IsEnterDeepSeep);

/**
 * @brief	���ø�Ƶ����������������
 * @param	XICap ����������������
 * @param	XOCap ����������������
 * @return  ��
 */
void Clock_HOSCCapSet(uint32_t XICap, uint32_t XOCap);

/**
 * @brief	���ø�Ƶ��������ôֵ�����
 * @param	XICap ����ֵ������������ݡ�0-3��
 * @param	XOCap ����ֵ������������ݡ�0-3��
 * @return  ��
 */
void Clock_HOSCMosCapSet(uint32_t XICap, uint32_t XOCap);

/**
 * @brief	���ø�Ƶ����������������ƫ�õ���
 * @param	Current ����ֵ����0x0-0xF����0xF���
 * @return  ��
 * @note	ע�⣬����ֵ�ĵ�����Ӱ��24M�������Ƶ��
 */
void Clock_HOSCCurrentSet(uint32_t Current);

/**
 * @brief	HOSC����ر�
 * @param	��
 * @return  ��
 */
void Clock_HOSCDisable(void);

/**
 * @brief	LOSC����ر�
 * @param	��
 * @return  ��
 */
void Clock_LOSCDisable(void);

/**
 * @brief	ʱ�������GPIO�˿���
 * @param	CLK_GPIO_Port GPIO�������ʱ�Ӷ˿ڣ�GPIOA29��GPIOB6��GPIOB7
 * @param	CLOCK_OUT_MODE ʱ�������Դ
 * @return  ��
 */
void Clock_GPIOOutSel(CLK_GPIO_Port Port, CLOCK_OUT_MODE mode);

/**
 * @brief	�ر�GPIO�˿��ϵ�ʱ���������
 * @param	CLK_GPIO_Port GPIO�������ʱ�Ӷ˿ڣ�GPIOA29��GPIOB6��GPIOB7
 * @return  ��
 */
void Clock_GPIOOutDisable(CLK_GPIO_Port Port);


/**
 * @brief	���λ�ȡRCʱ�ӿ�ʼ
 * @param	��
 * @return  ��
 */
void Clock_RcFreqCntOneTimeStart(void);

/**
 * @brief	���λ�ȡRCʱ�ӿɶ�ȡ״̬
 * @param	��
 * @return  1 �ɶ�
 * 			0 ���ɶ�
 */
bool Clock_RcFreqCntOneTimeReady(void);

/**
 * @brief	���λ�ȡrcƵ��
 * @param	IsReCount �Ƿ��ٴλ�ȡӲ��������ֵ��TRUE���ٴ�����Ӳ����������FALSE����ȡ�ϴμ�¼ֵ��
 * @return  rcƵ�ʣ���λHz
 */
uint32_t Clock_RcFreqCntOneTimeGet(bool IsReCount);

/**
 * @brief	����BB����HOSC�Ĺ��ܣ�Ĭ��Ϊ1
 * @param	0 : �ر�BB����HOSC
 * @param	1 : ����BB����HOSC
 * @return  ��
 */
void Clock_BBCtrlHOSCInDeepsleep(uint8_t set);
uint8_t Clock_BBCtrlHOSCInDeepsleepGet(void);

/**
 * @brief	HOSC��32K�ķ�Ƶϵ��
 * @param	clk_div : ��Ƶϵ��ֵ(����)
 * @return  ��
 */
void Clock_32KClkDivSet(uint32_t clk_div);
uint32_t Clock_32KClkDivGet(void);

/**
 * @brief  Close BT&PMU RC32K clock for deepsleep
 * @param  void
 * @return void
 * @note   none
 */
void Clock_DeepSleepCloseBTPMURC32K();

/**
 * @brief  Restore BT&PMU RC32K clock for wakeup
 * @param  void
 * @return void
 * @note   none
 */
void Clock_WakeupRetoreBTPMURC32K();

/**
 * @brief  Enable RTC LOSC 32K clock
 * @param  void
 * @return void
 * @note   none
 */
void Clock_EnableRTCLOSC32K();

/**
 * @brief  Disable RTC LOSC 32K clock
 * @param  void
 * @return void
 * @note   none
 */
void Clock_DisableRTCLOSC32K();

/**
 * @brief  Enable LOSC 32K clock
 * @param  void
 * @return void
 * @note   none
 */
void Clock_EnableLOSC32K();

/**
 * @brief  Disable LOSC 32K clock
 * @param  void
 * @return void
 * @note   none
 */
void Clock_DisableLOSC32K();

/**
 * @brief  IR Clock select HRC32K in for deepsleep wakeup test
 * @param  void
 * @return void
 * @note   none
 */
void Clock_IRSelHRC32K();

/**
 * @brief  Restore IR Clock selected
 * @param  void
 * @return void
 * @note   none
 */
void Clock_IRRestoreDefaultClk();

/**
 * @brief  RTC_ANA_CTRL
 * @param  RTC_MODE_SEL : 	1: differential Amplifier
 * 							0: single-port  Amplifier
 * @param  RTC_CS : #RTC Osc current control:
 * 					##000~111:240nA+120nA*RTC_CS
 * @return void
 * @note   none
 */
void Clock_RTCLOSC32K_ANA_CTRL(uint8_t RTC_MODE_SEL,uint8_t RTC_CS);

/**
 * @brief	CLOCK_SYNC_CTRLģ��״̬����
 * @param	CLOCK_SYNC_CTRLԴѡ��
 *   @arg	Module:CLOCK_SYNC_CTRLԴ
 *   @arg	Value:����ֵ
 * @return  none
 */
void Clock_SyncCtrl_Set(CLOCK_SYNC_CTRL_MODULE Module, uint32_t Value);

/**
 * @brief	CLOCK_SYNC_CTRLģ��״̬��ȡ
 * @param	CLOCK_SYNC_CTRLԴѡ��
 *   @arg	Module:CLOCK_SYNC_CTRLԴ
 * @return  ģ��״̬
 */
uint32_t Clock_SyncCtrl_Get(CLOCK_SYNC_CTRL_MODULE Module);

/**
 * @brief	CLK_CNTģ�����ֵ��ȡ
 * @param	CLOCK_CLK_CNTԴѡ��
 *   @arg	CLOCK_CLK_CNTԴ
 * @return  ����ֵ
 */
uint32_t Clock_ClkCnt_Get(CLOCK_CLK_CNT_MODULE Module);
#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

/**
 * @}
 * @}
 */
 
