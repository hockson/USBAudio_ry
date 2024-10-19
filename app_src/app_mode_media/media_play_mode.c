/**
 **************************************************************************************
 * @file    media_play_mode.c
 * @brief   
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2017-12-28 18:00:00$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include "main_task.h"
#include "media_play_api.h"
#include "media_play_mode.h"
#include "remind_sound.h"
#include "device_detect.h"
#include "audio_effect.h"
#include "audio_vol.h"
#include "dma.h"
#include "ctrlvars.h"

#if defined(CFG_APP_USB_PLAY_MODE_EN) || defined(CFG_APP_CARD_PLAY_MODE_EN)
typedef struct _mediaPlayContext
{

	DEV_ID				Device;	//��ǰ���ŵ��豸
	//QueueHandle_t 		audioMutex;
	//QueueHandle_t		pcmBufMutex;
	uint8_t				SourceNum;//�������ͻط�ʹ�ò�ͬͨ����ǰ������Ч
}MediaPlayContext;
static MediaPlayContext*	sMediaPlayCt = NULL;
static uint32_t sPlugOutTimeOutCount = 0; 
extern uint32_t CmdErrCnt;
extern volatile uint32_t gInsertEventDelayActTimer;

static const uint8_t DmaChannelMap[6] = {
	PERIPHERAL_ID_SDIO_RX,
	PERIPHERAL_ID_AUDIO_ADC0_RX,
	PERIPHERAL_ID_AUDIO_ADC1_RX,
	PERIPHERAL_ID_AUDIO_DAC0_TX,
	255,
#ifdef CFG_RES_AUDIO_I2SOUT_EN
	PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE,
#else
	255
#endif
};


uint8_t MediaPlayDevice(void)
{
	if(sMediaPlayCt == NULL)
	{
		return 255;
	}
	return sMediaPlayCt->Device;
}

static  void MediaPlayerSetAudioSource()
{
	AudioCoreSourceEnable(sMediaPlayCt->SourceNum);
	//AudioCoreSourceUnmute(sMediaPlayCt->SourceNum, TRUE, TRUE);

#ifdef CFG_FUNC_DISPLAY_EN
	MessageContext		msgSend;
	msgSend.msgId = MSG_DISPLAY_SERVICE_FILE_NUM;
	MessageSend(GetSysModeMsgHandle(), &msgSend);
#endif
	
}

static void MediaPlayMsgProcess(uint16_t msgId)
{
#ifdef CFG_FUNC_DISPLAY_EN
	MessageContext	msgSend;
#endif
	switch(msgId)
	{
		case MSG_REPEAT:
			APP_DBG("Media:MSG_REPEAT\n");
			MediaPlayerSwitchPlayMode(PLAY_MODE_SUM);
#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_REPEAT;
            MessageSend(GetSysModeMsgHandle(), &msgSend);
#endif
			break;

		case MSG_FOLDER_PRE:
			if(CmdErrCnt >= 3)//��ʱ����U���쳣����U�̲��ŵ����
			{
				if(!IsMediaPlugOut())
				{
					SendModeKeyMsg();
				}
				SoftFlagRegister(SoftFlagMediaDevicePlutOut);
				CmdErrCnt = 0;
				return;
			}
			MediaPlayerPreFolder();
			break;

		case MSG_FOLDER_NEXT:
			if(CmdErrCnt >= 3)//��ʱ����U���쳣����U�̲��ŵ����
			{
				if(!IsMediaPlugOut())
				{
					SendModeKeyMsg();
				}
				SoftFlagRegister(SoftFlagMediaDevicePlutOut);
				CmdErrCnt = 0;
				return;
			}
			MediaPlayerNextFolder();
			break;
		case MSG_PLAY_PAUSE:
			APP_DBG("Media:MSG_PLAY_PAUSE\n");
			MediaPlayerPlayPause();
#ifdef CFG_FUNC_DISPLAY_EN
			msgSend.msgId = MSG_DISPLAY_SERVICE_MEDIA;
			MessageSend(GetSysModeMsgHandle(), &msgSend);
#endif
			break;

		case MSG_STOP:
			APP_DBG("Media:MSG_STOP\n");
			MediaPlayerStop();
			break;

		case MSG_DECODER_SERVICE_MUTLI_EMPTY:
			APP_DBG("Media:Disk ERROR\n");
			MediaPlayerStop();
			SendModeKeyMsg();
			break;
		case MSG_DECODER_SERVICE_SONG_PLAY_END:
		//case MSG_DECODER_SERVICE_FF_END_SONG:
			if(GetDecoderState(DECODER_MODE_CHANNEL) == DecoderStatePause)//ȷ�����Ž��� ��������̬ͣ������stop��Ϣ����
			{
				APP_DBG("Media:Last end, play next\n");
#ifdef	CFG_APP_USB_PLAY_MODE_EN
				if(CmdErrCnt >= 3)//��ʱ����U���쳣����U�̲��ŵ����
				{
					APP_DBG("cmd error >= 3\n");
					if(!IsMediaPlugOut())
					{
						SendModeKeyMsg();
					}
					SoftFlagRegister(SoftFlagMediaDevicePlutOut);
					CmdErrCnt = 0;
					return;
				}
#endif
				if(gMediaPlayer->CurPlayMode==PLAY_MODE_REPEAT_OFF)
				{
					//gMediaPlayer->CurFileIndex;
					if(gMediaPlayer->CurFileIndex+1 > gMediaPlayer->TotalFileSumInDisk)
					{
						gMediaPlayer->CurFileIndex = 1;
						MediaPlayerStop();
					}
					else
					{
						DecoderMuteAndStop(DECODER_MODE_CHANNEL);
						MediaPlayerNextSong(TRUE);
					}
				}
				else
				{
					DecoderMuteAndStop(DECODER_MODE_CHANNEL);
					MediaPlayerNextSong(TRUE);
				}
			}
			break;
		case MSG_NEXT:
#ifdef CFG_FUNC_REMIND_SOUND_EN
			if(CmdErrCnt < 3)
			{
				DecoderMuteAndStop(DECODER_MODE_CHANNEL);
				if(RemindSoundServiceItemRequest(SOUND_REMIND_XIAYISOU, REMIND_PRIO_NORMAL|REMIND_ATTR_NEED_MUTE_APP_SOURCE))//RemindSound request
				{
					gMediaPlayer->RemindSoundFlag = 1;
					break;
				}
			}
#endif
		case MSG_SOFT_NEXT: 	
			APP_DBG("Media:play next\n");
			if(CmdErrCnt >= 3)//��ʱ����U���쳣����U�̲��ŵ����
			{
				if(!IsMediaPlugOut())
				{
					SendModeKeyMsg();
				}
				SoftFlagRegister(SoftFlagMediaDevicePlutOut);
				CmdErrCnt = 0;
				return;
			}
			DecoderMuteAndStop(DECODER_MODE_CHANNEL);
			MediaPlayerNextSong(FALSE);
			break;
		
		case MSG_PRE:
#ifdef CFG_FUNC_REMIND_SOUND_EN
			if(CmdErrCnt < 3)
			{
				DecoderMuteAndStop(DECODER_MODE_CHANNEL);
				if(RemindSoundServiceItemRequest(SOUND_REMIND_SHANGYIS, REMIND_PRIO_NORMAL|REMIND_ATTR_NEED_MUTE_APP_SOURCE))//RemindSound request
				{
					gMediaPlayer->RemindSoundFlag = 2;
					break;
				}
			}
#endif
		case MSG_SOFT_PRE:
			APP_DBG("Media:MSG_PRE\n");
			if(CmdErrCnt >= 3)//��ʱ����U���쳣����U�̲��ŵ����
			{
				if(!IsMediaPlugOut())
				{
					SendModeKeyMsg();
				}
				SoftFlagRegister(SoftFlagMediaDevicePlutOut);
				CmdErrCnt = 0;
				return;
			}
			
			DecoderMuteAndStop(DECODER_MODE_CHANNEL);
			MediaPlayerPreSong();
			break;
#ifdef CFG_FUNC_REMIND_SOUND_EN
		case MSG_REMIND_PLAY_END:
			if(gMediaPlayer->RemindSoundFlag == 1)
			{
				MessageContext	msgSend;
				msgSend.msgId = MSG_SOFT_NEXT;
				MessageSend(GetSysModeMsgHandle(), &msgSend);
			}
			if(gMediaPlayer->RemindSoundFlag == 2)
			{
				MessageContext	msgSend;
				msgSend.msgId = MSG_SOFT_PRE;
				MessageSend(GetSysModeMsgHandle(), &msgSend);
			}
			gMediaPlayer->RemindSoundFlag = 0;
			break;
#endif
		case MSG_FF_START:
			MediaPlayerFastForward();
			APP_DBG("Media:MSG_FF_START\n");
			break;
		
		case MSG_FB_START:
			MediaPlayerFastBackward();
			APP_DBG("Media:MSG_FB_START\n");
			break;

		case MSG_FF_FB_END:
			MediaPlayerFFFBEnd();
			APP_DBG("Media:MSG_FF_FB_END\n");
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
            #ifdef CFG_FUNC_DISPLAY_EN
             msgSend.msgId = MSG_DISPLAY_SERVICE_NUMBER;
             MessageSend(GetSysModeMsgHandle(), &msgSend);
            #endif
			break;
#endif

		case MSG_DECODER_SERVICE_UPDATA_PLAY_TIME:
			MediaPlayerTimeUpdate();
            #ifdef CFG_FUNC_DISPLAY_EN
			msgSend.msgId = MSG_DISPLAY_SERVICE_MEDIA;
			MessageSend(GetSysModeMsgHandle(), &msgSend);
            #endif
			break;

		case MSG_DECODER_MODE_PLAYING:
			AudioCoreSourceUnmute(sMediaPlayCt->SourceNum, TRUE, TRUE);
			break;

		case MSG_REPEAT_AB:
			APP_DBG("MSG_REPEAT_AB\n");
			MediaPlayerRepeatAB();
			break;
			
		default:
			if(gMediaPlayer->RepeatAB.RepeatFlag == REPEAT_OPENED
				&& (GetSystemMode() == ModeCardAudioPlay || GetSystemMode() == ModeUDiskAudioPlay))
			{
				MediaPlayerTimerCB();
			}
			CommonMsgProccess(msgId);
			break;
	}
#ifdef CFG_RES_IR_NUMBERKEY
	if(Number_select_flag)
	{
		if(IsTimeOut(&Number_selectTimer))
		{
			if((gMediaPlayer->CurPlayMode ==PLAY_MODE_REPEAT_FOLDER) || (gMediaPlayer->CurPlayMode == PLAY_MODE_RANDOM_FOLDER))
			{
				if((Number_value <= (gMediaPlayer->PlayerFolder.FirstFileIndex + gMediaPlayer->PlayerFolder.FileNumLen - 1)) && (Number_value))
				{
					DecoderMuteAndStop(DECODER_MODE_CHANNEL);
					gMediaPlayer->CurFileIndex = Number_value;
					gMediaPlayer->CurPlayTime = 0;
					gMediaPlayer->SongSwitchFlag = 0;
					SetMediaPlayerState(PLAYER_STATE_PLAYING);
					MediaPlayerSong();
				}
			}
#ifdef FUNC_BROWSER_PARALLEL_EN
			else if(GetBrowserPlay_state()==Browser_Play_None)
			{
				APP_DBG("browser normal playing,not support ir number key select song \n");
			}
#endif
			else
			{
				if((Number_value <= gMediaPlayer->TotalFileSumInDisk) && (Number_value))
				{
					DecoderMuteAndStop(DECODER_MODE_CHANNEL);
					gMediaPlayer->CurFileIndex = Number_value;
					gMediaPlayer->CurPlayTime = 0;
					gMediaPlayer->SongSwitchFlag = 0;
					SetMediaPlayerState(PLAYER_STATE_PLAYING);
					MediaPlayerSong();
				}
			}
			Number_select_flag = 0;
		}
	}
#endif
}

/**
 * @func        MediaPlayInit
 * @brief       Mediaģʽ�������ã���Դ��ʼ��
 * @param       MessageHandle   
 * @Output      None
 * @return      bool
 * @Others      ����顢Dac��AudioCore���ã�����Դ��DecoderService
 * @Others      ��������Decoder��audiocore���к���ָ�룬audioCore��Dacͬ����audiocoreService����������
 * Record
 */
bool MediaPlayInit(void)
{
	bool ret = FALSE;

	if(sMediaPlayCt != NULL)
	{
		return FALSE;
	}
	if(gInsertEventDelayActTimer)
	{
#ifdef CFG_FUNC_UDISK_DETECT
		if(GetUdiscState()==DETECT_STATE_OUT && ((GetSystemMode()==ModeUDiskAudioPlay)
#ifdef CFG_FUNC_RECORDER_EN
					||(GetSystemMode()==ModeUDiskPlayBack)
#endif
					))
		{
			APP_DBG(" not have usb,return\n");
			return FALSE;
		}
#endif
#ifdef CFG_FUNC_CARD_DETECT
		if(GetCardState()==DETECT_STATE_OUT && ((GetSystemMode()==ModeCardAudioPlay)
#ifdef CFG_FUNC_RECORDER_EN
				||(GetSystemMode()==ModeCardPlayBack)
#endif
				))
		{
			APP_DBG(" not have sd,return\n");
			return FALSE;
		}
#endif
	}
	//DMA channel
	DMA_ChannelAllocTableSet((uint8_t*)DmaChannelMap);
	if(!ModeCommonInit())
	{
		ModeCommonDeinit();
		DBG("Common Audio error!\n");
		return FALSE;
	}

	APP_DBG("Media Play Init\n");

	sMediaPlayCt = (MediaPlayContext*)osPortMalloc(sizeof(MediaPlayContext));
	if(sMediaPlayCt == NULL)
	{
		return FALSE;
	}
	memset(sMediaPlayCt, 0, sizeof(MediaPlayContext));

#ifdef CFG_FUNC_RECORDER_EN
	if(GetSystemMode() == ModeCardPlayBack || GetSystemMode() == ModeUDiskPlayBack)// || GetSystemMode() == AppModeFlashFsPlayBack)
	{
		sMediaPlayCt->SourceNum = PLAYBACK_SOURCE_NUM;
	}
	else
#endif
	{
		sMediaPlayCt->SourceNum = MEDIA_PLAY_DECODER_SOURCE_NUM;
	}
	DecoderSourceNumSet(sMediaPlayCt->SourceNum, DECODER_MODE_CHANNEL);
	AudioCoreIO	AudioIOSet;
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
	AudioIOSet.Adapt = SRC_ONLY;
	AudioIOSet.Sync = FALSE;
	AudioIOSet.Channels = 2;
	AudioIOSet.Net = DefaultNet;
	AudioIOSet.DataIOFunc = ModeDecoderPcmDataGet;
	AudioIOSet.LenGetFunc = ModeDecoderPcmDataLenGet;
	AudioIOSet.SampleRate = AudioCoreMixSampleRateGet(DefaultNet);//��ʼֵ
#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = 0;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 1;//��Ҫ���ݽ���λ����չ
#endif
	if(!AudioCoreSourceInit(&AudioIOSet, sMediaPlayCt->SourceNum))
	{
		DBG("mediaplay source error!\n");
		return FALSE;
	}
	
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioCoreProcessConfig((AudioCoreProcessFunc)AudioMusicProcess);
#else
	AudioCoreProcessConfig((AudioCoreProcessFunc)AudioBypassProcess);
#endif
	
	sMediaPlayCt->Device = (GetSystemMode() == ModeUDiskAudioPlay
#ifdef CFG_FUNC_RECORDER_EN
			|| GetSystemMode() == ModeUDiskPlayBack
#endif
			) ? DEV_ID_USB : DEV_ID_SD;
	APP_DBG("Media:Dev %d\n", sMediaPlayCt->Device);
	if(gMediaPlayer != NULL)
	{
		APP_DBG("player is reopened\n");
	}
	else
	{
		gMediaPlayer = osPortMalloc(sizeof(MEDIA_PLAYER));
		if(gMediaPlayer == NULL)
		{
			APP_DBG("gMediaPlayer malloc error\n");
			return FALSE;
		}
	}
	memset(gMediaPlayer, 0, sizeof(MEDIA_PLAYER));
	
	gMediaPlayer->AccRamBlk = (uint8_t*)osPortMalloc(MAX_ACC_RAM_SIZE);
	if(gMediaPlayer->AccRamBlk == NULL)
	{
		APP_DBG("AccRamBlk malloc error\n");
		return FALSE;
	}
#if defined(CFG_FUNC_BREAKPOINT_EN) && (defined(CFG_APP_USB_PLAY_MODE_EN) || defined(CFG_APP_CARD_PLAY_MODE_EN))
	if(GetSystemMode() == ModeUDiskAudioPlay || GetSystemMode() == ModeCardAudioPlay)
	{
		gMediaPlayer->CurPlayMode = BPDiskSongPlayModeGet();
	}
#endif

#ifdef CFG_RES_IR_NUMBERKEY
	Number_select_flag = 0;
	Number_value = 0;
#endif
	SoftFlagDeregister(SoftFlagMediaDevicePlutOut);
	SoftFlagRegister(SoftFlagMediaModeRead);

	AudioCodecGainUpdata();//update hardware config
	
#ifdef FUNC_BROWSER_PARALLEL_EN
	BrowserVarDefaultSet();
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(GetSystemMode() == ModeUDiskAudioPlay)
		ret = RemindSoundServiceItemRequest(SOUND_REMIND_UPANMODE, REMIND_ATTR_NEED_MUTE_APP_SOURCE);
	if(GetSystemMode() == ModeCardAudioPlay)
		ret = RemindSoundServiceItemRequest(SOUND_REMIND_CARDMODE, REMIND_ATTR_NEED_MUTE_APP_SOURCE);
#ifdef CFG_FUNC_RECORDER_EN
	if(GetSystemMode() == ModeUDiskPlayBack || GetSystemMode() == ModeCardPlayBack)
		ret = RemindSoundServiceItemRequest(SOUND_REMIND_RECHUIFA, REMIND_ATTR_NEED_MUTE_APP_SOURCE);
#endif
	gMediaPlayer->WaitFlag = ret;
	if(ret == FALSE)
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
#ifdef CFG_FUNC_RECORDER_EN
	if(GetSystemMode() == ModeCardPlayBack || GetSystemMode() == ModeUDiskPlayBack )//|| GetSystemMode() ==AppModeFlashFsPlayBack)
	{
		return DecoderServiceInit(GetSysModeMsgHandle(),DECODER_MODE_CHANNEL, DECODER_BUF_SIZE_MP3, DECODER_FIFO_SIZE_FOR_MP3);//DECODER_BUF_SIZE_MP3
	}
	else
#endif
	{
		return DecoderServiceInit(GetSysModeMsgHandle(),DECODER_MODE_CHANNEL, DECODER_BUF_SIZE, DECODER_FIFO_SIZE_FOR_PLAYER);// decode step1
	}
	return TRUE;
}

void MediaPlayRun(uint16_t msgId)
{
#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(gMediaPlayer->WaitFlag)
	{//����ʾ������
		if(!RemindSoundIsPlay())
		{
			gMediaPlayer->WaitFlag = FALSE;
		}
		return;
	}
#endif
	if(SoftFlagGet(SoftFlagMediaModeRead))
	{
		if(!MediaPlayerInitialize(sMediaPlayCt->Device, 1, 1))// decode step 2: include : DecoderInit(&gMediaPlayer->PlayerFile,DECODER_MODE_CHANNEL,(int32_t)IO_TYPE_FILE, (int32_t)SongFileType)
		{
			APP_DBG("Media decoder init error ,exit media init\n");

#ifdef CFG_FUNC_RECORDER_EN
			extern void RecServierToParent(uint16_t id);
			if(GetSystemMode() == ModeUDiskPlayBack)
			{
				RecServierToParent(MSG_DEVICE_SERVICE_U_DISK_BACK_OUT);
				return ;

			}
			else if(GetSystemMode() == ModeCardPlayBack)
			{
				RecServierToParent(MSG_DEVICE_SERVICE_CARD_BACK_OUT);
				return ;
			}
#endif

			if(!IsMediaPlugOut() && (GetSystemMode() == ModeUDiskAudioPlay || GetSystemMode() == ModeCardAudioPlay))
			{
				SendModeKeyMsg();
			}
			SoftFlagRegister(SoftFlagMediaDevicePlutOut);
			SoftFlagDeregister(SoftFlagMediaModeRead);
			return ;
		}
		SoftFlagDeregister(SoftFlagMediaModeRead);
		MediaPlayerSetAudioSource();
		DecoderPlay(DECODER_MODE_CHANNEL);//  decode step 3
		APP_DBG("Media Play run\n");
	}
#if FLASH_BOOT_EN
	if(!SoftFlagGet(SoftFlagUpgradeOK)&&(SoftFlagGet(SoftFlagMvaInUDisk)||SoftFlagGet(SoftFlagMvaInCard)))
	{
		return;
	}
#endif
	if(SoftFlagGet(SoftFlagMediaDevicePlutOut))
	{
		sPlugOutTimeOutCount++;
		if(sPlugOutTimeOutCount > 600) 
		{
			SendModeKeyMsg();
			sPlugOutTimeOutCount = 0;
		}	
		return;
	}

	MediaPlayMsgProcess(msgId);	
#if (defined(FUNC_BROWSER_PARALLEL_EN) || defined(FUNC_BROWSER_TREE_EN))
	BrowserMsgProcess(msgId);
#endif
	ModeDecodeProcess();//  decode step 4

}

bool MediaPlayDeinit(void)
{
	APP_DBG("Media Play Deinit\n");
	if(sMediaPlayCt == NULL)
	{
		return TRUE;
	}
	
	if(IsAudioPlayerMute() == FALSE)
	{
		HardWareMuteOrUnMute();
	}
	
	CmdErrCnt = 0;
	SoftFlagDeregister(SoftFlagMediaModeRead);
	SoftFlagDeregister(SoftFlagMediaNextOrPrev);
	
	DecoderServiceStop(DECODER_MODE_CHANNEL);
	PauseAuidoCore();
	MediaPlayerCloseSongFile();

//ע��˴��������TaskStateCreated,����stop������δinit��
	AudioCoreProcessConfig((void*)AudioNoAppProcess);
	AudioCoreSourceDisable(sMediaPlayCt->SourceNum);
	//AudioCoreSourceUnmute(sMediaPlayCt->SourceNum, TRUE, TRUE);
//	AudioCoreSourceDeinit(REMIND_SOURCE_NUM);
	AudioCoreSourceDeinit(sMediaPlayCt->SourceNum);
	//Kill used services
	DecoderServiceDeinit(DECODER_MODE_CHANNEL);
	if(GetSystemMode() == ModeUDiskAudioPlay
	#ifdef CFG_FUNC_RECORDER_EN
				|| GetSystemMode() == ModeUDiskPlayBack
	#endif
		 )
	{
#ifdef	CFG_APP_USB_PLAY_MODE_EN
		f_unmount(MEDIA_VOLUME_STR_U);
		osMutexUnlock(UDiskMutex);
#ifdef CFG_FUNC_UDISK_DETECT
		if(!IsUDiskLink())
		{
			SoftFlagDeregister(SoftFlagUpgradeOK);
		}
#endif
		APP_DBG("unmount u disk\n");
#endif
	}
	else if(GetSystemMode() == ModeCardAudioPlay
#ifdef CFG_FUNC_RECORDER_EN
			|| GetSystemMode() == ModeCardPlayBack
#endif
		)
	{
#if (defined(CFG_APP_CARD_PLAY_MODE_EN) )
		f_unmount(MEDIA_VOLUME_STR_C);
		SDCardDeinit(CFG_RES_CARD_GPIO);
		osMutexUnlock(SDIOMutex);
#ifdef CFG_FUNC_CARD_DETECT
		if(GetCardState() == DETECT_STATE_OUT)
#endif
		{
			SoftFlagDeregister(SoftFlagUpgradeOK);
		}
		APP_DBG("unmount sd card\n");
#endif
	}
	
	//PortFree
	ffpresearch_deinit();
	if(gMediaPlayer->AccRamBlk!= NULL)
	{
		osPortFree(gMediaPlayer->AccRamBlk);
		gMediaPlayer->AccRamBlk = NULL;
	}

	extern void DecoderTimeClear(DecoderChannels DecoderChannel);
	DecoderTimeClear(DECODER_MODE_CHANNEL);

	MediaPlayerDeinitialize();

	ModeCommonDeinit();

	osPortFree(sMediaPlayCt);
	sMediaPlayCt = NULL;
	return TRUE;

}

#endif
