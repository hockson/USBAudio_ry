/**
 **************************************************************************************
 * @file    Radio_mode.c
 * @brief
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2017-12-26 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include "type.h"
#include "irqn.h"
#include "gpio.h"
#include "dma.h"
#include "rtos_api.h"
#include "app_message.h"
#include "app_config.h"
#include "debug.h"
#include "delay.h"
#include "audio_adc.h"
#include "dac.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "audio_effect.h"
#include "radio_api.h"
#ifdef FUNC_RADIO_RDA5807_EN
#include "RDA5807.h"
#endif
#ifdef FUNC_RADIO_QN8035_EN
  #include "QN8035.h"
#endif
#include "audio_core_api.h"
#include "audio_core_service.h"
#include "powercontroller.h"
#include "deepsleep.h"
#include "breakpoint.h"
#include "decoder.h"
#include "remind_sound_item.h"
#include "remind_sound.h"
#include "audio_vol.h"
#include "clk.h"
#include "ctrlvars.h"
#include "main_task.h"
#include "reset.h"
#include "mode_task_api.h"


#ifdef CFG_APP_RADIOIN_MODE_EN

#define RADIO_PLAY_TASK_STACK_SIZE		512//1024
#define RADIO_PLAY_TASK_PRIO			3
#define RADIO_NUM_MESSAGE_QUEUE			10

#define RADIO_SOURCE_NUM				APP_SOURCE_NUM

typedef struct _RadioPlayContext
{
	uint32_t			*ADCFIFO;			//ADC��DMAѭ��fifo
	uint32_t			ADCFIFO_len;		//ADC��DMAѭ��fifo len

}RadioPlayContext;


/**����appconfigȱʡ����:DMA 6��ͨ������**/
static uint8_t sDmaChannelMap[6] = {
	PERIPHERAL_ID_AUDIO_ADC0_RX,
	PERIPHERAL_ID_AUDIO_ADC1_RX,
	PERIPHERAL_ID_AUDIO_DAC0_TX,
#ifdef CFG_RES_AUDIO_I2S_MIX_IN_EN
	PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_MIX_I2S_MODULE,
#else
	PERIPHERAL_ID_SDIO_RX,
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
	PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX_I2S_MODULE,
#else
	255,
#endif
#ifdef CFG_RES_AUDIO_I2SOUT_EN
	PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE,
#else
	255
#endif
};


static RadioPlayContext			*sRadioPlayCt = NULL;
RADIO_CONTROL 					*gRadioControl = NULL;

void RadioMute(bool mute)
{
#ifdef FUNC_RADIO_RDA5807_EN
	RDA5807Mute(mute);
#endif
#ifdef FUNC_RADIO_QN8035_EN
	QN8035Mute(mute);
#endif
}

void RadioSetFreq(uint16_t freq)
{
#ifdef FUNC_RADIO_RDA5807_EN
	RDA5807SetFreq(freq);
#endif
#ifdef FUNC_RADIO_QN8035_EN
	QN8035SetFreq(freq);
#endif
}

bool RadioPowerOn(void)
{
#ifdef FUNC_RADIO_RDA5807_EN
	return RDA5807PowerOn();
#endif
#ifdef FUNC_RADIO_QN8035_EN
	return QN8035PowerOn();
#endif
}

void RadioInit(void)
{
#ifdef FUNC_RADIO_RDA5807_EN
	RDA5807Init();
#endif
#ifdef FUNC_RADIO_QN8035_EN
	QN8035Init();
#endif
}

void RadioPowerDown(void)
{
#ifdef FUNC_RADIO_RDA5807_EN
	RDA5807PowerDown();
#endif
#ifdef FUNC_RADIO_QN8035_EN
	QN8035PowerDown();
#endif
}

uint8_t RadioSearchRead(uint16_t freq)
{
#ifdef FUNC_RADIO_RDA5807_EN
	return RDA5807SearchRead(freq);
#endif
#ifdef FUNC_RADIO_QN8035_EN
	return QN8035SearchRead(freq);
#endif
}


static uint16_t RadioGetBaseFreq(void)
{
	switch(gRadioControl->CurFreqArea)
	{
		case FM_AREA_CHINA:
			return CHINA_BASE_FREQ;
		case FM_AREA_JAPAN:
			return JAPAN_BASE_FREQ;
		case FM_AREA_RUSSIA:
			return RUSSIA_BASE_FREQ;
		default:
			return OTHER_BASE_FREQ;
	}
}

#ifdef CFG_FUNC_BREAKPOINT_EN
void RadioGetBreakPoint(void)
{
	BP_RADIO_INFO *pBpRadioInfo;

	pBpRadioInfo = (BP_RADIO_INFO *)BP_GetInfo(BP_RADIO_INFO_TYPE);

	gRadioControl->CurFreqArea = BP_GET_ELEMENT(pBpRadioInfo->CurBandIdx);
	gRadioControl->Freq = BP_GET_ELEMENT(pBpRadioInfo->CurFreq);
	gRadioControl->ChlCount = BP_GET_ELEMENT(pBpRadioInfo->StationCount);
	if(gRadioControl->ChlCount > 0)
	{
		uint32_t i;
		for(i = 0; i < gRadioControl->ChlCount; i++)
		{
			gRadioControl->Channel[i] = (uint16_t)(BP_GET_ELEMENT(pBpRadioInfo->StationList[i]));
			if((gRadioControl->Channel[i] + RadioGetBaseFreq()) == gRadioControl->Freq)
			{
				gRadioControl->CurStaIdx = (uint8_t)i;
			}
		}
	}
}
#endif

void RadioPlay(void)
{
	if(gRadioControl->State == RADIO_STATUS_SEEKING || gRadioControl->State == RADIO_STATUS_PREVIEW)
	{
		RadioMute(TRUE);
		gRadioControl->State = RADIO_STATUS_IDLE;
	}

	RadioMute(TRUE);
	RadioSetFreq(gRadioControl->Freq);
	APP_DBG("play Freq = %d\n", gRadioControl->Freq);
	RadioMute(FALSE);
	gRadioControl->State = RADIO_STATUS_PLAYING;
}

void RadioSeekEnd(void)
{
	if(gRadioControl->ChlCount > 0)
	{
		gRadioControl->Freq = gRadioControl->Channel[gRadioControl->CurStaIdx] + RadioGetBaseFreq();
	}
	else
	{
		gRadioControl->Freq = gRadioControl->MinFreq;
	}
	APP_DBG("ChlCount = %d\n", gRadioControl->ChlCount);
	APP_DBG("Freq = %d\n", gRadioControl->Freq);
	RadioPlay();
#ifdef CFG_FUNC_BREAKPOINT_EN
	BackupInfoUpdata(BACKUP_RADIO_INFO);
#endif
}

bool RadioChannelAdd(void)
{
	if(gRadioControl->ChlCount == MAX_RADIO_CHANNEL_NUM)
	{
		return FALSE;
	}
	gRadioControl->Channel[gRadioControl->ChlCount++] = gRadioControl->Freq - RadioGetBaseFreq();

#ifdef CFG_FUNC_BREAKPOINT_EN
	//RadioPlayBPUpdata();
	BackupInfoUpdata(BACKUP_RADIO_INFO);
#endif

	return TRUE;
}

void RadioSeekStart(void)
{
	RadioMute(TRUE);

	if(!(RADIO_STATUS_SEEKING == gRadioControl->State || RADIO_STATUS_PREVIEW == gRadioControl->State))
	{
		gRadioControl->Freq = gRadioControl->MinFreq;
		gRadioControl->State = RADIO_STATUS_SEEKING;
		gRadioControl->ChlCount = 0;
		gRadioControl->CurStaIdx = 0;
		memset(gRadioControl->Channel, 0, sizeof(gRadioControl->Channel));
	}
	RadioSetFreq(gRadioControl->Freq);

	//TimeOutSet(&gRadioControl->TimerHandle, 50);
}

void RadioTimerCB(void* unused)
{
	switch(gRadioControl->State)
	{
		case RADIO_STATUS_PLAYING:
			break;

		case RADIO_STATUS_SEEKING_DOWN:
			if(RadioSearchRead(gRadioControl->Freq))
			{
				RadioPlay();
			}
			else
			{
				if(gRadioControl->Freq > gRadioControl->MinFreq)
				 {
				   gRadioControl->Freq--;
				   RadioSetFreq(gRadioControl->Freq);
				 }
				else
				 RadioPlay();
			}
			break;

		case RADIO_STATUS_SEEKING_UP:
			if(RadioSearchRead(gRadioControl->Freq))
			{
				RadioPlay();
			}
			else
			{
				if(gRadioControl->Freq < gRadioControl->MaxFreq)
				 {
				   gRadioControl->Freq++;
				   RadioSetFreq(gRadioControl->Freq);
				 }
				else
				 RadioPlay();
			}
			break;

		case RADIO_STATUS_SEEKING:
			if(RadioSearchRead(gRadioControl->Freq) && (gRadioControl->Freq < gRadioControl->MaxFreq))//
			{
				if(RadioChannelAdd())
				{
					if(gRadioControl->Freq >= gRadioControl->MaxFreq)//
					{
						RadioSeekEnd();
						break;
					}
					else
					{
						gRadioControl->Freq++;
						RadioSeekStart();
						//APP_DBG("gRadioControl->Freq = %d \n",gRadioControl->Freq);
					}
				}
				else
				{
					RadioSeekEnd();
				}
			}
			else
			{
				if(gRadioControl->Freq >= gRadioControl->MaxFreq)//
				{
					RadioSeekEnd();
					break;
				}
				else
				{
					gRadioControl->Freq++;
					RadioSeekStart();
				}
			}
			break;

		case RADIO_STATUS_PREVIEW: //

			break;

		case RADIO_STATUS_IDLE:
		default:
			break;
	}
}

bool RadioSwitchChannel(uint32_t Msg)
{
	if(gRadioControl->State == RADIO_STATUS_SEEKING)
	{
		return FALSE;
	}
	if(gRadioControl->ChlCount == 0)
	{
		return FALSE;
	}

	if((Msg == MSG_RADIO_PLAY_NEXT) ||(Msg == MSG_NEXT))
	{
		gRadioControl->CurStaIdx++;
		gRadioControl->CurStaIdx %= gRadioControl->ChlCount;
	}
	else
	{
		if(gRadioControl->CurStaIdx > 0)
		{
			gRadioControl->CurStaIdx--;
		}
		else
		{
			gRadioControl->CurStaIdx = (uint8_t)(gRadioControl->ChlCount - 1);
		}
	}

	gRadioControl->Freq = gRadioControl->Channel[gRadioControl->CurStaIdx] + RadioGetBaseFreq();
	RadioPlay();

#ifdef CFG_FUNC_BREAKPOINT_EN
	//RadioPlayBPUpdata();
	BackupInfoUpdata(BACKUP_RADIO_INFO);
#endif

	return TRUE;
}

void RadioPlayResFree(void)
{
	if(sRadioPlayCt->ADCFIFO != NULL)
	{
		APP_DBG("ADCFIFO\n");
		osPortFree(sRadioPlayCt->ADCFIFO);
		sRadioPlayCt->ADCFIFO = NULL;
	}

#if 0
#if	defined(CFG_FUNC_REMIND_SOUND_EN)
	AudioCoreSourceDeinit(REMIND_SOURCE_NUM);
#endif
#endif
	if(gRadioControl != NULL)
	{
		osPortFree(gRadioControl);
		gRadioControl = NULL;
	}

	
}

bool RadioPlayResMalloc(uint16_t SampleLen)
{
	sRadioPlayCt = (RadioPlayContext*)osPortMalloc(sizeof(RadioPlayContext));

	if(sRadioPlayCt == NULL)
	{
		return FALSE;
	}
	memset(sRadioPlayCt, 0, sizeof(RadioPlayContext));

	gRadioControl = (RADIO_CONTROL *)osPortMalloc(sizeof(RADIO_CONTROL));
	if(gRadioControl == NULL)
	{
		return FALSE;
	}
	memset(gRadioControl, 0, sizeof(RADIO_CONTROL));

	sRadioPlayCt->ADCFIFO_len = SampleLen * sizeof(PCM_DATA_TYPE) * 2 * 2;
	sRadioPlayCt->ADCFIFO = (uint32_t*)osPortMalloc(sRadioPlayCt->ADCFIFO_len);
	if(sRadioPlayCt->ADCFIFO == NULL)
	{
		return FALSE;
	}
	memset(sRadioPlayCt->ADCFIFO, 0, sRadioPlayCt->ADCFIFO_len);

	return TRUE;
}

void RadioPlayResInit(void)
{
	//Soure1.
	AudioCoreIO	AudioIOSet;
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
	AudioIOSet.Adapt = STD;
	AudioIOSet.Sync = TRUE;
	AudioIOSet.Channels = 2;
	AudioIOSet.Net = DefaultNet;
	AudioIOSet.DataIOFunc = AudioADC0_DataGet;
	AudioIOSet.LenGetFunc = AudioADC0_DataLenGet;

#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = PCM_DATA_24BIT_WIDTH;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//��Ҫ���ݽ���λ����չ
#endif
	if(!AudioCoreSourceInit(&AudioIOSet, RADIO_SOURCE_NUM))
	{
		DBG("Radioplay source error!\n");
	}

#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioCoreProcessConfig((AudioCoreProcessFunc)AudioMusicProcess);
#else
	AudioCoreProcessConfig(AudioCoreProcessFunc)AudioBypassProcess);
#endif

	//LineIn4  analog (ADC)
	AudioAnaChannelSet(RADIO_INPUT_CHANNEL);

	AUDIO_BitWidth BitWidth = ADC_WIDTH_16BITS;

#ifdef	CFG_AUDIO_WIDTH_24BIT
	if(AudioCoreSourceBitWidthGet(RADIO_SOURCE_NUM) == PCM_DATA_24BIT_WIDTH)
		BitWidth = ADC_WIDTH_24BITS;
#endif

	AudioADC_DigitalInit(ADC0_MODULE, AudioCoreMixSampleRateGet(DefaultNet),BitWidth, (void*)sRadioPlayCt->ADCFIFO, sRadioPlayCt->ADCFIFO_len);
#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
	Clock_AudioMclkSel(AUDIO_ADC0, gCtrlVars.HwCt.ADC0DigitalCt.adc_mclk_source);
#else
	gCtrlVars.HwCt.ADC0DigitalCt.adc_mclk_source = Clock_AudioMclkGet(AUDIO_ADC0);
#endif

#if (RADIO_INPUT_CHANNEL == ANA_INPUT_CH_LINEIN1)
#ifdef CFG_ADCDAC_SEL_LOWPOWERMODE
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_LEFT,LINEIN1_LEFT,Single,ADCLowEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_l_gain);
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_RIGHT,LINEIN1_RIGHT,Single,ADCLowEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_r_gain);
#else
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_LEFT,LINEIN1_LEFT,Single,ADCCommonEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_l_gain);
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_RIGHT,LINEIN1_RIGHT,Single,ADCCommonEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_r_gain);
#endif // CFG_ADCDAC_SEL_LOWPOWERMODE
#endif
#if (RADIO_INPUT_CHANNEL == ANA_INPUT_CH_LINEIN2)
	GPIO_PortBModeSet(GPIOB0,0);
	GPIO_PortBModeSet(GPIOB1,0);
	GPIO_RegBitsClear(GPIO_B_IE,GPIOB0);
	GPIO_RegBitsClear(GPIO_B_OE,GPIOB0);
	GPIO_RegBitsClear(GPIO_B_IE,GPIOB1);
	GPIO_RegBitsClear(GPIO_B_OE,GPIOB1);

#ifdef CFG_ADCDAC_SEL_LOWPOWERMODE
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_LEFT,LINEIN2_LEFT,Single,ADCLowEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_l_gain);
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_RIGHT,LINEIN2_RIGHT,Single,ADCLowEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_r_gain);
#else
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_LEFT,LINEIN2_LEFT,Single,ADCCommonEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_l_gain);
	AudioADC_AnaInit(ADC0_MODULE,CHANNEL_RIGHT,LINEIN2_RIGHT,Single,ADCCommonEnergy,31 - gCtrlVars.HwCt.ADC0PGACt.pga_aux_r_gain);
#endif // CFG_ADCDAC_SEL_LOWPOWERMODE
#endif
}

//Radioģʽ�������ã���Դ��ʼ��
bool RadioPlayInit(void)
{
	APP_DBG("Radio Mode init\n");
	DMA_ChannelAllocTableSet(sDmaChannelMap);//FMIn

	if(!ModeCommonInit())
	{
		ModeCommonDeinit();
		return FALSE;
	}
	if(!RadioPlayResMalloc(AudioCoreFrameSizeGet(DefaultNet)))
	{
		APP_DBG("RadioPlayResMalloc Res Error!\n");
		return FALSE;
	}

	RadioPlayResInit();

	AudioCodecGainUpdata();//update hardware config

#ifdef	CFG_FUNC_BREAKPOINT_EN
	RadioGetBreakPoint();
#endif

	gRadioControl->State = RADIO_STATUS_IDLE;
	gRadioControl->CurFreqArea = FM_AREA_CHINA;

	// Audio core config
	TimeOutSet(&gRadioControl->TimerHandle, 100);
	switch(gRadioControl->CurFreqArea)
	{
		case FM_AREA_CHINA:
			gRadioControl->MinFreq = RADIO_DEF_MIN_FREQ_VALUE;
			gRadioControl->MaxFreq = RADIO_DEF_MAX_FREQ_VALUE;
			break;
		case FM_AREA_JAPAN:
			gRadioControl->MinFreq = RADIO_JAP_MIN_FREQ_VALUE;
			gRadioControl->MaxFreq = RADIO_JAP_MAX_FREQ_VALUE;
			break;
		case FM_AREA_RUSSIA:
			gRadioControl->MinFreq = RADIO_RUS_MIN_FREQ_VALUE;
			gRadioControl->MaxFreq = RADIO_RUS_MAX_FREQ_VALUE;
			break;
		default:
			gRadioControl->MinFreq = RADIO_OTH_MIN_FREQ_VALUE;
			gRadioControl->MaxFreq = RADIO_OTH_MAX_FREQ_VALUE;
			break;
	}

	if(gRadioControl->Freq < gRadioControl->MinFreq)
	{
		gRadioControl->Freq = gRadioControl->MinFreq;
	}
	else if(gRadioControl->Freq > gRadioControl->MaxFreq)
	{
		gRadioControl->Freq = gRadioControl->MaxFreq;
	}
	APP_DBG("gRadioControl->Freq = %d \n",gRadioControl->Freq);

#ifdef CFG_RADIO_CLK_M12
	Clock_GPIOOutSel(GPIO_CLK_OUT_A29, RC12M_CLK);
#endif

	if(RadioPowerOn())
	{
		APP_DBG("Radio PowerOn\n");
		RadioInit();
		RadioMute(TRUE);
		//gRadioControl->Freq = 971;
		RadioSetFreq(gRadioControl->Freq);
		RadioMute(FALSE);
		AudioCoreSourceEnable(RADIO_SOURCE_NUM);
	}
	else
	{
		APP_DBG("err: radio Ӳ���쳣!!!\n");
		//return FALSE;
	}

	//FM��Ƶ�����audio core
	
#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(RemindSoundServiceItemRequest(SOUND_REMIND_FMMODE, REMIND_PRIO_NORMAL) == FALSE)
	{
		if(IsAudioPlayerMute() == TRUE)
		{
			HardWareMuteOrUnMute();
		}
	}
#endif

#ifndef CFG_FUNC_REMIND_SOUND_EN
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}
#endif

	return TRUE;
}


void RadioPlayRun(uint16_t msgId)
{
	if(IsTimeOut(&gRadioControl->TimerHandle))
	{
		TimeOutSet(&gRadioControl->TimerHandle, 100);
		RadioTimerCB(NULL);
	}

	switch(msgId)
	{
		case MSG_PLAY_PAUSE:
		case MSG_RADIO_PLAY_SCAN:
			if(gRadioControl->State == RADIO_STATUS_SEEKING)
			{
				RadioSeekEnd();
			}
			else
			{
				RadioSeekStart();
			}
			break;
		case MSG_FB_START:
		case MSG_RADIO_PLAY_SCAN_DN:
			if(gRadioControl->State == RADIO_STATUS_SEEKING)
			{
				RadioSeekEnd();
			}
			else
			{
			   if(gRadioControl->Freq > gRadioControl->MinFreq)
				{
				  RadioMute(TRUE);
				  gRadioControl->Freq--;
				  RadioSetFreq(gRadioControl->Freq);
				  gRadioControl->State = RADIO_STATUS_SEEKING_DOWN;
				}
			}
			break;
		case MSG_FF_START:
		case MSG_RADIO_PLAY_SCAN_UP:
			if(gRadioControl->State == RADIO_STATUS_SEEKING)
			{
				RadioSeekEnd();
			}
			else
			{
			   if(gRadioControl->Freq < gRadioControl->MaxFreq)
				{
				   RadioMute(TRUE);
				  gRadioControl->Freq++;
				  RadioSetFreq(gRadioControl->Freq);
				  gRadioControl->State = RADIO_STATUS_SEEKING_UP;
				}
			}
			break;

#ifdef CFG_RES_IR_NUMBERKEY
		case MSG_NUM_0:
		case MSG_NUM_1:
		case MSG_NUM_2:
		case MSG_NUM_3:
		case MSG_NUM_4:
		case MSG_NUM_5:
		case MSG_NUM_6:
		case MSG_NUM_7:
		case MSG_NUM_8:
		case MSG_NUM_9:
			if(!Number_select_flag)
				Number_value = msgId - MSG_NUM_0;
			else
				Number_value = Number_value * 10 + (msgId - MSG_NUM_0);
			if(Number_value > 9999)
				Number_value = 0;
			Number_select_flag = 1;
			TimeOutSet(&Number_selectTimer, 2000);
			break;
#endif
		case MSG_PRE:
		case MSG_RADIO_PLAY_PRE:
			RadioSwitchChannel(msgId);
			break;
		case MSG_NEXT:
		case MSG_RADIO_PLAY_NEXT:
			RadioSwitchChannel(msgId);
			break;

		case MSG_DECODER_STOPPED:
#if 0//def CFG_FUNC_REMIND_SOUND_EN
			{
				MessageContext		msgSend;

				msgSend.msgId = msgId;
				MessageSend(GetRemindSoundServiceMessageHandle(), &msgSend);//��ʾ���ڼ�ת����������Ϣ��
			}
#endif
			break;

		default:
			CommonMsgProccess(msgId);
			break;
	}
}

bool RadioPlayDeinit(void)
{
	APP_DBG("Radio Mode Deinit\n");
	if(sRadioPlayCt == NULL)
	{
		return TRUE;
	}
	if(IsAudioPlayerMute() == FALSE)
	{
		HardWareMuteOrUnMute();
	}
	
	PauseAuidoCore();
	
	//ע�⣺AudioCore�����������mainApp�£��˴�ֻ�ر�AudioCoreͨ�������ر�����
	AudioCoreProcessConfig((void*)AudioNoAppProcess) ;
	AudioCoreSourceDisable(RADIO_SOURCE_NUM);
	AudioCoreSourceDeinit(RADIO_SOURCE_NUM);

	AudioAnaChannelSet(ANA_INPUT_CH_NONE);

#if (RADIO_INPUT_CHANNEL == ANA_INPUT_CH_LINEIN3)
	AudioADC_Disable(ADC1_MODULE);
	AudioADC_DeInit(ADC1_MODULE);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC1_RX, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC1_RX, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC1_RX, DMA_ERROR_INT);
	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_ADC1_RX);

#else
	AudioADC_Disable(ADC0_MODULE);
	AudioADC_DeInit(ADC0_MODULE);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC0_RX, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC0_RX, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC0_RX, DMA_ERROR_INT);
	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_ADC0_RX);
#endif

	RadioPowerDown();
#ifdef CFG_RADIO_CLK_M12
#if zsq
	Clock_GPIOOutDisable(GPIO_CLK_OUT_A29);
#endif
#endif

	RadioPlayResFree();
	ModeCommonDeinit();//ͨ·ȫ���ͷ�
	osPortFree(sRadioPlayCt);
	sRadioPlayCt = NULL;

	return TRUE;
}

#endif //CFG_APP_RADIOIN_MODE_EN
