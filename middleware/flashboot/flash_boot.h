#ifndef __FLASH_BOOT_H__
#define __FLASH_BOOT_H__
/*
�汾˵������ǰΪV5.2.0�汾
���ڣ�2024��7��29��
*/
#ifdef CFG_APP_CONFIG
	#include "upgrade.h"
	#include "app_config.h"
	#include "flash_table.h"
#endif

#define FLASH_BOOT_NONE			0 //��flashboot
#define FLASH_BOOT_BASIC		1 //������flashboot
#define FLASH_BOOT_OTA_PRO		2 //Pro�� flashboot

#ifdef CFG_FUNC_BT_OTA_EN
	#define FLASH_BOOT_EN     	FLASH_BOOT_OTA_PRO		//��OTA����
#else
	#define FLASH_BOOT_EN    	FLASH_BOOT_BASIC
#endif

//��Ҫ��debug.h�ж����GPIOһһ��Ӧ
typedef enum __UART_TX
{
	BOOT_DEBUG_TX_A0 = 0,
	BOOT_DEBUG_TX_A1,
	BOOT_DEBUG_TX_A6,
	BOOT_DEBUG_TX_A10,
	BOOT_DEBUG_TX_A18,
	BOOT_DEBUG_TX_A19,
	BOOT_DEBUG_TX_A20,
	BOOT_DEBUG_TX_A21,
	BOOT_DEBUG_TX_A22,
	BOOT_DEBUG_TX_A23,
	BOOT_DEBUG_TX_A24,
	BOOT_DEBUG_TX_A28,
	BOOT_DEBUG_TX_A29,
	BOOT_DEBUG_TX_A30,
	BOOT_DEBUG_TX_A31,
	BOOT_DEBUG_TX_B5
}UART_TX;

//��Ҫ��debug.h�ж���Ĳ�����һһ��Ӧ
typedef enum __UART_BAUDRATE
{
	BOOT_DEBUG_BAUDRATE_9600 = 0,
	BOOT_DEBUG_BAUDRATE_115200,
	BOOT_DEBUG_BAUDRATE_256000,
	BOOT_DEBUG_BAUDRATE_1000000,
	BOOT_DEBUG_BAUDRATE_2000000,
	BOOT_DEBUG_BAUDRATE_3000000,
}UART_BAUDRATE;


//����������
#if CFG_FLASHBOOT_DEBUG_EN

	#define  DEBUG_CONNECT(x, y)			x ## y
	#define  DEBUG_STRING_CONNECT(x, y)		DEBUG_CONNECT(x, y)

	#define CFG_BOOT_UART_BANDRATE   		DEBUG_STRING_CONNECT(BOOT_,CFG_UART_BANDRATE)
	#define CFG_BOOT_UART_TX_PORT    		DEBUG_STRING_CONNECT(BOOT_,CFG_UART_TX_PORT)
	#define BOOT_UART_CONFIG				((CFG_FLASHBOOT_DEBUG_EN<<7) + (CFG_BOOT_UART_BANDRATE<<4) + CFG_BOOT_UART_TX_PORT)

#else
	#define BOOT_UART_CONFIG				0
#endif


/*  JUDGEMENT_STANDARD˵��
 * �ָ�4bit���4bit��
 *   ��4bit��
 *      ΪF��code���汾������
 *      Ϊ5��code��CRC��������
 *   ��4bit:
 *     ΪF��������code��Ҫ�õ����ռ伴�������ռ�
 *     Ϊ5ʱ���ʶ����codeǰȫ������оƬ���ݣ���������ȫƬ����������flash��0��ַ��ʼflashbootռ�ÿռ䲻�����Լ����8K��������
 * ���磺0x5F ��Ϊ�Ƚ�CODE CRC�ж��Ƿ���Ҫ����������ʱ�����ֲ�����������ȫ����
 */
#define JUDGEMENT_STANDARD		0x5F

#if CFG_RES_CARD_GPIO == SDIO_A15_A16_A17
#define SD_PORT				CHN_MASK_SDCARD
#else
#define SD_PORT				CHN_MASK_SDCARD1
#endif

#ifdef CFG_FUNC_BT_OTA_EN
	#define UP_PORT				(CHN_MASK_USBCDC + CHN_MASK_UDISK + SD_PORT + CHN_MASK_BLE) //����Ӧ�������������Щ�����ӿ�
#else
	#define UP_PORT				(CHN_MASK_USBCDC + CHN_MASK_UDISK + SD_PORT) //����Ӧ�������������Щ�����ӿ�
#endif

typedef enum __UART_UP
{
	UART0_UP_TXRX_A0A1 = 0,
	UART0_UP_TXRX_A1A0,
	UART0_UP_TXRX_A6A5,
	UART1_UP_TXRX_A10A9,
	UART1_UP_TXRX_A18A19,
	UART1_UP_TXRX_A19A18,
}UART_UP_PORT;
typedef enum __UART_UP_BAUDRATE
{
	UART_UP_BAUDRATE_115200 = 0,
	UART_UP_BAUDRATE_256000,
	UART_UP_BAUDRATE_1000000,
	UART_UP_BAUDRATE_2000000,
}UART_UP_BAUDRATE;
//��ӡ���ں��������ڲ���ʹ��ͬһ�顣
/* UART_UP_CONFIG ˵��
	bit[3:0]:�˿�����
		�ο� UART_UP_PORT ö�ٲ���
	bit[7:4]: ����������
		�ο� UART_BAUDRATE ö�ٲ���
*/
#define CFG_UART_UP_PORT		UART0_UP_TXRX_A6A5
#define CFG_UART_UP_BAUD_RATE	UART_UP_BAUDRATE_115200
#define UART_UP_CONFIG			((CFG_UART_UP_BAUD_RATE<<4)+CFG_UART_UP_PORT)

/* SPIMFLASH_UP_CONFIG ˵��
	bit[0]: �˿�����
		2��b00:SPIM_PORT0_A5_A6_A7
		2��b01:SPIM_PORT1_A20_A21_A22
	bit[1]: ����
	bit[2]: CSƬѡIO������
		2��b00:GPIO_A_START
		2��b01:GPIO_B_START
	bit[7:3]: CSƬѡIO���� GPIO_INDEX
		0~31: GPIO_INDEX0~GPIO_INDEX31
*/
#define SPIMFLASH_UP_SPIM_PORT	0	//SPIM_PORT0_A5_A6_A7
#define SPIMFLASH_UP_CS_PORT	0	//GPIO_A_START
#define SPIMFLASH_UP_CS_PIN		9	//GPIO_INDEX9
#define SPIMFLASH_UP_CONFIG ((SPIMFLASH_UP_CS_PIN<<3) + (SPIMFLASH_UP_CS_PORT<<2) + SPIMFLASH_UP_SPIM_PORT)

/*  BOOT_SYSTEM_CONFIGURATIOM ˵��
	bit[1:0]:ϵͳʱ������
		2��b00: HSE OSC 24M
		2��b01: RC_32K
	bit[3:2]:����У��ģʽ����
		2��b00: SYMBOL_CHECK,			//�ж�������+����У��
		2��b01: CRC_PART_CODE_CHECK,		//CRC����  user code У��
		2��b10: CRC_ALL_CODE_CHECK,		//CRCȫ��  user code У��
	bit[7:4]:����
 */
typedef enum {
	HSE_OSC_24M,						//�ⲿ24m����
	RC_32K,								//�ڲ�RC32K
}BOOT_CLK_MODE;


#ifdef USB_CRYSTA_FREE_EN
#define BOOT_CLK 	RC_32K
#else
#define BOOT_CLK 	HSE_OSC_24M			//�ⲿ24m����
#endif

#define BOOT_SYSTEM_CONFIGURATIOM		(BOOT_CLK)

#define  CFG_SDK_MAGIC_NUMBER                (0xB0BEBDC9)

#endif

