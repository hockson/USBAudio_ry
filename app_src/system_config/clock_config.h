#ifndef _POWER_CONFIG_H__
#define _POWER_CONFIG_H__

#include "app_config.h"

#define CORE_HIGH_MODE				0	//��Ƶ/����ģʽ����Ƶ ���Ľϸ�
#define CORE_USB_CRYSTA_FREE_MODE	1	//�⾧��ģʽ����Ҫ����USB�������ܲ�����������
#define CORE_ONLY_APLL_MODE			2	//��APLLģʽ���ر�DPLL����Ҫ�ر�BT�����Ժ�ʹ��
#define CORE_USER_MODE				3	//
#define CORE_SPDIF_OUT_MODE			4	//spdifʹ��DPLL,����ʱ��ʹ��APLL

//����ϵͳģʽ
#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	#define SYS_CORE_SET_MODE			CORE_SPDIF_OUT_MODE
#else
	#define SYS_CORE_SET_MODE			CORE_HIGH_MODE//CORE_ONLY_APLL_MODE  //george
#endif

#define SYS_CRYSTAL_FREQ			24*1000*1000	//����Ƶ�� Hz

#if (SYS_CORE_SET_MODE == CORE_USB_CRYSTA_FREE_MODE)
	#ifndef  CFG_APP_USB_AUDIO_MODE_EN
		#error	USB_AUDIO_MODE is not turned on!!!
	#else
		//usb�����⾧��ģʽ/��Ҫ����USB�������ܲ�����������
		#define USB_CRYSTA_FREE_EN
		//�����Ҫ��Ƶ����Ҫ����Ϊ48M����������USBʱ��Ƶ��Ϊ48M��
		//USBĬ��5��Ƶ��Clock_USBClkDivSet(5);
		//����DPLL_FREQΪ288M����Ҫ��Ӧ����USBƵ�ʣ�Clock_USBClkDivSet(6);
		#define	SYS_CORE_DPLL_FREQ			240*1000		//kHZ

		#define SYS_CORE_CLK_SELECT			PLL_CLK_MODE
		//Note: USB��UARTʱ������DPLL��APLL������ͬһ��ʱ��,����UART���Ե���ѡ��RC
		#define SYS_UART_CLK_SELECT			PLL_CLK_MODE
		#define SYS_USB_CLK_SELECT			PLL_CLK_MODE
		#define SYS_SPDIF_CLK_SELECT		PLL_CLK_MODE

		//flash ʱ�����ã�Ƶ�ʱ���ΪAPLL_FREQ/DPLL_FREQ������Ƶ��������96M��
		#define SYS_FLASH_CLK_SELECT		FSHC_PLL_CLK_MODE
		#define SYS_FLASH_FREQ_SELECT		((SYS_CORE_DPLL_FREQ/3)*1000) 	//Hz

		//��Ƶʱ��
		#define	SYS_AUDIO_CLK_SELECT		PLL_CLK_MODE
	#endif
#elif (SYS_CORE_SET_MODE == CORE_ONLY_APLL_MODE)
	#define	SYS_CORE_APLL_FREQ			240*1000		//kHZ
	#define	SYS_CORE_DPLL_FREQ			240*1000		//kHZ

	#define SYS_CORE_CLK_SELECT			APLL_CLK_MODE
	//Note: USB��UARTʱ������DPLL��APLL������ͬһ��ʱ��,����UART���Ե���ѡ��RC
	#define SYS_UART_CLK_SELECT			APLL_CLK_MODE
	#define SYS_USB_CLK_SELECT			APLL_CLK_MODE
	#define SYS_SPDIF_CLK_SELECT		APLL_CLK_MODE

	//flash ʱ�����ã�Ƶ�ʱ���ΪAPLL_FREQ/DPLL_FREQ������Ƶ��������96M��
	#define SYS_FLASH_CLK_SELECT		FSHC_APLL_CLK_MODE
	#define SYS_FLASH_FREQ_SELECT		((SYS_CORE_APLL_FREQ/3)*1000) 	//Hz

	//��Ƶʱ��
	#define	SYS_AUDIO_CLK_SELECT		APLL_CLK_MODE
#elif (SYS_CORE_SET_MODE == CORE_SPDIF_OUT_MODE)

	#define	SYS_CORE_APLL_FREQ			240*1000		//kHZ
	#define	SYS_CORE_DPLL_FREQ			240*1000		//kHZ only allow 240M��264M and 288M

	#define SYS_CORE_CLK_SELECT			APLL_CLK_MODE //APLL_CLK_MODE
	//Note: USB��UARTʱ������DPLL��APLL������ͬһ��ʱ��,����UART���Ե���ѡ��RC
	#define SYS_UART_CLK_SELECT			APLL_CLK_MODE//APLL_CLK_MODE
	#define SYS_USB_CLK_SELECT			APLL_CLK_MODE //APLL_CLK_MODE
	#define SYS_SPDIF_CLK_SELECT		APLL_CLK_MODE //APLL_CLK_MODE
	//flash ʱ�����ã�Ƶ�ʱ���ΪAPLL_FREQ/DPLL_FREQ������Ƶ��������96M��
	#define SYS_FLASH_CLK_SELECT		FSHC_APLL_CLK_MODE //FSHC_APLL_CLK_MODE
	#define SYS_FLASH_FREQ_SELECT		((SYS_CORE_APLL_FREQ/3)*1000) 	//Hz

	//��Ƶʱ��
	#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
		#define	SYS_AUDIO_CLK_SELECT		PLL_CLK_MODE
	#else
		#define	SYS_AUDIO_CLK_SELECT		APLL_CLK_MODE
	#endif

#else
	#define	SYS_CORE_APLL_FREQ			240*1000		//kHZ
	
	#if (SYS_CORE_SET_MODE == CORE_HIGH_MODE) || defined(CFG_AI_DENOISE_EN)
		#define	SYS_CORE_DPLL_FREQ			(360*1000) //288*1000		//kHZ only allow 240M��264M and 288M
	#else
		#define	SYS_CORE_DPLL_FREQ			240*1000		//kHZ only allow 240M��264M and 288M
	#endif

	#define SYS_CORE_CLK_SELECT			PLL_CLK_MODE //APLL_CLK_MODE
	//Note: USB��UARTʱ������DPLL��APLL������ͬһ��ʱ��,����UART���Ե���ѡ��RC
	#define SYS_UART_CLK_SELECT			APLL_CLK_MODE//APLL_CLK_MODE
	#define SYS_USB_CLK_SELECT			APLL_CLK_MODE //APLL_CLK_MODE
	#define SYS_SPDIF_CLK_SELECT		APLL_CLK_MODE //APLL_CLK_MODE
	//flash ʱ�����ã�Ƶ�ʱ���ΪAPLL_FREQ/DPLL_FREQ������Ƶ��������96M��
	#define SYS_FLASH_CLK_SELECT		FSHC_PLL_CLK_MODE //FSHC_APLL_CLK_MODE
	#define SYS_FLASH_FREQ_SELECT		((SYS_CORE_DPLL_FREQ/4)*1000) 	//Hz ((SYS_CORE_DPLL_FREQ/3)*1000) 	//Hz

	//��Ƶʱ��
	#define	SYS_AUDIO_CLK_SELECT		APLL_CLK_MODE
#endif

#if (SYS_FLASH_FREQ_SELECT > 96*1000*1000)
	//flashʱ��Ƶ�ʲ��ܴ���96M
	#error	flash frequency error!!!
#endif

#endif



