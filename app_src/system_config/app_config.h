/******************************************************************************
 * @file    app_config.h
 * @author
 * @version V_NEW
 * @date    2021-06-25
 * @maintainer
 * @brief
 ******************************************************************************
 * @attention
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include "type.h"
#include "chip_config.h"
#include "spi_flash.h"

//************************************************************************************************************
//    ��ϵͳĬ�Ͽ���2��ϵͳȫ�ֺ꣬��IDE��������(Build Settings-Compiler-Symbols)���˴���������
//*CFG_APP_CONFIG �� FUNC_OS_EN*/
//************************************************************************************************************

//************************************************************************************************************
//    ���ܿ���˵����
// *CFG_APP_*  : ���ϵͳӦ��ģʽ����������USB U�̲��Ÿ���Ӧ��ģʽ��ѡ��
// *CFG_FUNC_* : ������ܿ���
// *CFG_PARA_* : ϵͳ�����������
// *CFG_RES_*  : ϵͳӲ����Դ����
// ************************************************************************************************************

//24bit ��Ч���غ����
//24bit SDK RAM��������������
#define  CFG_AUDIO_WIDTH_24BIT

/**��ƵSDK�汾�ţ�V1.0.0**/
/*0xB5��оƬB15X��01����汾�ţ� 00��С�汾�ţ� 00���û��޶��ţ����û��趨���ɽ�ϲ����ţ���
 *ʵ�ʴ洢�ֽ���1A 01 00 00 ����������sdk�汾*/
/*����flash_bootʱ����flashboot����usercode��boot��������code����(��0xB8��0xB8+0x10000)����ֵ�᲻ͬ��
 * ǰ����burner��¼ʱ�汾��������mva�汾���ע*/
#define	 CFG_SDK_VER_CHIPID			(0xB5)
#define  CFG_SDK_MAJOR_VERSION	0 //	(0)
#define  CFG_SDK_MINOR_VERSION	6 //	(5)
#define  CFG_SDK_PATCH_VERSION	7  //	(0)


#define CFG_RONGYUAN_CMD


#define MODEL_DEMO		0		
#define MODEL_G1		1		
#define MODEL_GS20		2
#define MODEL_S6		3

#define CUSTOM_MODEL	MODEL_GS20 // MODEL_S6

#define GPIOA_INIT(x)  	    GPIO_PortAModeSet(x, 0),\
							GPIO_RegOneBitClear(GPIO_A_PU, x),\
							GPIO_RegOneBitClear(GPIO_A_PD, x),\
							GPIO_RegOneBitSet(GPIO_A_OE, x),\
							GPIO_RegOneBitClear(GPIO_A_IE, x)
							
#define GPIOB_INIT(x)  	    GPIO_PortBModeSet(x, 0),\
							GPIO_RegOneBitClear(GPIO_B_PU, x),\
							GPIO_RegOneBitClear(GPIO_B_PD, x),\
							GPIO_RegOneBitSet(GPIO_B_OE, x),\
							GPIO_RegOneBitClear(GPIO_B_IE, x)

#if CUSTOM_MODEL == MODEL_G1
#define EXT_MUTE_PIN		GPIOA16

#define EXT_MUTE_INIT()  	    GPIOA_INIT(EXT_MUTE_PIN)

#define EXT_MUTE_ON()           		GPIO_RegOneBitClear(GPIO_A_OUT, EXT_MUTE_PIN)
#define EXT_MUTE_OFF()           	GPIO_RegOneBitSet(GPIO_A_OUT, EXT_MUTE_PIN)

#elif CUSTOM_MODEL == MODEL_GS20
#define LED1_PIN		GPIOA17
#define LED2_PIN		GPIOA16
#define LED3_PIN		GPIOA15
#define LED4_PIN		GPIOA7

#define DISP_LED1_INIT()  	    GPIOA_INIT(LED1_PIN)
#define DISP_LED1_ON()           	GPIO_RegOneBitClear(GPIO_A_OUT, LED1_PIN)
#define DISP_LED1_OFF()           	GPIO_RegOneBitSet(GPIO_A_OUT, LED1_PIN)


#define DISP_LED2_INIT()  	    GPIOA_INIT(LED2_PIN)
#define DISP_LED2_ON()           	GPIO_RegOneBitClear(GPIO_A_OUT, LED2_PIN)
#define DISP_LED2_OFF()           	GPIO_RegOneBitSet(GPIO_A_OUT, LED2_PIN)

#define DISP_LED3_INIT()  	    GPIOA_INIT(LED3_PIN)
#define DISP_LED3_ON()           	GPIO_RegOneBitClear(GPIO_A_OUT, LED3_PIN)
#define DISP_LED3_OFF()           	GPIO_RegOneBitSet(GPIO_A_OUT, LED3_PIN)

#define DISP_LED4_INIT()  	    GPIOA_INIT(LED4_PIN)
#define DISP_LED4_ON()           	GPIO_RegOneBitClear(GPIO_A_OUT, LED4_PIN)
#define DISP_LED4_OFF()           	GPIO_RegOneBitSet(GPIO_A_OUT, LED4_PIN)

//#define  CFG_FUNC_DETECT_PHONE_EN
//#define  DET_PHONE_INPUT_PIN	GPIOA20
#define  EXT_AMP_MUTE_PIN		GPIOA6

#define CFG_RES_IO_KEY_SCAN

#elif CUSTOM_MODEL == MODEL_S6
#define LED1_PIN		GPIOA17
#define LED2_PIN		GPIOA16

#define DISP_LED1_INIT()  	    GPIOA_INIT(LED1_PIN)
#define DISP_LED1_ON()           	GPIO_RegOneBitClear(GPIO_A_OUT, LED1_PIN)
#define DISP_LED1_OFF()           	GPIO_RegOneBitSet(GPIO_A_OUT, LED1_PIN)


#define DISP_LED2_INIT()  	    GPIOA_INIT(LED2_PIN)
#define DISP_LED2_ON()           	GPIO_RegOneBitClear(GPIO_A_OUT, LED2_PIN)
#define DISP_LED2_OFF()           	GPIO_RegOneBitSet(GPIO_A_OUT, LED2_PIN)

#define CFG_RES_IO_KEY_SCAN

#endif

#define EQ_OFF		0
#define EQ_CLASSIC	1
#define EQ_POP		2
#define EQ_ROCK		3
#define EQ_JAZZ		4
#define EQ_CUST1		5
#define EQ_CUST2		6
#define EQ_CUST3		7
#define EQ_MAX_NUM		8

//****************************************************************************************
// ϵͳApp����ģʽѡ��
//****************************************************************************************
#define CFG_APP_IDLE_MODE_EN
#define CFG_APP_BT_MODE_EN
//#define	CFG_APP_USB_PLAY_MODE_EN
//#define	CFG_APP_CARD_PLAY_MODE_EN
//#define	CFG_APP_LINEIN_MODE_EN
//#define CFG_APP_RADIOIN_MODE_EN
#define CFG_APP_USB_AUDIO_MODE_EN
//#define CFG_APP_I2SIN_MODE_EN
//#define	CFG_APP_OPTICAL_MODE_EN	// SPDIF ����ģʽ
//#define CFG_APP_COAXIAL_MODE_EN	// SPDIF ͬ��ģʽ
//#define CFG_APP_HDMIIN_MODE_EN

#ifdef CHIP_BT_DISABLE
	#undef CFG_APP_BT_MODE_EN
#endif

#define CFG_FUNC_OPEN_SLOW_DEVICE_TASK

#ifdef CFG_APP_LINEIN_MODE_EN
	#define LINEIN_INPUT_CHANNEL				(CHIP_LINEIN_CHANNEL)
#endif

#ifdef CFG_APP_RADIOIN_MODE_EN
    #define FUNC_RADIO_RDA5807_EN
    //#define FUNC_RADIO_QN8035_EN  //оƬioʱ��ֻ֧��12M��qn8035������Ҿ���
#if defined(FUNC_RADIO_RDA5807_EN) && defined(FUNC_RADIO_QN8035_EN)
   #error Conflict: radio type error //����ͬʱѡ��������ʾģʽ
#endif
#endif

/**USB��������������һ��ͨ���� **/
#ifdef  CFG_APP_USB_AUDIO_MODE_EN
	#define CFG_PARA_USB_MODE	AUDIO_MIC

	#define CFG_PARA_AUDIO_USB_IN_SYNC	//ʱ��ƫ������� ������ͬ��
	#define CFG_PARA_AUDIO_USB_IN_SRC	//ת����׼��

	#define CFG_PARA_AUDIO_USB_OUT_SYNC	//ʱ��ƫ������� ������ͬ��
	#define CFG_PARA_AUDIO_USB_OUT_SRC	//ת����׼��
	#define CFG_RES_AUDIO_USB_VOL_SET_EN
#endif

//IDLEģʽ(�ٴ���),powerkey/deepsleep����ͬʱѡ��Ҳ���Ե�������
//ͨ����Ϣ���벻ͬ��ģʽ
//MSG_DEEPSLEEP/MSG_POWER/MSG_POWERDOWN --> ����IDLEģʽ(�ٴ���)
//MSG_DEEPSLEEP --> ����IDLEģʽ�Ժ����CFG_IDLE_MODE_DEEP_SLEEP�򿪽���deepsleep
//MSG_POWERDOWN --> ����IDLEģʽ�Ժ����CFG_IDLE_MODE_POWER_KEY�򿪽���powerdown
#ifdef  CFG_APP_IDLE_MODE_EN
	//#define CFG_IDLE_MODE_POWER_KEY	//power key
	#define CFG_IDLE_MODE_DEEP_SLEEP //deepsleep
	#ifdef CFG_IDLE_MODE_POWER_KEY
		#define POWERKEY_MODE		POWERKEY_MODE_PUSH_BUTTON
		#if(POWERKEY_MODE==POWERKEY_MODE_PUSH_BUTTON)
			//#define POWERKEY_FIRST_ENTER_POWERDOWN		//��һ���ϵ���Ҫ����PowerKey
		#endif
	#endif
	#ifdef CFG_IDLE_MODE_DEEP_SLEEP
		/*���ⰴ������,ע��CFG_PARA_WAKEUP_GPIO_IR�� ���Ѽ�IR_KEY_POWER����*/
		#define CFG_PARA_WAKEUP_SOURCE_IR		SYSWAKEUP_SOURCE11_IR//�̶�source11
		/*ADCKey���� ���CFG_PARA_WAKEUP_GPIO_ADCKEY ���Ѽ�ADCKEYWAKEUP���ü����ƽ*/
		#define CFG_PARA_WAKEUP_SOURCE_ADCKEY	SYSWAKEUP_SOURCE1_GPIO//��ʹ��ADC���ѣ������CFG_RES_ADC_KEY_SCAN ��CFG_RES_ADC_KEY_USE
		#define CFG_PARA_WAKEUP_SOURCE_POWERKEY SYSWAKEUP_SOURCE6_POWERKEY
		#define CFG_PARA_WAKEUP_SOURCE_IOKEY1	SYSWAKEUP_SOURCE3_GPIO
		#define CFG_PARA_WAKEUP_SOURCE_IOKEY2	SYSWAKEUP_SOURCE4_GPIO
	#endif
#endif

#define CFG_RES_MIC_SELECT      (1)//0=NO MIC, 1= MIC


//****************************************************************************************
//                 ��Ƶ���ͨ���������
//˵��:
//    �������Դ��ͬʱ�����
//****************************************************************************************
/**DACͨ������ѡ��**/
#define CFG_RES_AUDIO_DAC0_EN

/**I2S��Ƶ���ͨ������ѡ��**/
//#define CFG_RES_AUDIO_I2SOUT_EN

/**����ͬ����Ƶ���ͨ������ѡ��**/
//#define CFG_RES_AUDIO_SPDIFOUT_EN
#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	#define SPDIF_OUT_NUM			SPDIF1
	#define SPDIF_OUT_DMA_ID		PERIPHERAL_ID_SPDIF1_TX
//	#define CFG_I2S_SLAVE_TO_SPDIFOUT_EN	//Just support I2S1 and AUDIO_CLK must select DPLL
	#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
		#define CFG_FUNC_EFFECT_BYPASS_EN
		#undef 	CFG_RES_AUDIO_DAC0_EN
		#undef 	CFG_RES_MIC_SELECT
		#define CFG_RES_MIC_SELECT      (0)
	#endif
#endif

//#define CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
//****************************************************************************************
//     I2S�������ѡ��
//˵��:
//    1.I2S���Ҳʹ��ʱ�˿�ѡ���ģʽ��Ҫע�Ᵽ��һ��;
//	  2��ȱʡ����ΪMaster��I2S������Ƶʹ��Master��MCLK,����Է���΢��ʱ�Ӿͻ���pop����
//    3.��������I2S slave��I2S��������ṩ�ȶ�ʱ�ӣ�����������߹������ȶ���������AudioIOSet.Sync = FALSE����������DetectLock����Sync
//****************************************************************************************
#if defined(CFG_APP_I2SIN_MODE_EN) || defined(CFG_RES_AUDIO_I2SOUT_EN)
#include"i2s.h"
//i2s gpio���ã����붼���ó�i2s1����i2s0
#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
	#define I2S_MCLK_GPIO					I2S1_MCLK_IN_A6 //ѡ��MCLK_OUT/MCLK_IN�ţ�I2S�Զ����ó�master/slave
	#define I2S_LRCLK_GPIO					I2S1_LRCLK_A7
	#define I2S_BCLK_GPIO					I2S1_BCLK_A9
	#ifdef CFG_RES_AUDIO_I2SOUT_EN
		#define I2S_DOUT_GPIO				I2S1_DOUT_A30
	#endif
	#ifdef CFG_APP_I2SIN_MODE_EN
		#define I2S_DIN_GPIO				I2S1_DIN_A10
	#endif
#else
	#define I2S_MCLK_GPIO					I2S0_MCLK_OUT_A24 //ѡ��MCLK_OUT/MCLK_IN�ţ�I2S�Զ����ó�master/slave
	#define I2S_LRCLK_GPIO					I2S0_LRCLK_A20
	#define I2S_BCLK_GPIO					I2S0_BCLK_A21
	#ifdef CFG_RES_AUDIO_I2SOUT_EN
		#define I2S_DOUT_GPIO				I2S0_DOUT_A22
	#endif
	#ifdef CFG_APP_I2SIN_MODE_EN
		#define I2S_DIN_GPIO				I2S0_DIN_A23
	#endif
#endif
#define CFG_RES_I2S_MODE				GET_I2S_MODE(I2S_MCLK_GPIO)		//����I2S_MCLK_GPIO�Զ�����master/slave
																		//Ҳ�����ֶ����ó�1����0  0:master mode ;1:slave mode
#define	CFG_RES_I2S_MODULE				GET_I2S_I2S_PORT(I2S_MCLK_GPIO)	//����I2S_MCLK_GPIO�Զ�����i2s1/i2s0
																		//Ҳ�����ֶ����ó�1����0  0 ---> i2s0  1 ---> i2s1
#define CFG_PARA_I2S_SAMPLERATE			44100
#define CFG_FUNC_I2S_IN_SYNC_EN			//ȱʡΪSRA
#define CFG_FUNC_I2S_OUT_SYNC_EN
#endif

//****************************************************************************************
//                 ����������ѡ��
//˵��:
//    ���½���������ѡ���Ӱ��code size;
//****************************************************************************************
//�򿪺�֧�ָ߲����ʽ��룬��Դ���Ľϴ���������������֧��ape/flac/wav��
//����ͬ������CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000 ������ת��������������
//#define LOSSLESS_DECODER_HIGH_RESOLUTION

#define USE_MP3_DECODER
//#define USE_WMA_DECODER
#define USE_SBC_DECODER
//#define USE_WAV_DECODER
//#define USE_DTS_DECODER
//#define USE_FLAC_DECODER	//24bit 1.5Mbps������ʱ����Ҫ����DECODER_FIFO_SIZE_FOR_PLAYER ���fifo�����������룺FLAC_INPUT_BUFFER_CAPACITY
#define USE_AAC_DECODER
//#define USE_AIF_DECODER
//#define USE_AMR_DECODER
//#define USE_APE_DECODER

//****************************************************************************************
//                 ����Ч��������
//****************************************************************************************
//�ߵ������ڹ�������˵��:
//    1.�˹����ǻ���MIC OUT EQ�����ֶ����õģ���Ҫ�ڵ���������ʹ�ܴ�EQ��
//    2.Ĭ��f5��Ӧbass gain,f6��Ӧtreb gain,�������������޸Ĵ�EQ filter��Ŀ����Ҫ��Ӧ�޸�BassTrebAjust()�ж�Ӧ��ţ�
//EQģʽ��������˵��:
//    1.�˹����ǻ���MUSIC EQ�����ֶ����õģ���Ҫ�ڵ���������ʹ�ܴ�EQ��
//    2.����flat/classic/pop/jazz/pop/vocal boost֮��ͨ��KEY���л�   
#if CFG_RES_MIC_SELECT
//	#define	CFG_FUNC_MIC_KARAOKE_EN      //MIC karaoke����ѡ��
#endif

#ifdef CFG_FUNC_MIC_KARAOKE_EN
//Line in mix for Karaoke
#ifndef CFG_APP_LINEIN_MODE_EN
//	#define CFG_FUNC_LINEIN_MIX_MODE
#endif
// I2S mix mode for Karaoke
//#define CFG_RES_AUDIO_I2S_MIX_OUT_EN
//#define CFG_RES_AUDIO_I2S_MIX_IN_EN
//#define CFG_RES_AUDIO_I2S_MIX2_OUT_EN
//#define CFG_RES_AUDIO_I2S_MIX2_IN_EN
//USB Audio mix for Karaoke
#ifdef CFG_APP_USB_AUDIO_MODE_EN
//	#define CFG_FUNC_USB_AUDIO_MIX_MODE //USB Audio mix��Ҫ����USB_AUDIO_MODE
#endif
#if defined(CFG_RES_AUDIO_I2S_MIX_IN_EN) || defined(CFG_RES_AUDIO_I2S_MIX_OUT_EN)
	#define CFG_FUNC_I2S_MIX_MODE
	#include"i2s.h"
	#define I2S_MIX_MCLK_GPIO					I2S1_MCLK_OUT_A6 //ѡ��MCLK_OUT/MCLK_IN�ţ�I2S�Զ����ó�master/slave
	#define I2S_MIX_LRCLK_GPIO					I2S1_LRCLK_A7
	#define I2S_MIX_BCLK_GPIO					I2S1_BCLK_A9

#ifdef CFG_RES_AUDIO_I2S_MIX_IN_EN
	#define I2S_MIX_DIN_GPIO				I2S1_DIN_A10
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
	#define I2S_MIX_DOUT_GPIO				I2S1_DOUT_A10
#endif

	#define CFG_RES_MIX_I2S_MODE				GET_I2S_MODE(I2S_MIX_MCLK_GPIO)		//����I2S_MCLK_GPIO�Զ�����master/slave
	#define	CFG_RES_MIX_I2S_MODULE				GET_I2S_I2S_PORT(I2S_MIX_MCLK_GPIO)	//����I2S_MCLK_GPIO�Զ�����i2s1/i2s0
	#define CFG_PARA_MIX_I2S_SAMPLERATE		44100
	#define CFG_FUNC_MIX_I2S_IN_SYNC_EN		//ȱʡΪSRA
	#define CFG_FUNC_MIX_I2S_OUT_SYNC_EN
#endif

#if defined(CFG_RES_AUDIO_I2S_MIX2_IN_EN) || defined(CFG_RES_AUDIO_I2S_MIX2_OUT_EN)
	#undef 	CFG_RES_AUDIO_I2SOUT_EN
	#define CFG_FUNC_I2S_MIX2_MODE
	#include"i2s.h"
	#define I2S_MIX2_MCLK_GPIO					I2S0_MCLK_OUT_A24 //ѡ��MCLK_OUT/MCLK_IN�ţ�I2S�Զ����ó�master/slave
	#define I2S_MIX2_LRCLK_GPIO					I2S0_LRCLK_A20
	#define I2S_MIX2_BCLK_GPIO					I2S0_BCLK_A21

#ifdef CFG_RES_AUDIO_I2S_MIX2_IN_EN
	#define I2S_MIX2_DIN_GPIO				I2S0_DIN_A22
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX2_OUT_EN
	#define I2S_MIX2_DOUT_GPIO				I2S0_DOUT_A23
#endif

	#define CFG_RES_MIX2_I2S_MODE				GET_I2S_MODE(I2S_MIX2_MCLK_GPIO)		//����I2S_MCLK_GPIO�Զ�����master/slave
	#define	CFG_RES_MIX2_I2S_MODULE				GET_I2S_I2S_PORT(I2S_MIX2_MCLK_GPIO)	//����I2S_MCLK_GPIO�Զ�����i2s1/i2s0
	#define CFG_PARA_MIX2_I2S_SAMPLERATE		44100
	#define CFG_FUNC_MIX2_I2S_IN_SYNC_EN		//ȱʡΪSRA
	#define CFG_FUNC_MIX2_I2S_OUT_SYNC_EN
#endif
#endif

#define CFG_FUNC_AUDIO_EFFECT_EN //����Чʹ�ܿ���
#ifdef CFG_FUNC_AUDIO_EFFECT_EN

	#define CFG_FUNC_AUDIOEFFECT_AUTO_GEN_MSG_PROC		//�Զ�������Ч���ƴ���
	//#define CFG_FUNC_EFFECT_BYPASS_EN		//������Ĭ������bypass��Ч��ͼ��������Ƶָ�����
	#ifdef CFG_FUNC_EFFECT_BYPASS_EN
		#undef CFG_FUNC_MIC_KARAOKE_EN
	#endif

    //#define CFG_FUNC_ECHO_DENOISE          //�������ٵ���delayʱ��������
 	#define CFG_FUNC_MUSIC_EQ_MODE_EN     //Music EQģʽ��������

	#ifdef CFG_FUNC_MIC_KARAOKE_EN
		#define CFG_FUNC_MIC_TREB_BASS_EN    	//Mic�ߵ������ڹ�������
		#define CFG_FUNC_MUSIC_TREB_BASS_EN    //Music�ߵ������ڹ�������
		//���ܹ���ѡ������
		//ע:����Ҫ������Ƶ��������౸��������⣬��Ҫ�����˹���(����MIC�źż��ӿڴ���)
	//	#define  CFG_FUNC_SHUNNING_EN
			#define SHNNIN_VALID_DATA                          	 500  ////MIC������ֵ
			#define SHNNIN_STEP                                  1  /////���ε��ڵĲ�����ӦVolArr�е�һ��
			#define SHNNIN_THRESHOLD                             SHNNIN_STEP*10  ////threshold
			#define SHNNIN_VOL_RECOVER_TIME                      50////���������ָ�ʱ����50*20ms = 1s
			#define SHNNIN_UP_DLY                                3/////��������ʱ��
			#define SHNNIN_DOWN_DLY                              1/////�����½�ʱ��

			#define CFG_FLOWCHART_KARAOKE_ENABLE
	#endif
    //#define CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN     //���ź��Զ��ػ����ܣ�
    #ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN      
		#define  SILENCE_THRESHOLD                 120        //�����źż�����ޣ�С�����ֵ��Ϊ���ź�
		#define  SILENCE_POWER_OFF_DELAY_TIME      10*60*1000 //���źŹػ���ʱʱ��:10Min����λ��ms
    #endif

	#define CFG_FUNC_AUDIO_EFFECT_ONLINE_TUNING_EN//���ߵ���
	#ifdef CFG_FUNC_AUDIO_EFFECT_ONLINE_TUNING_EN
		#define  CFG_COMMUNICATION_BY_USB			//���ߵ���Ӳ���ӿ�USB HID
		#define  CFG_COMMUNICATION_CRYPTO						(0)////����ͨѶ����=1 ����ͨѶ������=0
		#define  CFG_COMMUNICATION_PASSWORD                     0x11223344//////���ֽڵĳ�������
	#endif

	//ʹ��flash��õĵ�������
	//��Ч�����洢��flash�̶�������
//	#define CFG_EFFECT_PARAM_IN_FLASH_EN
	#ifdef CFG_EFFECT_PARAM_IN_FLASH_EN
		#define CFG_EFFECT_PARAM_IN_FLASH_SIZE			(16)//KB���������Ч�����������ص�flash�ռ�
		#ifdef CFG_FUNC_AUDIO_EFFECT_ONLINE_TUNING_EN
			#define CFG_EFFECT_PARAM_UPDATA_BY_ACPWORKBENCH
		#endif
	#endif

#endif

//****************************************************************************************
//     ת��������ѡ��
//˵��:
//    1.ʹ�ܸú��ʾϵͳ�Ὣ����ͳһ�Ĳ����������Ĭ��ʹ��44.1KHz;
//    2.�˰汾Ĭ�ϴ򿪣�����ر�!!!!!!!!!!
//****************************************************************************************	
#define	CFG_FUNC_MIXER_SRC_EN

//     ������Ӳ��΢������ѡ��
//˵��:
//	     Ӳ��΢��ͬһʱ��ֻ��ʹ�ܿ���һ��΢����ʹϵͳAUPLLʱ�Ӹ�����Դ
//****************************************************************************************	
//#define	CFG_FUNC_FREQ_ADJUST
#ifdef CFG_FUNC_FREQ_ADJUST
	#define CFG_PARA_BT_FREQ_ADJUST		//Btplay ģʽ�����ڼ� Ӳ��΢������CFG_PARA_BT_SYNC�����
 	#define CFG_PARA_HFP_FREQ_ADJUST	//ͨ��ģʽ �����ڼ� Ӳ��΢��  ʹ������΢�������и��档 ��CFG_PARA_HFP_SYNC���
#endif

//****************************************************************************************
//                 ADC/DAC/I2S mclk��������
//˵��:
//    1.���º꿪������ڵ����������ߵ���ADC,DAC,I2Sʱ�����ó�ʼ��mclk
//    2.�ù�����CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000��ͻ
//****************************************************************************************
#ifndef CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
//	#define	CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
#endif
//****************************************************************************************
//                 ¼����������
//˵��:CFG_FUNC_RECORD_EXTERN_FLASH_EN	//¼�ƶ����ʾ��������flash/�ڲ�flash  ������Ч��¼�ƺͲ���
//****************************************************************************************
//#define CFG_FUNC_RECORDER_EN
#ifdef CFG_FUNC_RECORDER_EN
	#define CFG_FUNC_RECORD_SD_UDISK	//¼����SD������U��
	//¼�ƶ����ʾ��������flash/�ڲ�flash������Ч��¼�ƺͲ��ţ���CFG_FUNC_RECORD_SD_UDISKֻ��2ѡ1
	//�ر�USE_EXTERN_FLASH_SPACE�꣬¼����Ч�����ڲ�flash������Ҫ�� \BT_Audio_APP\tools\merge_script\merge.ini���õ�ַ
	//merge.ini�п����������ã�	maxlen������Ҫ����Ϊ CFG_PARA_RECORDS_MAX_SIZE * CFG_PARA_RECORDS_INDEX
	//	[rec_data]
	//	virtual = 1
	//	fullpath=..\merge_script\rec_data.bin
	//	maxlen = 0x30000
	//	enable = 1
//	#define	CFG_FUNC_RECORD_EXTERN_FLASH_EN

	#ifdef CFG_FUNC_RECORD_SD_UDISK
		#define CFG_FUNC_RECORD_UDISK_FIRST				//U�̺Ϳ�ͬʱ����ʱ��¼���豸����ѡ��U�̣���������ѡ��¼����SD����
		#define CFG_PARA_RECORDS_FOLDER 		"REC"	//¼��¼U��ʱ��Ŀ¼�ļ��С�ע��ffpresearch_init ʹ�ûص����������ַ�����
		#define CFG_FUNC_RECORDS_MIN_TIME		1000	//��λms�������˺��С��������ȵ��Զ�ɾ����
		#define CFG_PARA_REC_MAX_FILE_NUM       256     //¼���ļ������Ŀ

		#define MEDIAPLAYER_SUPPORT_REC_FILE            // U�̻�TF��ģʽ�£��򿪴˹��ܣ���֧�ֲ���¼���ļ�������ֻ����¼���ط�ģʽ�²���¼���ļ�
        //#define AUTO_DEL_REC_FILE_FUNCTION            //¼���ļ��ﵽ��������Զ�ɾ��ȫ��¼���ļ��Ĺ���ѡ��
	#endif

	#define DEL_REC_FILE_EN

	#ifdef CFG_FUNC_RECORD_EXTERN_FLASH_EN
		//#define USE_EXTERN_FLASH_SPACE		//ʹ������flash�ռ�(��������ʹ��оƬ����flash)
		#define CFG_PARA_RECORDS_MAX_SIZE			(0x30000) 	// ����Ϊһ��¼���ļ��Ĵ�С192K
		#define CFG_PARA_RECORDS_INFO_SIZE			256				// ����256BYTE�ռ䣬��������һЩ�뱣���¼����Ϣ������¼��ʱ����¼����С�ȵȵ�
		#define	EXTERN_FLASH_RECORDER_FILE_SECOND	30				//�����ļ�¼��ʱ��
		#ifdef USE_EXTERN_FLASH_SPACE
			#define CFG_PARA_RECORDS_INDEX				4			// ������������¼������
			#define	CFG_PARA_RECORDS_FLASH_BEGIN_ADDR	0			// ¼���ļ�����ʼ��ַ
			#define	SpiWrite(a,b,c)						SPI_Flash_Write(a,b,c)
			#define	SpiRead(a,b,c)						SPI_Flash_Read(a,b,c)
			#define SpiErase(a)							SPI_Flash_Erase_4K(a)
		#else
			#define CFG_PARA_RECORDS_INDEX				1			// ������������¼������
			#define	SpiWrite(a,b,c)						SpiFlashWrite(a,b,c,1)
			#define	SpiRead(a,b,c)						SpiFlashRead(a,b,c,1)
			#define SpiErase(a)							SpiFlashErase(SECTOR_ERASE, a, 1)
		#endif
	#endif

	//N >= 2 ������128ϵͳ֡�Լ�����ЧMIPS�ϸߣ����ȼ�Ϊ3�ı�����̴������ݽ������Ƽ�ֵΪ 6�����ϵͳ֡��mips��ʱ���Ե�СN,��Լram��
	#define MEDIA_RECORDER_FIFO_N				6
	#define MEDIA_RECORDER_FIFO_LEN				(CFG_PARA_MAX_SAMPLES_PER_FRAME * MEDIA_RECORDER_CHANNEL * MEDIA_RECORDER_FIFO_N)
	//�������в�����¼�����ʿ�����Ҫ���������Բ��� ����FILE_WRITE_FIFO_LEN��
	#ifdef CFG_FUNC_RECORD_EXTERN_FLASH_EN
		#define MEDIA_RECORDER_CHANNEL				1
		#define MEDIA_RECORDER_BITRATE				48 //Kbps
		#define MEDIA_RECORDER_ENC_FORMAT_MP2		//¼��Ϊmp2��ʽ
	#else
	#define MEDIA_RECORDER_CHANNEL				2
	#define MEDIA_RECORDER_BITRATE				96 //Kbps
	#endif
	#define MEDIA_RECODER_IO_BLOCK_TIME			1000//ms
	//FIFO_Len=(����(96) / 8 * ����ʱ��ms(1000) �����ʵ�λKbps,��Ч���룩
	//����SDIOЭ�飬д����������250*2ms���� ���ܣ�ʵ�ⲿ��U�̴���785ms������д��������Ҫ���������fifo�ռ� ȷ������������ȵ�����(��ͬ��)��
	#define FILE_WRITE_FIFO_LEN					((((MEDIA_RECORDER_BITRATE / 8) * MEDIA_RECODER_IO_BLOCK_TIME ) / 512) * 512)//(����U��/Card�����������RAM��Դ��ѡ400~1500ms��������512����
#endif //CFG_FUNC_RECORDER_EN
//****************************************************************************************
//                 U�̻�SD��ģʽ��ع�������
//    
//****************************************************************************************
#if(defined(CFG_APP_USB_PLAY_MODE_EN) || defined(CFG_APP_CARD_PLAY_MODE_EN) || BT_AVRCP_SONG_TRACK_INFOR)
/**LRC��ʹ��� **/
//#define CFG_FUNC_LRC_EN			 	// LRC����ļ�����

/*------browser function------*/
//#define FUNC_BROWSER_PARALLEL_EN  		//browser Parallel
//#define FUNC_BROWSER_TREE_EN  			//browser tree
#if	defined(FUNC_BROWSER_PARALLEL_EN)||defined(FUNC_BROWSER_TREE_EN)
#define FUNCTION_FILE_SYSTEM_REENTRY
#if defined(FUNC_BROWSER_TREE_EN)||defined(FUNC_BROWSER_PARALLEL_EN)
#define GUI_ROW_CNT_MAX		5		//�����ʾ������
#else
#define GUI_ROW_CNT_MAX		1		//�����ʾ������
#endif
#endif
/*------browser function------*/

/**�ַ���������ת�� **/
/**Ŀǰ֧��Unicode     ==> Simplified Chinese (DBCS)**/
/**�ַ�ת������fatfs�ṩ������Ҫ�����ļ�ϵͳ**/
/**���֧��ת���������ԣ���Ҫ�޸�fatfs���ñ�**/
#define CFG_FUNC_STRING_CONVERT_EN	// ֧���ַ�����ת��

/**ȡ��AA55�ж�**/
/*fatfs�ڴ���ϵͳMBR��DBR������β�д˱�Ǽ�⣬Ϊ��߷Ǳ������̼����ԣ��ɿ�������, Ϊ��Ч������Ч�̣�����Ĭ�Ϲر�*/
//#define	CANCEL_COMMON_SIGNATURE_JUDGMENT
//#define FUNC_UPDATE_CONTROL   //�����������̿���(ͨ������ȷ������)
#endif

/**USB Host��⹦��**/
#if(defined(CFG_APP_USB_PLAY_MODE_EN))
#define CFG_RES_UDISK_USE
#define CFG_FUNC_UDISK_DETECT
#endif

/**USB Device��⹦��**/
#if (defined (CFG_APP_USB_AUDIO_MODE_EN)) || (defined(CFG_COMMUNICATION_BY_USB))
	#define CFG_FUNC_USB_DEVICE_DETECT
#endif


//****************************************************************************************
//****************************************************************************************
/**OS����ϵͳ����IDLEʱ��core��������״̬���Դﵽ���͹���Ŀ��**/
/*ע�⣬����OS���ȵ�IDLE������Ӧ�ò�APPMODE��Ӧ�ò��������*/
#ifndef CFG_FUNC_MIC_KARAOKE_EN //KARAOKE�����£�Ĭ�Ϲر�
#define CFG_FUNC_IDLE_TASK_LOW_POWER
#ifdef	CFG_FUNC_IDLE_TASK_LOW_POWER
	#define	CFG_GOTO_SLEEP_USE
#endif
#endif
//************************************************************************************************************
//* �͹����Ż�,���ݸ�������ģ����Ż�,�ﵽ���͹��ĵ�Ŀ��
//* ע��: ����ģ��ĵ͹����Ż���ʽ��ģ�������й�ϵ
//************************************************************************************************************
#define CFG_FUNC_SYSTEM_LOW_POWER
#ifdef CFG_FUNC_SYSTEM_LOW_POWER
#define CFG_ADCDAC_SEL_LOWPOWERMODE  //ADC/DACʹ�õ͹���ģʽ
#endif

//****************************************************************************************
//                 UART DEBUG��������
//ע�⣺ DEBUG�򿪺󣬻�����micͨ·��delay������ҪDEBUG���Դ���ʱ������رյ���
//		SHELL������Ҫ���� UART DEBUG����
//****************************************************************************************
#include "debug.h"
#define CFG_FUNC_DEBUG_EN
//#define CFG_FUNC_USBDEBUG_EN
#ifdef CFG_FUNC_DEBUG_EN
	#define CFG_UART_TX_PORT 				DEBUG_TX_A21   // DEBUG_TX_A18 // DEBUG_TX_A10
	#define CFG_UART_BANDRATE   			DEBUG_BAUDRATE_2000000//DEBUG_BAUDRATE_115200
	#define CFG_FLASHBOOT_DEBUG_EN          (1)

//	#define CFG_FUNC_SHELL_EN				//SHELL��������
	#ifdef	CFG_FUNC_SHELL_EN
		//UART RX����,��Ҫ�ʹ�����־��ӡΪͬһ��UART��
		#define CFG_FUNC_SHELL_RX_PORT 		DEBUG_RX_A9
	#endif
#endif

//****************************************************************************************
//                 ��ʾ����������
//˵��:
//    1.��¼���߲μ�MVs26_SDK\tools\script��
//    2.��ʾ�����ܿ�����ע��flash��const data��ʾ��������ҪԤ����¼�����򲻻Ქ��;
//    3.const data���ݿ�����飬Ӱ�쿪���ٶȣ���Ҫ������֤��
//****************************************************************************************
//#define CFG_FUNC_REMIND_SOUND_EN
#ifdef CFG_FUNC_REMIND_SOUND_EN
	#define CFG_PARAM_FIXED_REMIND_VOL   	CFG_PARA_SYS_VOLUME_DEFAULT		//�̶���ʾ������ֵ,0��ʾ��music volͬ������
#endif

//****************************************************************************************
//                 �ϵ���书������        
//****************************************************************************************
#define CFG_FUNC_BREAKPOINT_EN
#ifdef CFG_FUNC_BREAKPOINT_EN
	#define BP_PART_SAVE_TO_NVM			// �ϵ���Ϣ���浽NVM
	#define BP_SAVE_TO_FLASH			// �ϵ���Ϣ���浽Flash
	#define FUNC_MATCH_PLAYER_BP		// ��ȡFSɨ����벥��ģʽ�ϵ���Ϣƥ����ļ����ļ��к�ID��
#endif

//****************************************************************************************
//                            Key �����������
//****************************************************************************************
/**����beep������**/
//#define  CFG_FUNC_BEEP_EN
#define CFG_PARA_BEEP_DEFAULT_VOLUME    15//ע��:����������ͬ�����ܿ����󣬴�ֵ���Ϊ16

/**����˫������**/
#define  CFG_FUNC_DBCLICK_MSG_EN

/**ADC����**/
#define CFG_RES_ADC_KEY_SCAN

/**IR����**/
//#define CFG_RES_IR_KEY_SCAN				//����device service Keyɨ��IRKey

/**������ť����**/
//#define	CFG_RES_CODE_KEY_USE

/**GPIO����**/
//#define CFG_RES_IO_KEY_SCAN

/**��λ������ѡ��**/
//#define CFG_ADC_LEVEL_KEY_EN

//***************************************************************************************
//					RTC/���ӹ�������
//    OSC_32K: RTCʱ��ѡ���徧��32.768kHZ,оƬ���ţ�GPIOB5/GPIOB9���������ͺ�֧��
//    OSC_24M: RTCʱ��ѡ���徧��24MHZ
//***************************************************************************************
//#define CFG_FUNC_RTC_EN
#ifdef CFG_FUNC_RTC_EN
	#define CFG_FUNC_RTC_OSC_FREQ		OSC_32K
	#define CFG_FUNC_ALARM_EN  			//���ӹ���,���뿪ʱ��
	#define CFG_FUNC_LUNAR_EN  			//������,���뿪ʱ��
	#ifdef CFG_FUNC_ALARM_EN
		#define CFG_FUNC_SNOOZE_EN 		//����̰˯����
	#endif
#endif

//****************************************************************************************
//                            Display ��ʾ����
//****************************************************************************************
//#define  CFG_FUNC_DISPLAY_EN
#ifdef CFG_FUNC_DISPLAY_EN
//  #define  DISP_DEV_SLED
  #define  DISP_DEV_7_LED
/**8��LED��ʾ����**/
/*LED�Դ�ˢ����Ҫ��Timer1ms�жϽ��У���дflash����ʱ��ر��ж�*/
/*������Ҫ�����⴦�����ע�ú�����Ĵ����*/
/*ע��timer�жϷ������͵��õ���API�������TCM�������õ�����api���⺯������ѯ֧��*/
/*�����˺꣬Ҫ��ע����ʹ��NVIC_SetPriority ����Ϊ0�Ĵ��룬�����Ӧ�жϵ��÷�TCM��������������λ*/
#ifdef DISP_DEV_7_LED
  #define	CFG_FUNC_LED_REFRESH
#endif

#if defined(DISP_DEV_SLED) && defined(DISP_DEV_7_LED) && defined(LED_IO_TOGGLE)
   #error Conflict: display setting error //����ͬʱѡ��������ʾģʽ
#endif
#endif

//****************************************************************************************
//				   ������μ�⹦��ѡ������
//****************************************************************************************
//#define  CFG_FUNC_DETECT_PHONE_EN                            

//****************************************************************************************
//				   3�ߣ�4�߶������ͼ�⹦��ѡ������
//****************************************************************************************
//#define  CFG_FUNC_DETECT_MIC_SEG_EN  

//flashϵͳ�������ߵ���
#define CFG_FUNC_FLASH_PARAM_ONLINE_TUNING_EN

//CAN����demo
//#define CFG_FUNC_CAN_DEMO_EN

//AI_DENOISE demo,IDE��Ҫʹ��V323�����Ժ�İ汾,��Ҫר��оƬ
//����AI_DENOISE����Դ���޽���ص�BTģʽ����ʾ���ȹ��ܣ���Ҫͬʱ����CFG_DOUBLE_KEY_EN��
//����궨����ýű����ƣ����뵥��һ�У���Ҫ�ڸ��к������ע��
//#define CFG_AI_DENOISE_EN

//˫key���� demo�����Ե���������
//��Ҫע�⣺˫key���� ��������Ҫ���� MVAssistant_V3.3.0�汾�����Ժ�汾
//����CFG_AI_DENOISE_EN����Ҫͬʱ���� CFG_DOUBLE_KEY_EN
//����궨����ýű����ƣ����뵥��һ�У���Ҫ�ڸ��к������ע��
//#define CFG_DOUBLE_KEY_EN

#if defined(CFG_AI_DENOISE_EN) &&(!defined(CFG_DOUBLE_KEY_EN))
	#error ����CFG_AI_DENOISE_EN ��Ҫͬʱ���� CFG_DOUBLE_KEY_EN //����CFG_AI_DENOISE_EN����Ҫͬʱ���� CFG_DOUBLE_KEY_EN
#endif

#include "sys_gpio.h"
#include "clock_config.h"

//************************************************************************************************************
//dump����,���Խ����ݷ��͵�dump����,���ڷ���
//ע��1:ÿ��ģʽ��DmaChannelMap������,��Ҫռ��һ·DMA�������ڶ���CFG_DUMP_UART_TX_DMA_CHANNEL; 
//      �ο�main_task.c��DmaChannelMap
//ע��2:Ĭ��DUMP_UART��A31-UART1,��Ҫ��DEBUG_UART��������
//************************************************************************************************************
//#define CFG_DUMP_DEBUG_EN
#ifdef CFG_DUMP_DEBUG_EN
	//UART����,��Ҫ�ʹ�����־��ӡΪ��ͬ��UART��
	#define CFG_DUMP_UART_TX_PORT 				DEBUG_TX_A31
	#define CFG_DUMP_UART_BANDRATE 				(2000000)
	#define CFG_DUMP_UART_TX_DMA_CHANNEL		(PERIPHERAL_ID_UART0_TX + 2*GET_DEBUG_GPIO_UARTPORT(CFG_DUMP_UART_TX_PORT))
#endif

#endif /* APP_CONFIG_H_ */
