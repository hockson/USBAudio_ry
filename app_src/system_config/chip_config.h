#ifndef _CHIP_CONFIG_H_
#define _CHIP_CONFIG_H_

//****************************************************************************************
// оƬ��װ����,���������޸�
//****************************************************************************************
#define CFG_CHIP_B5X128			1		//128pin ������
#define CFG_CHIP_BP1524A1		2
#define CFG_CHIP_BP1524A2		3
#define CFG_CHIP_BP1532A1		4
#define CFG_CHIP_BP1532A2		5
#define CFG_CHIP_BP1532B1		6
#define CFG_CHIP_BP1532B2		7
#define CFG_CHIP_BP1548C1		8
#define CFG_CHIP_BP1548C2		9
#define CFG_CHIP_BP1564A1		10
#define CFG_CHIP_BP1564A2		11
#define CFG_CHIP_AP1524A1		12		//������
#define CFG_CHIP_AP1532B1		13		//������
#define CFG_CHIP_BP1532E2		14
#define CFG_CHIP_BP1532E1		15
#define CFG_CHIP_BP1552A2		16
#define CFG_CHIP_BP1552D2		17
#define CFG_CHIP_BP1532F1		18
#define CFG_CHIP_BP1532F2		19
//****************************************************************************************
//       оƬ�ͺ�ѡ������
// ��ͬ�ķ�װ�ĵ�Դ����/����ģ������ȿ��ܻ᲻һ��
// ��������ѡ����Ӧ�ķ�װоƬ���п���
//****************************************************************************************
#define CFG_CHIP_SEL           	CFG_CHIP_BP1532B2 // CFG_CHIP_BP1564A2


//****************************************************************************************
// оƬ��ز����궨�壬���������޸�
//****************************************************************************************
#if (CFG_CHIP_SEL == CFG_CHIP_B5X128)
    #define CHIP_FLASH_CAPACTITY			16						//flash����
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1	//lineinͨ��
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1524A1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1524A2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532A1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_DAC_USE_DIFF										//DACʹ�ò�����
	#define	CHIP_DAC_USE_PVDD16										//DACʹ��PVDD16
	#define	CHIP_USE_DCDC											//ʹ��DCDC
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532A2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_DAC_USE_DIFF										//DACʹ�ò�����
	#define	CHIP_DAC_USE_PVDD16										//DACʹ��PVDD16
	#define	CHIP_USE_DCDC											//ʹ��DCDC
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532B1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC											//ʹ��DCDC
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532B2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC											//ʹ��DCDC
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532E2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC											//ʹ��DCDC
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532E1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1548C1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//ʹ��DCDC
	#define	CHIP_DAC_USE_DIFF
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1548C2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//ʹ��DCDC
	#define	CHIP_DAC_USE_DIFF
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1564A1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//ʹ��DCDC
	#define	CHIP_DAC_USE_DIFF
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1564A2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//ʹ��DCDC
	#define	CHIP_DAC_USE_DIFF
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1552A2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//ʹ��DCDC
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1552D2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
	#define	CHIP_USE_DCDC											//ʹ��DCDC
#elif (CFG_CHIP_SEL == CFG_CHIP_AP1524A1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_BT_DISABLE											//����������
#elif (CFG_CHIP_SEL == CFG_CHIP_AP1532B1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_USE_DCDC
	#define	CHIP_BT_DISABLE											//����������
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532F1)
    #define CHIP_FLASH_CAPACTITY			8
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_DAC_USE_DIFF										//DACʹ�ò�����
	//PVDD������������DVDD16��Ҫ��CHIP_DAC_USE_PVDD16,���������DVDD33��رոú�
	#define	CHIP_DAC_USE_PVDD16										//DACʹ��PVDD16
	#define	CHIP_USE_DCDC											//ʹ��DCDC
#elif (CFG_CHIP_SEL == CFG_CHIP_BP1532F2)
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN2
	#define	CHIP_DAC_USE_DIFF										//DACʹ�ò�����
	//PVDD������������DVDD16��Ҫ��CHIP_DAC_USE_PVDD16,���������DVDD33��رոú�
	#define	CHIP_DAC_USE_PVDD16										//DACʹ��PVDD16
	#define	CHIP_USE_DCDC
#else
    #define CHIP_FLASH_CAPACTITY			16
	#define	CHIP_LINEIN_CHANNEL				ANA_INPUT_CH_LINEIN1
    #error "Undefined Chip type!!!!"
#endif

//****************************************************************************************
// VCOM�����ֱ���������� (����оƬ֧��)
//****************************************************************************************
#if (CFG_CHIP_SEL == CFG_CHIP_BP1532E1) || (CFG_CHIP_SEL == CFG_CHIP_BP1532E2) || (CFG_CHIP_SEL == CFG_CHIP_BP1564A1) || (CFG_CHIP_SEL == CFG_CHIP_BP1564A2)
//	#define CFG_VCOM_DRIVE_EN
#endif

//****************************************************************************************
// оƬRAM�궨�壬���������޸�
//****************************************************************************************
#define CFG_D16K_MEM16K_EN		0		//1 ---> D-Cache�ó�16K��RAM��,D-Cache 16KB
										//0 ---> D-Cache 32KB
#define CFG_D16KMEM16K_RAM_SIZE	(CFG_D16K_MEM16K_EN*16*1024)

#define CFG_CHIP_RAM_SIZE		(256*1024 + CFG_D16KMEM16K_RAM_SIZE)


#endif


