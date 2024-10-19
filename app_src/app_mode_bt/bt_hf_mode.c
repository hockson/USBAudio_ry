/**
 **************************************************************************************
 * @file    bt_hf_mode.c
 * @brief   ����ͨ��ģʽ
 *
 * @author  KK
 * @version V1.0.1
 *
 * $Created: 2019-3-28 18:00:00$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#include "app_config.h"
#include "app_message.h"
//driver
#include "dma.h"
//middleware
#include "main_task.h"
#include "audio_vol.h"
#include "bt_manager.h"
#include "audio_core_api.h"
#include "ctrlvars.h"
#include "bt_interface.h"
//application
#include "bt_hf_mode.h"
#include "bt_hf_api.h"
#include "audio_effect.h"
#include "bt_app_hfp_deal.h"
#include "bt_stack_service.h"
#include "remind_sound.h"

/*******************************************************************************************************************************************************************
*           		SaveScoData	 			   		   InAdapter	  	    CoreProcess			   OutAdapter			SaveScoData
*			 	********************  *******  ******  *********  *******  *************  *******  **********  ******  **********************************
* Mic16K   									*->*FIFO*->*       *->*Frame*->*AEC *MixNet*->*Frame*->* Src    *->*FIFO*->*				     * ScoOutPCM:8K Send
* RefSco Delay Fifo Get	 					*                  *->*Frame*->*						 				  \ Encode * mSbcOutFIFO * ScoOutmSBC Send
* ScoInPCM    								*->*FIFO*->*Src    *->*Frame*->*           *->*Frame*->*        *->*FIFO*->*Dac
* ScoInMsbc *mSbcRecvfifo*mSbcInFIFO *Decode*->*FIFO* /
*							  													   	   *->*Frame*->*		*->*FIFO*->*RefSco Delay Fifo Set
*				********************  ******   *****  **********  *******  *************  *******  **********  ******  **********************************
*
*****************************************************************************************************************************************************************/

#if defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT)
static const uint8_t DmaChannelMap[6] = {
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

////////////////////////////////////////////////////////////////////////////////////
//msbc encoder
#define MSBC_CHANNE_MODE	    1 		// mono
#define MSBC_SAMPLE_REATE	    16000	// 16kHz
#define MSBC_BLOCK_LENGTH	    15

#define DELAY_EXIT_BT_HF_TIME   200 //��ʱ�˳�HFģʽʱ������  ��λ/ms
#define CFG_BT_RING_TIME		2000//���ñ����������ż��ʱ��

/////////////////////////////////////////////////////////////////////////////////////
BtHfContext	*gBtHfCt = NULL;

//���롢�˳�ͨ��ģʽ����������ȫ�ֱ���
static uint32_t sBtHfModeEnterFlag = 0;
uint32_t BtHfModeExitFlag = 0;
uint32_t BtHfModeEnterFlag = 0;
static uint32_t sBtHfModeEixtList = 0; //������Ϣ���У���ģʽ������ɺ��ٵ����˳�ģʽ

extern uint32_t gSpecificDevice;
extern uint32_t gHfFuncState;

static uint32_t	flagVoiceDelayExitBtHf = 0;	//��������־�ر��˳�ʱ����ʱ1s�˳�bt hfģʽ
uint32_t	HfStateCount = 0;

//////////////////////////////////////////////////////////////////////////////////////
extern void ModeCommonDeinit(void);

void SetsBtHfModeEnterFlag(uint32_t flag)
{
	sBtHfModeEnterFlag = flag;
}

uint8_t GetBtHfModeEnterFlag(void)
{
	return BtHfModeEnterFlag;
}

void BtModeEnterDetect(void)
{
	if(GetBtHfModeEnterFlag())
	{
		APP_DBG("BtHfModeEnterFlag -->> BtHfModeEnter\n");
		BtHfModeEnter();
	}
}

/*****************************************************************************************
 * ����������뱨�� ��ʾ����������
 ****************************************************************************************/
#ifdef CFG_FUNC_REMIND_SOUND_EN
static void BtHfRingRemindNumberInit(void)
{
	if(gBtHfCt == NULL)
		return;
	if(sys_parameter.bt_CallinRingType)
	{
		TimeOutSet(&gBtHfCt->CallRingTmr,CFG_BT_RING_TIME);
		gBtHfCt->RemindSoundStart = 0;
	}

	if(sys_parameter.bt_CallinRingType == USE_NUMBER_REMIND_RING)
	{
		memset(gBtHfCt->PhoneNumber, 0, sizeof(gBtHfCt->PhoneNumber));
		gBtHfCt->PhoneNumberLen = 0;
	}
}

static void BtHfRingRemindNumberStop(void)
{
	if(gBtHfCt == NULL)
		return;
	if(sys_parameter.bt_CallinRingType )
	{
		if(gBtHfCt->RemindSoundStart)
		{
			gBtHfCt->RemindSoundStart = 0;
			RemindSoundClearPlay();

			if(sys_parameter.bt_CallinRingType == USE_NUMBER_REMIND_RING)
			{
				memset(gBtHfCt->PhoneNumber, 0, sizeof(gBtHfCt->PhoneNumber));
				gBtHfCt->PhoneNumberLen = 0;
			}	
		}
	}
}

static void BtHfRingRemindNumberRunning(void)
{
	static uint8_t i = 0;
	static uint8_t len = 0;

	if((!sys_parameter.bt_CallinRingType) || (GetHfpState(BtCurIndex_Get()) != BT_HFP_STATE_INCOMING))
	{
		i = 0;
		len = 0;
		BtHfRingRemindNumberStop();
		return;
	}
	if(gBtHfCt->RemindSoundStart && IsTimeOut(&gBtHfCt->CallRingTmr))
	{
		if(sys_parameter.bt_CallinRingType >= USE_LOCAL_AND_PHONE_RING)
		{
			if(!RemindSoundIsPlay())
			{
				if(gBtHfCt->WaitFlag)
				{
					gBtHfCt->WaitFlag = 0;
					TimeOutSet(&gBtHfCt->CallRingTmr,CFG_BT_RING_TIME);
					return;
				}		
				TimeOutSet(&gBtHfCt->CallRingTmr,0);
				RemindSoundServiceItemRequest(SOUND_REMIND_CALLRING, REMIND_PRIO_NORMAL);
				gBtHfCt->WaitFlag = 1;
			}
		}
		else
		{
			char *SoundItem = NULL;

			if(gBtHfCt->WaitFlag)
			{
				gBtHfCt->WaitFlag = 0;
				TimeOutSet(&gBtHfCt->CallRingTmr, 800);
				return;
			}
			gBtHfCt->WaitFlag = 1;
			len = (gBtHfCt->PhoneNumberLen >= sizeof(gBtHfCt->PhoneNumber)) ? sizeof(gBtHfCt->PhoneNumber) : gBtHfCt->PhoneNumberLen;
			switch(gBtHfCt->PhoneNumber[i])
			{
				case '0':
					SoundItem = SOUND_REMIND_0;
					break;
				case '1':
					SoundItem = SOUND_REMIND_1;
					break;
				case '2':
					SoundItem = SOUND_REMIND_2;
					break;
				case '3':
					SoundItem = SOUND_REMIND_3;
					break;
				case '4':
					SoundItem = SOUND_REMIND_4;
					break;
				case '5':
					SoundItem = SOUND_REMIND_5;
					break;
				case '6':
					SoundItem = SOUND_REMIND_6;
					break;
				case '7':
					SoundItem = SOUND_REMIND_7;
					break;
				case '8':
					SoundItem = SOUND_REMIND_8;
					break;
				case '9':
					SoundItem = SOUND_REMIND_9;
					break;
				default:
					SoundItem = NULL;
					break;
			}

			if (i < len)
			{
				i++;
				if(SoundItem && RemindSoundServiceItemRequest(SoundItem, REMIND_PRIO_ORDER) == FALSE)
				{
					DBG("Phone Number play error!\n");
				}
			}
			else if(i == len)
			{
				i++;
				RemindSoundServiceItemRequest(SOUND_REMIND_CALLRING, REMIND_PRIO_NORMAL);
			}
			else
			{
				i = 0;
			}
		}
	}	
}
#endif

#ifdef BT_HFP_CALL_DURATION_DISP
//������ʾͨ��ʱ��
void BtHfActiveTimeCount(void)
{

	if(GetHfpState(BtCurIndex_Get()) >= BT_HFP_STATE_ACTIVE
#if BT_HFG_SUPPORT
		|| gSwitchSourceAndSink == A2DP_SET_SOURCE
#endif
		)
	{
		gBtHfCt->BtHfTimerMsCount++;
		if(gBtHfCt->BtHfTimerMsCount>=1000) //1s
		{
			gBtHfCt->BtHfTimerMsCount = 0;
			gBtHfCt->BtHfTimeUpdate = 1;
			gBtHfCt->BtHfActiveTime++;
		}
	}
}

void BtHfActiveTimeInit(void)
{
	gBtHfCt->BtHfActiveTime = 0;
	gBtHfCt->BtHfTimerMsCount = 0;
	gBtHfCt->BtHfTimeUpdate = 0;
}

void BtHfActiveTimeUpdate(void)
{
	if(gBtHfCt->BtHfTimeUpdate)
	{
		uint8_t hour = (uint8_t)(gBtHfCt->BtHfActiveTime / 3600);
		uint8_t minute = (uint8_t)((gBtHfCt->BtHfActiveTime % 3600) / 60);
		uint8_t second = (uint8_t)(gBtHfCt->BtHfActiveTime % 60);
		gBtHfCt->BtHfTimeUpdate = 0;
		APP_DBG("ͨ���У�%02d:%02d:%02d   GetHfpState(%d) = %d\n", hour, minute, second, BtCurIndex_Get(), GetHfpState(BtCurIndex_Get()));
	}
	else if(GetHfpState(BtCurIndex_Get()) < BT_HFP_STATE_ACTIVE)
	{
		if (HfStateCount++ > 1000)
		{
			HfStateCount = 0;
			APP_DBG("GetHfpState(%d) = %d\n", BtCurIndex_Get(), GetHfpState(BtCurIndex_Get()));
		}
	}
}
#endif

/*******************************************************************************
 * TIMER2��ʱ������ͨ��ʱ���ļ�ʱ
 * 1MS����1��
 ******************************************************************************/
void BtHf_Timer1msProcess(void)
{
#ifdef BT_HFP_CALL_DURATION_DISP
	if(gBtHfCt == NULL)
		return;
	BtHfActiveTimeCount();
#endif
}

/***********************************************************************************
 **********************************************************************************/
bool BtHf_CvsdInit(void)
{
	if(gBtHfCt->CvsdInitFlag)
		return FALSE;
	
	//cvsd plc config
	memset(&gBtHfCt->cvsdPlcState, 0, sizeof(CVSD_PLC_State));
	cvsd_plc_init(&gBtHfCt->cvsdPlcState);

	gBtHfCt->CvsdInitFlag = 1;
	return TRUE;
}

/***********************************************************************************
 **********************************************************************************/
bool BtHf_MsbcInit(void)
{
	if(gBtHfCt->MsbcInitFlag)
		return FALSE;
	
	//msbc decoder init
	if(BtHf_MsbcDecoderInit() != 0)
	{
		APP_DBG("msbc decoder error\n");
		return FALSE;
	}

	//encoder init
	BtHf_MsbcEncoderInit();

	gBtHfCt->MsbcInitFlag = 1;
	return TRUE;
}

/***********************************************************************************
 **********************************************************************************/
void BtHfCodecTypeUpdate(uint8_t codecType)
{
	if(gBtHfCt == NULL)
		return;

	if(gBtHfCt->codecType != codecType)
	{
		MessageContext		msgSend;
		APP_DBG("[HF]codec type %d -> %d\n", gBtHfCt->codecType, codecType);
		
		gBtHfCt->codecType = codecType;
		
		//reply
		msgSend.msgId		= MSG_BT_HF_MODE_CODEC_UPDATE;
		MessageSend(GetSysModeMsgHandle(), &msgSend);
	}
}

/***********************************************************************************
 **********************************************************************************/
static void BtHfModeRunningConfig(void)
{
	APP_DBG("calling start...\n");

	DecoderSourceNumSet(BT_HF_SOURCE_NUM,DECODER_MODE_CHANNEL);

	//call resume 
	if(GetHfpScoAudioCodecType(BtCurIndex_Get()))
	{
		if(gBtHfCt->codecType != HFP_AUDIO_DATA_mSBC)
		{
			gBtHfCt->codecType = HFP_AUDIO_DATA_mSBC;
			AudioCoreSinkChange(AUDIO_HF_SCO_SINK_NUM, 1, BTHF_MSBC_SAMPLE_RATE);
		}
	}
	else
	{
		if(gBtHfCt->codecType != HFP_AUDIO_DATA_PCM)
		{
			gBtHfCt->codecType = HFP_AUDIO_DATA_PCM;
			AudioCoreSinkChange(AUDIO_HF_SCO_SINK_NUM, 1, BTHF_CVSD_SAMPLE_RATE);
		}
	}

	//��ʼ��������
	BtHf_MsbcInit();
	
	//DecoderServiceCreate(GetSysModeMsgHandle(), DECODER_BUF_SIZE_MP3, DECODER_FIFO_SIZE_FOR_MP3);
	
	DecoderServiceInit(GetSysModeMsgHandle(),DECODER_MODE_CHANNEL,DECODER_BUF_SIZE_SBC, DECODER_FIFO_SIZE_FOR_SBC);
	
	BtHf_CvsdInit();
	
	//source1 config
	AudioCoreIO	AudioIOSet;
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
#if defined(CFG_PARA_HFP_SYNC) && !defined(CFG_PARA_HFP_FREQ_ADJUST)
	AudioIOSet.Adapt = SRC_SRA;
#else
	AudioIOSet.Adapt = SRC_ONLY;
#endif
	AudioIOSet.Sync = FALSE;
	AudioIOSet.Channels = 1;
	AudioIOSet.Net = DefaultNet;
	AudioIOSet.DataIOFunc = BtHf_ScoDataGet;
	AudioIOSet.LenGetFunc = BtHf_ScoDataLenGet;
#if	defined(CFG_PARA_HFP_SYNC) && !defined(CFG_PARA_HFP_FREQ_ADJUST)
	AudioIOSet.HighLevelCent = 60;
	AudioIOSet.LowLevelCent = 40;
#endif
	if(gBtHfCt->codecType == HFP_AUDIO_DATA_PCM)
	{
		AudioIOSet.SampleRate = BTHF_CVSD_SAMPLE_RATE;
#if defined(CFG_PARA_HFP_SYNC) && !defined(CFG_PARA_HFP_FREQ_ADJUST)
		AudioIOSet.Depth = BT_SCO_PCM_FIFO_LEN / 2;
#endif
	}
	else
	{
		AudioIOSet.SampleRate = BTHF_MSBC_SAMPLE_RATE;
#if defined(CFG_PARA_HFP_SYNC) && !defined(CFG_PARA_HFP_FREQ_ADJUST)
		AudioIOSet.Depth = DECODER_FIFO_SIZE_FOR_SBC / 2; //
#endif
	}
	if(!AudioCoreSourceInit(&AudioIOSet, BT_HF_SOURCE_NUM))
	{
		DBG("Handfree source error!\n");
	}
#if defined(CFG_PARA_HFP_SYNC) && !defined(CFG_PARA_HFP_FREQ_ADJUST)
	AudioCoreSourceAdjust(APP_SOURCE_NUM, TRUE); //ͨ·������ ΢��������
#endif
	
	BtAppiFunc_SaveScoData(BtHf_SaveScoData);
//	BtAppiFunc_BtScoSendProcess(BtHf_SendData);//unused
	AudioCoreSourceEnable(MIC_SOURCE_NUM);
	//AudioCoreSourceUnmute(MIC_SOURCE_NUM, TRUE, TRUE);
	AudioCoreSinkEnable(AUDIO_DAC0_SINK_NUM);
	AudioCoreSinkEnable(AUDIO_HF_SCO_SINK_NUM);

	gBtHfCt->btHfScoSendReady = 1;
	gBtHfCt->btHfScoSendStart = 0;

	if(GetBtHfpVoiceRecognition())
	{
		SetHfpState(BtCurIndex_Get(),BT_HFP_STATE_ACTIVE);
	}
	DecoderSourceNumSet(BT_HF_SOURCE_NUM,DECODER_MODE_CHANNEL);
	AudioCoreSourceEnable(BT_HF_SOURCE_NUM);
	//AudioCoreSourceUnmute(BT_HF_SOURCE_NUM, TRUE, TRUE);
}

void BtHfModeRunningResume(void)
{
	if(gBtHfCt == NULL)
		return;

	//if(gBtHfCt->state == TaskStatePaused)
	{
		BtHfTaskResume();
	}
}

/************************************************************************************
 * @func        BtHfInit
 * @brief       BtHfģʽ�������ã���Դ��ʼ��
 * @param       MessageHandle   
 * @Output      None
 * @return      int32_t
 * @Others      ����顢Dac��AudioCore���ã�����Դ��DecoderService
 * @Others      ��������Decoder��audiocore���к���ָ�룬audioCore��Dacͬ����audiocoreService����������
 * @Others      ��ʼ��ͨ��ģʽ�������ģ��,��ģʽ������,����SCO��·�������������ݸ�ʽ��Ӧ�Ĵ�������������
 * Record
 ***********************************************************************************/
bool BtHfInit(void)
{
	AudioCoreIO	AudioIOSet;
	APP_DBG("BT HF init\n");
	
	//�����������ȼ��ָ�����һ��,��AudioCoreServiceͬ��
	vTaskPrioritySet(GetBtStackServiceTaskHandle(), (GetBtStackServiceTaskPrio()+1));

	//DMA channel
	DMA_ChannelAllocTableSet((uint8_t *)DmaChannelMap);

	AudioCoreFrameSizeSet(DefaultNet, CFG_BTHF_PARA_SAMPLES_PER_FRAME);
	
	//Task & App Config
	gBtHfCt = (BtHfContext*)osPortMalloc(sizeof(BtHfContext));
	if(gBtHfCt == NULL)
	{
		return FALSE;
	}
	memset(gBtHfCt, 0, sizeof(BtHfContext));

	gBtHfCt->SystemEffectMode = mainAppCt.EffectMode;	//����EffectMode���˳�HFPģʽ�Ժ���Ҫ�ָ�
	mainAppCt.EffectMode = EFFECT_MODE_HFP_AEC;

	if(!AudioIoCommonForHfp(BT_HFP_MIC_PGA_GAIN))
	{
		DBG("Mic and Dac set error\n");
	}

	//sco input/output ��������
	memset(&gBtHfCt->ScoInDataBuf[0], 0, SCO_IN_DATA_SIZE);
	
	memset(&gBtHfCt->PcmBufForMsbc[0], 0, MSBC_PACKET_PCM_SIZE);
	
	//call resume 
	if(GetHfpScoAudioCodecType(BtCurIndex_Get()))
		gBtHfCt->codecType = HFP_AUDIO_DATA_mSBC;
	else
		gBtHfCt->codecType = HFP_AUDIO_DATA_PCM;
	
	{
		MCUCircular_Config(&gBtHfCt->ScoInPcmFIFOCircular, &gBtHfCt->ScoInPcmFIFO[0], BT_SCO_PCM_FIFO_LEN);
		memset(&gBtHfCt->ScoInPcmFIFO[0], 0, BT_SCO_PCM_FIFO_LEN);
	}

#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioCoreProcessConfig((void*)AudioEffectProcessBTHF);
#else
	AudioCoreProcessConfig((void*)AudioBypassProcess);
#endif

	//sink2 for sco out
	memset(&gBtHfCt->ScoOutPcmFifo[0], 0, BT_SCO_PCM_FIFO_LEN);
	MCUCircular_Config(&gBtHfCt->ScoOutPcmFIFOCircular, &gBtHfCt->ScoOutPcmFifo[0], BT_SCO_PCM_FIFO_LEN);
	gBtHfCt->hfpSendDataCnt = 0;

//AudioCore sink2 config -- ����HFP SCO��������
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));

#ifdef	CFG_PARA_HFP_SYNC
#ifdef CFG_PARA_HFP_FREQ_ADJUST
	AudioIOSet.Adapt = SRC_ADJUST;
#else//���΢��
	AudioIOSet.Adapt = SRC_SRA;
#endif
#else //��΢��
	AudioIOSet.Adapt = SRC_ONLY;
#endif
	AudioIOSet.Sync = FALSE;//TRUE;
	AudioIOSet.Channels = 1;
	AudioIOSet.Net = DefaultNet;
	AudioIOSet.Depth = BT_SCO_PCM_FIFO_LEN / 2;
	AudioIOSet.DataIOFunc = BtHf_SinkScoDataSet;
	AudioIOSet.LenGetFunc = BtHf_SinkScoDataSpaceLenGet;

#ifdef	CFG_PARA_HFP_SYNC
	AudioIOSet.HighLevelCent = 60;
	AudioIOSet.LowLevelCent = 40;
#endif

	if(gBtHfCt->codecType == HFP_AUDIO_DATA_PCM)
	{
		AudioIOSet.SampleRate = BTHF_CVSD_SAMPLE_RATE;
	}
	else
	{
		AudioIOSet.SampleRate = BTHF_MSBC_SAMPLE_RATE;
	}

#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = 0;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//SCO 16bit ,����Ҫ���⴦��
#endif
	if(!AudioCoreSinkInit(&AudioIOSet, AUDIO_HF_SCO_SINK_NUM))
	{
		DBG("ScoSink init error");
		return FALSE;
	}
#ifdef	CFG_PARA_HFP_SYNC
	AudioCoreSinkAdjust(AUDIO_HF_SCO_SINK_NUM, TRUE);//ͨ·δʹ��ʱ����������
#endif

	if((gBtHfCt->ScoOutPcmFifoMutex = xSemaphoreCreateMutex()) == NULL)
	{
		return FALSE;
	}

	//��Ҫ�������ݻ���
	MCUCircular_Config(&gBtHfCt->MsbcSendCircular, &gBtHfCt->MsbcSendFifo[0], BT_SCO_PCM_FIFO_LEN);
	memset(&gBtHfCt->MsbcSendFifo[0], 0, BT_SCO_PCM_FIFO_LEN);

	//SBC receivr fifo
	MCUCircular_Config(&gBtHfCt->msbcRecvFifoCircular, &gBtHfCt->msbcRecvFifo[0], BT_SBC_RECV_FIFO_SIZE);
	
	APP_DBG("Bt Handfree Call mode\n");

#ifdef BT_HFP_CALL_DURATION_DISP
	BtHfActiveTimeInit();
#endif

	SetBtHfSyncVolume(mainAppCt.HfVolume);
	AudioHfVolSet(mainAppCt.HfVolume);
	gCtrlVars.AutoRefresh = AutoRefresh_ALL_PARA;
	
	BtHfModeRunningConfig();
#ifdef CFG_FUNC_REMIND_SOUND_EN
	BtHfRingRemindNumberInit();
#endif
		
	APP_DBG("BT HF Run\n");

	AudioCodecGainUpdata();//update hardware config

	//bt hf ģʽ��ʼ�����
	sBtHfModeEnterFlag = 0;
	BtHfModeEnterFlag = 0;
	mainAppCt.gSysVol.MuteFlag = 1;//��ʱ�����ͨ������������ (hfpvolsetʱ���mute ��ʱdac����δ��ʼ��)
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}

	//ע�� ͨ�������м���ֻ�ͨ��״̬����
	BtHfpRunloopRegister();
	return TRUE;
}

/*****************************************************************************************
 * 
 ****************************************************************************************/
void BtHfMsgToParent(uint16_t id)
{
	MessageContext		msgSend;

	// Send message to main app
	msgSend.msgId		= id;
	MessageSend(GetMainMessageHandle(), &msgSend);
}

/*****************************************************************************************
 * 
 ****************************************************************************************/
void BtHfTaskResume(void)
{
	if((!gBtHfCt)||(gBtHfCt->ModeKillFlag))
		return;
	
	APP_DBG("Bt Handfree Mode Resume\n");
	
#ifdef CFG_PARA_HFP_SYNC
	AudioCoreSinkAdjust(AUDIO_HF_SCO_SINK_NUM, TRUE);
#ifndef CFG_PARA_HFP_FREQ_ADJUST
	AudioCoreSourceAdjust(APP_SOURCE_NUM, TRUE);
#endif
#endif
}

/*****************************************************************************************
 * 
 ****************************************************************************************/
void BtHfRun(uint16_t msgId)
{
	if(sBtHfModeEixtList)
	{
		BtHfModeExit();
		sBtHfModeEixtList = 0;
	}

	switch(msgId)
	{
		case MSG_BT_HF_MODE_CODEC_UPDATE:
		{
			if(gBtHfCt->codecType == HFP_AUDIO_DATA_mSBC)
			{
				AudioCoreSourceChange(APP_SOURCE_NUM, 1, BTHF_MSBC_SAMPLE_RATE);
				#if defined(CFG_PARA_HFP_SYNC) && !defined(CFG_PARA_HFP_FREQ_ADJUST)
				AudioCoreSourceDepthChange(APP_SOURCE_NUM, DECODER_FIFO_SIZE_FOR_SBC / 2);
				#endif
				AudioCoreSinkChange(AUDIO_HF_SCO_SINK_NUM, 1, BTHF_MSBC_SAMPLE_RATE);
			}
			else
			{
				AudioCoreSourceChange(APP_SOURCE_NUM, 1, BTHF_CVSD_SAMPLE_RATE);
				#if defined(CFG_PARA_HFP_SYNC) && !defined(CFG_PARA_HFP_FREQ_ADJUST)
				AudioCoreSourceDepthChange(APP_SOURCE_NUM, BT_SCO_PCM_FIFO_LEN / 2);
				#endif
				AudioCoreSinkChange(AUDIO_HF_SCO_SINK_NUM, 1, BTHF_CVSD_SAMPLE_RATE);
			}
			DBG("CODEC_UPDATE");
			break;
		}
		
		#ifdef CFG_FUNC_REMIND_SOUND_EN			
		case MSG_BT_HF_MODE_REMIND_PLAY:
			if(!SoftFlagGet(SoftFlagNoRemind) 
				&& (GetHfpState(BtCurIndex_Get()) == BT_HFP_STATE_INCOMING)
				&& ((gBtHfCt->CvsdInitFlag) || (gBtHfCt->MsbcInitFlag)))	
			{
				switch(sys_parameter.bt_CallinRingType)
				{
					case USE_NUMBER_REMIND_RING:
						if (gBtHfCt->RemindSoundStart == 0)
						{
							gBtHfCt->PhoneNumberLen = GetBtCallInPhoneNumber(BtCurIndex_Get(),gBtHfCt->PhoneNumber);
							if(gBtHfCt->PhoneNumberLen > 0)
								gBtHfCt->RemindSoundStart = 1;
						}
						break;

					case USE_LOCAL_AND_PHONE_RING:
						if(!GetScoConnectFlag())
						{
							gBtHfCt->RemindSoundStart = 1;
							TimeOutSet(&gBtHfCt->CallRingTmr,0);
						}					
						break;
					
					case USE_ONLY_LOCAL_RING:
						gBtHfCt->RemindSoundStart = 1;
						TimeOutSet(&gBtHfCt->CallRingTmr,0);					
						break;
					
					default:
						break;
				}
			}
			break;
		#endif		
		
		//HFP control
		case MSG_PLAY_PAUSE:
			switch(GetHfpState(BtCurIndex_Get()))
			{
				case BT_HFP_STATE_CONNECTED:
				case BT_HFP_STATE_INCOMING:
					APP_DBG("call answer\n");
					BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_ANSWER_CALL);
					break;

				case BT_HFP_STATE_OUTGOING:
				case BT_HFP_STATE_ACTIVE:
					if(GetBtHfpVoiceRecognition())
					{
						BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_CLOSE_VOICE_RECONGNITION);
					}
					else
					{
						//�Ҷ�ͨ��
						BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_HANGUP); 
					}
					break;
						
				case BT_HFP_STATE_3WAY_INCOMING_CALL:
					//����ǰͨ��, ������һ���绰
					BtHfp_HoldCur_Answer_Call(); 
					break;
				case BT_HFP_STATE_3WAY_OUTGOING_CALL:	
				case BT_HFP_STATE_3WAY_ATCTIVE_CALL:
					//�Ҷ�ͨ��
					BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_HANGUP); 
					break;
					
				default:
					break;
			}
			break;
				
		case MSG_BT_HF_CALL_REJECT:
			switch(GetHfpState(BtCurIndex_Get()))
			{
				case BT_HFP_STATE_INCOMING:
				case BT_HFP_STATE_OUTGOING:
					APP_DBG("call reject\n");
					BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_HANGUP);
					break;
					
				case BT_HFP_STATE_ACTIVE:
					if (btManager.voiceRecognition == 0)
					{
						APP_DBG("Hfp Audio Transfer.\n");
						BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_HF_AUDIO_TRANSFER);
					}
					break;
					
				case BT_HFP_STATE_3WAY_INCOMING_CALL:
					//�Ҷϵ��������磬��������ԭ��ͨ��
					BtHfp_Hangup_Another_Call();
					break;
					
				case BT_HFP_STATE_3WAY_OUTGOING_CALL:
					//�Ҷ�ͨ��
					BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_HANGUP); 
					break;
				case BT_HFP_STATE_3WAY_ATCTIVE_CALL:
					//����������֮���л�
					BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_HF_CALL_HOLDCUR_ANSWER_CALL); 
					break;
				default:
					break;
			}
			break;
		
		case MSG_BT_HF_TRANS_CHANGED:
			if (btManager.voiceRecognition == 0)
			{
				APP_DBG("Hfp Audio Transfer.\n");
				BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_HF_AUDIO_TRANSFER);
			}
			break;
			
		case MSG_BT_HF_VOICE_RECOGNITION:
			if (btManager.voiceRecognition)
			{
				BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_CLOSE_VOICE_RECONGNITION);
			}
			break;
			
		default:
			CommonMsgProccess(msgId);
			break;
	}
	
	BtHf_EncodeProcess();
	ModeDecodeProcess();
#ifdef BT_HFP_CALL_DURATION_DISP
	BtHfActiveTimeUpdate();
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
	BtHfRingRemindNumberRunning();
#endif

	DelayExitBtHfMode();
}


/*****************************************************************************************
 * ɾ��HF����
 ****************************************************************************************/
bool BtHfDeinit(void)
{
	//clear voice recognition flag
	if(gBtHfCt == NULL)
	{
		return TRUE;
	}
	
	//ע�� ͨ�������м���ֻ�ͨ��״̬����
	BtHfpRunloopDeregister();
	
	if(IsAudioPlayerMute() == FALSE)
	{
		HardWareMuteOrUnMute();
	}	
	
	PauseAuidoCore();
	
#ifdef CFG_FUNC_REMIND_SOUND_EN
	BtHfRingRemindNumberStop();
#endif	
	APP_DBG("BT HF Deinit\n");
	
	SetBtHfpVoiceRecognition(0);
	BtAppiFunc_SaveScoData(NULL);
	AudioCoreSourceDeinit(BT_HF_SOURCE_NUM)	;
	AudioCoreSinkDeinit(AUDIO_HF_SCO_SINK_NUM);

	//BtHfDeinitialize();	
	AudioCoreProcessConfig((void*)AudioNoAppProcess);

	//msbc
	//if(gBtHfCt->codecType == HFP_AUDIO_DATA_mSBC)
	{
		DecoderServiceDeinit(DECODER_MODE_CHANNEL);
	}

	if(gBtHfCt->ScoOutPcmFifoMutex != NULL)
	{
		vSemaphoreDelete(gBtHfCt->ScoOutPcmFifoMutex);
		gBtHfCt->ScoOutPcmFifoMutex = NULL;
	}

	//msbc decoder/encoder deinit
	BtHf_MsbcDecoderDeinit();
	BtHf_MsbcEncoderDeinit();

	ModeCommonDeinit();//mic dac ���� �ȴ���һ��ģʽ����
	mainAppCt.EffectMode = gBtHfCt->SystemEffectMode;	//�ָ�EffectMode

#ifdef CFG_FUNC_REMIND_SOUND_EN
	BtHfRingRemindNumberInit();
#endif	

	//�ָ�����ģʽ������ֵ
	AudioMusicVolSet(MusicVolume);

	//ͨ��ģʽ�˳���ɣ������־
	BtHfModeExitFlag = 0;
	sBtHfModeEixtList = 0;
	SetScoConnectFlag(FALSE);

#if (BT_LINK_DEV_NUM == 2)
	if(!BtHfModeEnterFlag)//�˳������У��д�绰��Ҫ����ͨ��ģʽ �������־
	{
		//���ͨ����־
		FirstTalkingPhoneIndexSet(0xff);
		//SecondTalkingPhoneIndexSet(0xff);//Ŀǰ����secondindex��δʹ��
	}
#endif

	osPortFree(gBtHfCt);
	gBtHfCt = NULL;
	APP_DBG("!!gBtHfCt\n");
	
	//�����������ȼ��ָ���Ĭ�����ȼ�(4)
	vTaskPrioritySet(GetBtStackServiceTaskHandle(), GetBtStackServiceTaskPrio());

	return TRUE;
}

/*****************************************************************************************
 ****************************************************************************************/
MessageHandle GetBtHfMessageHandle(void)
{
	if(gBtHfCt == NULL)
	{
		return NULL;
	}
	return GetSysModeMsgHandle();
}

/*****************************************************************************************
 * ����/�˳�HFģʽ
 ****************************************************************************************/
void BtHfModeEnter_Index(uint8_t index)
{
	if((btManager.btLinked_env[index].btLinkState == 0)||(SoftFlagGet(SoftFlagWaitBtRemindEnd)))
	{
		DBG("btManager.btLinked_env[%d].btLinkState = %d   SoftFlagGet(SoftFlagWaitBtRemindEnd) = %d\n", index, btManager.btLinked_env[index].btLinkState, SoftFlagGet(SoftFlagWaitBtRemindEnd));
		if(!SoftFlagGet(SoftFlagDelayEnterBtHf))
			SoftFlagRegister(SoftFlagDelayEnterBtHf);
		return;
	}
	if(BtHfModeExitFlag)
	{
		APP_DBG("BtHfModeEnter: BtHfModeExitFlag is return\n");
		BtHfModeEnterFlag = 1;
		return;
	}

	if((GetSystemMode() != ModeBtHfPlay)&&(!sBtHfModeEnterFlag))
	{
		extern bool GetBtCurPlayState(void);
		if((GetSystemMode() == ModeBtAudioPlay) && GetBtCurPlayState() && (!SoftFlagGet(SoftFlagBtCurPlayStateMask)))
		{
			SoftFlagRegister(SoftFlagBtCurPlayStateMask);
		}
		
		if(GetSysModeState(ModeBtHfPlay) == ModeStateSusend)
		{
			SetSysModeState(ModeBtHfPlay,ModeStateReady);
		}
		
		BtHfMsgToParent(MSG_DEVICE_SERVICE_BTHF_IN);
		//ͨ��ģʽ�����־������
		sBtHfModeEnterFlag = 1;
	}
	else
	{
		DBG("GetSystemMode() = %d   sBtHfModeEnterFlag = %d\n", (int)GetSystemMode(), (int)sBtHfModeEnterFlag);
	}
}

void BtHfModeEnter(void)
{
#if (BT_LINK_DEV_NUM == 2)
	if((btManager.btLinked_env[0].btLinkState == 0 && btManager.btLinked_env[1].btLinkState == 0)||(SoftFlagGet(SoftFlagWaitBtRemindEnd)))
#else
	if((btManager.btLinked_env[0].btLinkState == 0)||(SoftFlagGet(SoftFlagWaitBtRemindEnd)))
#endif
	{
		DBG("btManager.btLinked_env[0].btLinkState = %d\n", btManager.btLinked_env[0].btLinkState);
		DBG("SoftFlagGet(SoftFlagWaitBtRemindEnd) = %d\n", SoftFlagGet(SoftFlagWaitBtRemindEnd));
		if(!SoftFlagGet(SoftFlagDelayEnterBtHf))
		{
			SoftFlagRegister(SoftFlagDelayEnterBtHf);
		}
		return;
	}

	if(BtHfModeExitFlag)
	{
		APP_DBG("BtHfModeEnter: BtHfModeExitFlag is return\n");
		BtHfModeEnterFlag = 1;
		return;
	}
	DBG("GetSystemMode() = %d   sBtHfModeEnterFlag = %d\n", (int)GetSystemMode(), (int)sBtHfModeEnterFlag);
	if((GetSystemMode() != ModeBtHfPlay)&&(!sBtHfModeEnterFlag))
	{
		if((GetSystemMode() == ModeBtAudioPlay))
		{
			extern bool GetBtCurPlayState(void);
			if(GetBtCurPlayState())
			{
				if(!SoftFlagGet(SoftFlagBtCurPlayStateMask))
					SoftFlagRegister(SoftFlagBtCurPlayStateMask);
			}
		}
		
		if(GetSysModeState(ModeBtHfPlay) == ModeStateSusend)
		{
			SetSysModeState(ModeBtHfPlay,ModeStateReady);
		}
		
		BtHfMsgToParent(MSG_DEVICE_SERVICE_BTHF_IN);
		//ͨ��ģʽ�����־������
		sBtHfModeEnterFlag = 1;
	}
}

/*****************************************************************************************
 * 
 ****************************************************************************************/
void BtHfModeExit(void)
{
	SoftFlagDeregister(SoftFlagDelayEnterBtHf);

	if(flagVoiceDelayExitBtHf &&(flagVoiceDelayExitBtHf<=DELAY_EXIT_BT_HF_TIME))
	{
		return;
	}
	flagVoiceDelayExitBtHf = 0;

	DBG("BtHfModeExitFlag = %d\n", (int)BtHfModeExitFlag);
	//�˳��������ڽ���ʱ�����ٴδ�����ֹ����
	if(BtHfModeExitFlag)
		return;
	
	if(GetSystemMode() == ModeBtHfPlay)
	{
		//��ͨ��ģʽ��δ��ȫ��ʼ����ɣ����˳�����Ϣ���뵽�����Ķ��У��ڳ�ʼ����ɺ��˳�ͨ��ģʽ
		if((gBtHfCt==NULL)||(sBtHfModeEnterFlag))
		{
			sBtHfModeEixtList = 1;
			DBG("sBtHfModeEnterFlag1 = %d\n", (int)sBtHfModeEnterFlag);
			return;
		}
		
		//ResourceDeregister(AppResourceBtHf);
		BtHfMsgToParent(MSG_DEVICE_SERVICE_BTHF_OUT);

		//���˳�ͨ��ģʽǰ,�ȹر�sink2ͨ·,����AudioCore����תʱ,ʹ�õ�sink��ص��ڴ�,��Hfģʽkill�������ͻ,����Ұָ������
		AudioCoreSinkDisable(AUDIO_HF_SCO_SINK_NUM);

		gBtHfCt->ModeKillFlag=1;

		//ͨ��ģʽ�˳���־������
		BtHfModeExitFlag = 1;
	}
	else if(sBtHfModeEnterFlag)
	{
		//��ͨ��ģʽ���̻�δ���У����˳�����Ϣ���뵽�����Ķ��У��ڽ���ͨ��ģʽ��ʼ����ɺ��˳�ͨ��ģʽ
		sBtHfModeEixtList = 1;
		DBG("sBtHfModeEnterFlag2 = %d\n", (int)sBtHfModeEnterFlag);
		return;
	}
	else
	{
		DBG("BtHfModeExit is error\n");
	}
}

/*****************************************************************************************
 * 
 ****************************************************************************************/
void DelayExitBtHfModeSet(void)
{
//#if (BT_LINK_DEV_NUM == 2)
//	if(SecondTalkingPhoneIndexGet()==0xff)//��ֹ�������ֽ�����ת����B�ֻ�ͨ��ģʽ      //Ŀǰ����secondindex��δʹ��
	flagVoiceDelayExitBtHf = 1;
//#endif
}

bool GetDelayExitBtHfMode(void)
{
	if(flagVoiceDelayExitBtHf)
	{
		return TRUE;
	}
	return FALSE;
}

/*****************************************************************************************
 * 
 ****************************************************************************************/
void DelayExitBtHfModeCancel(void)
{
	flagVoiceDelayExitBtHf = 0;
}

void DelayExitBtHfMode(void)
{
	//delay exit hf mode
	if(flagVoiceDelayExitBtHf)
	{
		flagVoiceDelayExitBtHf++;
		if(flagVoiceDelayExitBtHf>=DELAY_EXIT_BT_HF_TIME)
		{
			flagVoiceDelayExitBtHf=0;
			BtHfModeExit();
		}
	}
}

/*****************************************************************************************
 * 
 ****************************************************************************************/
void SpecialDeviceFunc(void)
{
	if(gSpecificDevice)
	{
		if((GetSystemMode() == ModeBtHfPlay)&&(GetHfpState(BtCurIndex_Get()) >= BT_HFP_STATE_CONNECTED))
		{
			//HfpAudioTransfer();
			BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_HF_AUDIO_TRANSFER);
		}
	}
}

#endif //#if defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT)

