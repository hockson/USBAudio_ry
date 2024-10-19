/**
 **************************************************************************************
 * @file    hdmi_in_mode.c
 * @brief
 *
 * @author  Cecilia Wang
 * @version V1.0.0
 *
 * $Created: 2018-03-23 13:06:47$
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
#include "audio_core_api.h"
#include "audio_core_service.h"
#include "hdmi_in_api.h"
#include "timeout.h"
#include "main_task.h"
#include "audio_effect.h"
#include "mcu_circular_buf.h"
#include "resampler_polyphase.h"
//#include "decoder_service.h"
//#include "remind_sound_service.h"
#include "hdmi_in_Mode.h"
#include "powercontroller.h"
#include "deepsleep.h"
#include "breakpoint.h"
#include "ctrlvars.h"
#include "recorder_service.h"
#include "audio_vol.h"
#include "clk.h"
#include "ctrlvars.h"
#include "reset.h"
#include "watchdog.h"

#include "cec.h"
#include "remind_sound_item.h"
#include "mode_task_api.h"
#include "remind_sound.h"
//#include "audio_adjust.h"
//#include "audio_common.h"

extern CECInitTypeDef 	*gCecInitDef;
extern HDMIInfo         *gHdmiCt;
#ifdef  CFG_APP_HDMIIN_MODE_EN

#define HDMI_IN_PLAY_TASK_STACK_SIZE		512//1024
#define HDMI_IN_PLAY_TASK_PRIO				4
#define HDMI_IN_NUM_MESSAGE_QUEUE			10

#define PCM_REMAIN_SMAPLES					3//spdif���ݶ�����ʱ,�������ݻᳬ��һ֡128�����ڻ������ݣ������и��Ӻ�

//���ڲ���ֵ��32bitת��Ϊ16bit������ʹ��ͬһ��buf������Ҫ��������
#define HDMI_ARC_CARRY_LEN					(MAX_FRAME_SAMPLES * sizeof(PCM_DATA_TYPE) * 2 * 2 + PCM_REMAIN_SMAPLES * 2 * 4)// buf len for get data form dma fifo, deal
 //ת�������buf,���spdifת�������������ı���Ҫ�Ӵ��SPDIF_CARRY_LEN����������8000����ת48000,��Ҫ��С����carry֡��С�����HDMI_SRC_RESAMPLER_OUT_LEN��HDMI_ARC_PCM_FIFO_LEN
#define HDMI_SRC_RESAMPLER_OUT_LEN			(MAX_FRAME_SAMPLES * sizeof(PCM_DATA_TYPE) * 2 * 2 * 2 * 2)


typedef struct _HdmiInPlayContext
{
	uint32_t			*hdmiARCFIFO;	    //ARC��DMAѭ��fifo
	uint32_t            *sourceBuf_ARC;		//ȡARC����
	uint32_t            *hdmiARCCarry;
	uint32_t            *hdmiARCCarry24;
	uint32_t			*hdmiARCPcmFifo;
	MCU_CIRCULAR_CONTEXT hdmiPcmCircularBuf;

	uint32_t			hdmiSampleRate;

	uint8_t    			hdmiArcDone;
	uint8_t     		hdmiRetransCnt;
	TIMER       		hdmiMaxRespondTime;
	SPDIF_TYPE_STR  	AudioInfo;
}HdmiInPlayContext;


/**����appconfigȱʡ����:DMA 8��ͨ������**/
/*1��cec��PERIPHERAL_ID_TIMER3*/
/*2��SD��¼����PERIPHERAL_ID_SDIO RX/TX*/
/*3��SPDIF��PERIPHERAL_ID_SDPIF_RX*/
/*4�����ߴ��ڵ�����PERIPHERAL_ID_UART1 RX/TX,����ʹ��USB HID����ʡDMA��Դ*/
/*5��Mic������PERIPHERAL_ID_AUDIO_ADC1_RX��mode֮��ͨ������һ��*/
/*6��Dac0������PERIPHERAL_ID_AUDIO_DAC0_TX mode֮��ͨ������һ��*/
/*7��DacX�迪��PERIPHERAL_ID_AUDIO_DAC1_TX mode֮��ͨ������һ��*/
/*ע��DMA 8��ͨ�����ó�ͻ:*/
/*a��UART���ߵ�����DAC-X�г�ͻ,Ĭ�����ߵ���ʹ��USB HID*/
/*b��UART���ߵ�����HDMI/SPDIFģʽ��ͻ*/
static const uint8_t DmaChannelMap[6] = {
	PERIPHERAL_ID_SDIO_RX,
	PERIPHERAL_ID_AUDIO_ADC0_RX,
	PERIPHERAL_ID_AUDIO_ADC1_RX,
	PERIPHERAL_ID_AUDIO_DAC0_TX,
	CFG_HDMI_DMA_CHANNEL,
#ifdef CFG_RES_AUDIO_I2SOUT_EN
	PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE,
#else
	PERIPHERAL_ID_TIMER5,
#endif
};


typedef enum __HDMI_FAST_SWITCH_ARC_STATUS
{
	FAST_SWITCH_INIT_STATUS = 0,
	FAST_SWITCH_DONE_STATUS = 1,
	FAST_SWITCH_WORK_STATUS = 2,

} HDMI_FAST_SWITCH_ARC_STATUS;

static HdmiInPlayContext*		hdmiInPlayCt;

//osMutexId			hdmiPcmFifoMutex;
bool 				HdmiSpdifLockFlag = FALSE;
bool 				unmuteLockFlag = 0;
TIMER 				unmuteLockTime;

static void HdmiARCScan(void);
static void HdmiInPlayRunning(uint16_t msgId);

bool HdmiInPlayResMalloc(uint16_t SampleLen)
{
	hdmiInPlayCt->hdmiARCFIFO = (uint32_t*)osPortMalloc(SampleLen * 2 * 2 * 2 * 2);
	if(hdmiInPlayCt->hdmiARCFIFO == NULL)
	{
		return FALSE;
	}
	memset(hdmiInPlayCt->hdmiARCFIFO, 0, SampleLen * 2 * 2 * 2 * 2);

	hdmiInPlayCt->sourceBuf_ARC = (uint32_t *)osPortMalloc(SampleLen * 2 * 2);
	if(hdmiInPlayCt->sourceBuf_ARC == NULL)
	{
		return FALSE;
	}
	memset(hdmiInPlayCt->sourceBuf_ARC, 0, SampleLen * 2 * 2);

	hdmiInPlayCt->hdmiARCPcmFifo = (uint32_t *)osPortMalloc(AudioCoreFrameSizeGet(DefaultNet) * sizeof(PCM_DATA_TYPE) * 2 * 2 * 2);
	if(hdmiInPlayCt->hdmiARCPcmFifo == NULL)
	{
		return FALSE;
	}
	MCUCircular_Config(&hdmiInPlayCt->hdmiPcmCircularBuf, hdmiInPlayCt->hdmiARCPcmFifo, AudioCoreFrameSizeGet(DefaultNet) * sizeof(PCM_DATA_TYPE) * 2 * 2 * 2);
	memset(hdmiInPlayCt->hdmiARCPcmFifo, 0, AudioCoreFrameSizeGet(DefaultNet) * sizeof(PCM_DATA_TYPE) * 2 * 2 * 2);

	return TRUE;
}


/**
 * @func        HdmiInPaly_Init
 * @brief       HdmiInģʽ�������ã���Դ��ʼ��
 * @param       MessageHandle parentMsgHandle
 * @Output      None
 * @return      bool
 * @Others      ����顢Adc��Dac��AudioCore����
 * @Others      ��������Adc��audiocore���к���ָ�룬audioCore��Dacͬ����audiocoreService����������
 * Record
 */
 bool HdmiInPlayInit(void)
{
	AudioCoreIO AudioIOSet;
	bool ret;
	//��SPDIFʱ���л���AUPLL
	// Clock_SpdifClkSelect(APLL_CLK_MODE);

	DMA_ChannelAllocTableSet((uint8_t *)DmaChannelMap);//HdmiIn

	if(!ModeCommonInit())
	{
		return FALSE;
	}

	hdmiInPlayCt = (HdmiInPlayContext*)osPortMalloc(sizeof(HdmiInPlayContext));
	if(hdmiInPlayCt == NULL)
	{
		return FALSE;
	}
	memset(hdmiInPlayCt, 0, sizeof(HdmiInPlayContext));

	if(!HdmiInPlayResMalloc(AudioCoreFrameSizeGet(DefaultNet)))
	{
		APP_DBG("HdmiInPlayResMalloc error\n");
		return FALSE;
	}

	hdmiInPlayCt->hdmiARCCarry = (uint32_t *)osPortMalloc(HDMI_ARC_CARRY_LEN);
	if(hdmiInPlayCt->hdmiARCCarry == NULL)
	{
		return FALSE;
	}
	memset(hdmiInPlayCt->hdmiARCCarry, 0, HDMI_ARC_CARRY_LEN);

#ifdef CFG_AUDIO_WIDTH_24BIT
	hdmiInPlayCt->hdmiARCCarry24 = (uint32_t *)osPortMalloc(HDMI_ARC_CARRY_LEN);
	if(hdmiInPlayCt->hdmiARCCarry24 == NULL)
	{
		return FALSE;
	}
	memset(hdmiInPlayCt->hdmiARCCarry24, 0, HDMI_ARC_CARRY_LEN);
#endif

	memset(&hdmiInPlayCt->AudioInfo, 0, sizeof(SPDIF_TYPE_STR));
	hdmiInPlayCt->AudioInfo.audio_type = SPDIF_AUDIO_PCM_DATA_TYPE;


	HDMI_ARC_Init((uint16_t *)hdmiInPlayCt->hdmiARCFIFO, AudioCoreFrameSizeGet(DefaultNet) * 2 * 2 * 2 * 2, &hdmiInPlayCt->hdmiPcmCircularBuf);
	HDMI_CEC_DDC_Init();

	//Audio init
//	//note Soure0.��sink0�Ѿ���main app�����ã���Ҫ��������


//#ifdef CFG_FUNC_AUDIO_EFFECT_EN
//	#ifdef CFG_FUNC_MIC_KARAOKE_EN
//	hdmiInPlayCt->AudioCoreHdmiIn->AudioEffectProcess = (AudioCoreProcessFunc)AudioEffectProcess;
//	#else
//	hdmiInPlayCt->AudioCoreHdmiIn->AudioEffectProcess = (AudioCoreProcessFunc)AudioMusicProcess;
//	#endif
//#else
//	hdmiInPlayCt->AudioCoreHdmiIn->AudioEffectProcess = (AudioCoreProcessFunc)AudioBypassProcess;
//#endif
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioCoreProcessConfig((void*)AudioMusicProcess);
#else
	AudioCoreProcessConfig((void*)AudioBypassProcess);
#endif
	//HDMI_CEC_InitiateARC();

	//Audio init

	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));

	AudioIOSet.Adapt = SRC_SRA;
	AudioIOSet.Sync = FALSE;
	AudioIOSet.Channels = 2;
	AudioIOSet.Net = DefaultNet;

	AudioIOSet.DataIOFunc = HDMI_ARC_DataGet;
	AudioIOSet.LenGetFunc = HDMI_ARC_DataLenGet;

	AudioIOSet.Depth = AudioCoreFrameSizeGet(DefaultNet) * 2;
	AudioIOSet.LowLevelCent = 40;
	AudioIOSet.HighLevelCent = 60;
	AudioIOSet.SampleRate =  AudioCoreMixSampleRateGet(DefaultNet);
#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = 1;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//��Ҫ���ݽ���λ����չ
#else
	AudioIOSet.IOBitWidth = 0;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//��Ҫ���ݽ���λ����չ
#endif

	if(!AudioCoreSourceInit(&AudioIOSet, HDMI_IN_SOURCE_NUM))
	{
		DBG("hdmi source init error!\n");
		return FALSE;
	}

	AudioCoreSourceAdjust(HDMI_IN_SOURCE_NUM, TRUE);

	DMA_ChannelDisable(CFG_HDMI_DMA_CHANNEL);
	DMA_CircularConfig(CFG_HDMI_DMA_CHANNEL, AudioCoreFrameSizeGet(DefaultNet) * 2 * 2 * 2 * 2, (uint16_t *)hdmiInPlayCt->hdmiARCFIFO, AudioCoreFrameSizeGet(DefaultNet) * 2 * 2 * 2 * 2);
    DMA_ChannelEnable(CFG_HDMI_DMA_CHANNEL);

	SPDIF_ModuleEnable(CFG_HDMI_SPDIF_NUM);

#ifdef CFG_FUNC_BREAKPOINT_EN
		BackupInfoUpdata(BACKUP_SYS_INFO);
#endif

		AudioCodecGainUpdata();//update hardware config

	//AudioCoreSourceEnable(HDMI_IN_SOURCE_NUM);
#ifdef CFG_FUNC_REMIND_SOUND_EN

	DBG("HdmiIn Play run\n");
	ret = RemindSoundServiceItemRequest(SOUND_REMIND_HDMIMODE, REMIND_PRIO_NORMAL);
	if(ret == FALSE)
	{
		if(IsAudioPlayerMute() == TRUE)
		{
			HardWareMuteOrUnMute();
		}
	}
#endif

	HdmiSpdifLockFlag = FALSE;
	unmuteLockFlag = 0;

#ifndef CFG_FUNC_REMIND_SOUND_EN
	 if(IsAudioPlayerMute() == TRUE)
	 {
		 HardWareMuteOrUnMute();
	 }
#endif

	return TRUE;
}

void HdmiInPlayRun(uint16_t msgId)
{
		switch(msgId)
		{
				
			default:
				//if(hdmiInPlayCt->state == TaskStateRunning)
				{
					HdmiInPlayRunning(msgId);
					if(HdmiSpdifLockFlag && !SPDIF_FlagStatusGet(CFG_HDMI_SPDIF_NUM,LOCK_FLAG_STATUS))
					{
						APP_DBG("SPDIF RX UNLOCK!\n");
						HdmiSpdifLockFlag = FALSE;
						//HDMI_CEC_DDC_Init();
						AudioCoreSourceDisable(HDMI_IN_SOURCE_NUM);
//				#ifdef CFG_FUNC_FREQ_ADJUST
//						AudioCoreSourceFreqAdjustDisable();
//				#endif
					}

					if(!HdmiSpdifLockFlag && SPDIF_FlagStatusGet(CFG_HDMI_SPDIF_NUM,LOCK_FLAG_STATUS)/* && HDMI_ARC_IsReady()*/
						// #if	defined(CFG_FUNC_REMIND_SOUND_EN)
						// && hdmiInPlayCt->IsSoundRemindDone
						// #endif
					)
					{
						if((TV_XIAOMI_XMD == gHdmiCt->hdmi_tv_inf.tv_type && HDMI_ARC_IsReady())
						|| (TV_XIAOMI_XMD != gHdmiCt->hdmi_tv_inf.tv_type))
						{
							APP_DBG("SPDIF RX LOCK!\n");
							HdmiSpdifLockFlag = TRUE;

						//AudioCoreSourceMute(APP_SOURCE_NUM, TRUE, TRUE);
						HDMISourceMute();
						vTaskDelay(20);
						AudioCoreSourceEnable(HDMI_IN_SOURCE_NUM);
						//AudioCoreSourceUnmute(APP_SOURCE_NUM, TRUE, TRUE);
						TimeOutSet(&unmuteLockTime, 500);
						unmuteLockFlag = 1;

						}
					}
					if(HdmiSpdifLockFlag == TRUE)
					{
						//Ӳ�������ʻ�ȡ���趨��
						if(hdmiInPlayCt->hdmiSampleRate != SPDIF_SampleRateGet(CFG_HDMI_SPDIF_NUM))
						{
							hdmiInPlayCt->hdmiSampleRate = SPDIF_SampleRateGet(CFG_HDMI_SPDIF_NUM);

							APP_DBG("Get samplerate: %d\n", (int)hdmiInPlayCt->hdmiSampleRate);
							AudioCoreSourceChange(HDMI_IN_SOURCE_NUM, 2, hdmiInPlayCt->hdmiSampleRate);
						}

						if(unmuteLockFlag == 1)
						{
							if(IsTimeOut(&unmuteLockTime))
							{
								unmuteLockFlag = 0;
								if(gHdmiCt->hdmi_audiomute_flag == 0)
								{
									HDMISourceUnmute();
									if(IsAudioPlayerMute() == TRUE)
									{
										HardWareMuteOrUnMute();
									}
									if(gHdmiCt->hdmiActiveReportMuteAfterInit == 1)
									{
										gHdmiCt->hdmiActiveReportMuteAfterInit = 0;
										gHdmiCt->hdmiActiveReportMuteflag = 2;
										gHdmiCt->hdmiActiveReportMuteStatus = 0;
									}
								}

							}
						}

						HdmiARCScan();
					}
					HDMI_CEC_Scan(1);

					//�������ȼ�����Ϊ4,ͨ�����͸�����������AudioCore service��Ч������
					{
						MessageContext		msgSend;
						msgSend.msgId		= MSG_NONE;
						MessageSend(GetAudioCoreServiceMsgHandle(), &msgSend);
					}
				}
				break;
		}
}

static void HdmiInPlayRunning(uint16_t msgId)
{
	if(hdmiInPlayCt->hdmiArcDone == FAST_SWITCH_WORK_STATUS)
	{
		if(IsTimeOut(&hdmiInPlayCt->hdmiMaxRespondTime))
		{
			TimeOutSet(&hdmiInPlayCt->hdmiMaxRespondTime, 200);
			msgId = MSG_HDMI_AUDIO_ARC_ONOFF;
		}
	}
	switch(msgId)
	{
		case MSG_HDMI_AUDIO_ARC_ONOFF:
			if(hdmiInPlayCt->hdmiArcDone != FAST_SWITCH_WORK_STATUS)
			{
				gHdmiCt->hdmiGiveReportStatus = 0;
				hdmiInPlayCt->hdmiRetransCnt = 0;
			}

			hdmiInPlayCt->hdmiArcDone = FAST_SWITCH_WORK_STATUS;
			TimeOutSet(&hdmiInPlayCt->hdmiMaxRespondTime, 200);
			if(gHdmiCt->hdmiGiveReportStatus == 1)
			{
				mainAppCt.hdmiArcOnFlg = !mainAppCt.hdmiArcOnFlg;
				hdmiInPlayCt->hdmiArcDone = FAST_SWITCH_DONE_STATUS;
			}
			else
			{
				if(HDMI_CEC_IsWorking() == CEC_IS_IDLE)
				{
					HDMI_ARC_OnOff(!mainAppCt.hdmiArcOnFlg);
					hdmiInPlayCt->hdmiRetransCnt ++;
				}
				else if(HDMI_CEC_IsWorking() == CEC_IS_INACTIVE)
				{
					hdmiInPlayCt->hdmiRetransCnt = 0;
					mainAppCt.hdmiArcOnFlg = !mainAppCt.hdmiArcOnFlg;
					hdmiInPlayCt->hdmiArcDone = FAST_SWITCH_DONE_STATUS;
				}
			}

			if(hdmiInPlayCt->hdmiRetransCnt >= 2)
			{
				hdmiInPlayCt->hdmiRetransCnt = 0;
				mainAppCt.hdmiArcOnFlg = !mainAppCt.hdmiArcOnFlg;
				hdmiInPlayCt->hdmiArcDone = FAST_SWITCH_DONE_STATUS;
			}
			break;

		default:
			CommonMsgProccess(msgId);
			break;
	}
}

static void HdmiARCScan(void)
{
	int32_t pcm_space;
	uint16_t spdif_len;
	int16_t pcm_len;
	int16_t *pcmBuf  = (int16_t *)hdmiInPlayCt->hdmiARCCarry;
	uint16_t cnt;

	spdif_len = DMA_CircularDataLenGet(CFG_HDMI_DMA_CHANNEL);
	pcm_space = MCUCircular_GetSpaceLen(&hdmiInPlayCt->hdmiPcmCircularBuf) - sizeof(PCM_DATA_TYPE) * 8;

#if 1//def CFG_FUNC_MIXER_SRC_EN
	pcm_space = (pcm_space * hdmiInPlayCt->hdmiSampleRate) /  AudioCoreMixSampleRateGet(DefaultNet) - sizeof(PCM_DATA_TYPE) * 8;
#endif

	if(pcm_space < sizeof(PCM_DATA_TYPE) * 8)
	{
		DBG("hdmi pcm_space err\n");
		return;
	}
	if((spdif_len >> 1) > pcm_space)
	{
		spdif_len = pcm_space * 2;
	}

	spdif_len = spdif_len /(MAX_FRAME_SAMPLES * sizeof(PCM_DATA_TYPE) * 4) * (MAX_FRAME_SAMPLES * sizeof(PCM_DATA_TYPE) * 4);
	spdif_len = spdif_len & 0xFFF8;
	if(!spdif_len)
	{
		return ;
	}

	cnt = (spdif_len /  (sizeof(PCM_DATA_TYPE) * 4)) / MAX_FRAME_SAMPLES;
	while(cnt--)
	{

#ifdef CFG_AUDIO_WIDTH_24BIT
		DMA_CircularDataGet(CFG_HDMI_DMA_CHANNEL, pcmBuf, MAX_FRAME_SAMPLES * 16);
		pcm_len = SPDIF_SPDIFDataToPCMData(CFG_HDMI_SPDIF_NUM,(int32_t *)pcmBuf, MAX_FRAME_SAMPLES * 16, (int32_t *)hdmiInPlayCt->hdmiARCCarry24, SPDIF_WORDLTH_24BIT);

		if(pcm_len > 0)
		{
			int32_t *pcmBuf32  =  (int32_t *)hdmiInPlayCt->hdmiARCCarry24;
			uint16_t i;
			//��8λ�޷���λ����Ҫ��λ����
			for(i=0;i<pcm_len/4;i++)
			{
				pcmBuf32[i] <<= 8;
				pcmBuf32[i] >>= 8;
			}

			if(hdmiInPlayCt->AudioInfo.audio_type != SPDIF_AUDIO_PCM_DATA_TYPE)
			{
				return;
			}

			if(!mainAppCt.hdmiArcOnFlg)
			{
				memset(pcmBuf, 0, pcm_len);
			}
			MCUCircular_PutData(&hdmiInPlayCt->hdmiPcmCircularBuf, hdmiInPlayCt->hdmiARCCarry24, pcm_len);//ע���ʽת������ֵ��byte
		}

#else
		DMA_CircularDataGet(CFG_HDMI_DMA_CHANNEL, pcmBuf, MAX_FRAME_SAMPLES * 8);
		pcm_len = SPDIF_SPDIFDataToPCMData(CFG_HDMI_SPDIF_NUM,(int32_t *)pcmBuf, MAX_FRAME_SAMPLES * 8, (int32_t *)pcmBuf, SPDIF_WORDLTH_16BIT);

		if(hdmiInPlayCt->AudioInfo.audio_type != SPDIF_AUDIO_PCM_DATA_TYPE)
		{
			return;
		}
		if(!mainAppCt.hdmiArcOnFlg)
		{
			memset(pcmBuf, 0, pcm_len);
		}
		MCUCircular_PutData(&hdmiInPlayCt->hdmiPcmCircularBuf, pcmBuf, pcm_len);//ע���ʽת������ֵ��byte
#endif

	 }
}

bool HdmiInPlayDeinit(void)
{
	APP_DBG("HdmiIn Play Deinit\n");
	if(hdmiInPlayCt == NULL)
	{
		return FALSE;
	}

	if(IsAudioPlayerMute() == FALSE)
	{
		HardWareMuteOrUnMute();
	}

	PauseAuidoCore();


	AudioCoreProcessConfig((void*)AudioNoAppProcess);
	AudioCoreSourceDisable(HDMI_IN_SOURCE_NUM);
	AudioCoreSourceDeinit(HDMI_IN_SOURCE_NUM);

	HDMI_CEC_DDC_DeInit();
	HDMI_ARC_DeInit();

	if(hdmiInPlayCt->hdmiARCFIFO != NULL)
	{
		osPortFree(hdmiInPlayCt->hdmiARCFIFO);
		hdmiInPlayCt->hdmiARCFIFO = NULL;
	}

	if(hdmiInPlayCt->sourceBuf_ARC != NULL)
	{
		osPortFree(hdmiInPlayCt->sourceBuf_ARC);
		hdmiInPlayCt->sourceBuf_ARC = NULL;
	}

	if(hdmiInPlayCt->hdmiARCCarry != NULL)
	{
		osPortFree(hdmiInPlayCt->hdmiARCCarry);
		hdmiInPlayCt->hdmiARCCarry = NULL;
	}

#ifdef CFG_AUDIO_WIDTH_24BIT
	if(hdmiInPlayCt->hdmiARCCarry24 != NULL)
	{
		osPortFree(hdmiInPlayCt->hdmiARCCarry24);
		hdmiInPlayCt->hdmiARCCarry24 = NULL;
	}
#endif

	if(hdmiInPlayCt->hdmiARCPcmFifo != NULL)
	{
		osPortFree(hdmiInPlayCt->hdmiARCPcmFifo);
		hdmiInPlayCt->hdmiARCPcmFifo = NULL;
	}

	ModeCommonDeinit();//ͨ·ȫ���ͷ�

	osPortFree(hdmiInPlayCt);
	hdmiInPlayCt = NULL;

	return TRUE;
}

#endif  //CFG_APP_HDMIIN_MODE_EN
