/**
 **************************************************************************************
 * @file    audio_vol.c
 * @brief   audio syetem vol set here
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-1-7 15:42:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include "type.h"
#include "app_config.h"
#include "app_message.h"
#include "dac.h"
#include "audio_adc.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "debug.h"
#include "audio_vol.h"
#include "audio_core_api.h"
#include "main_task.h"
#include "timeout.h"
#include "bt_manager.h"
#include "bt_play_mode.h"
#include "remind_sound.h"
#include "bt_app_avrcp_deal.h"
#if (BT_HFP_SUPPORT)
#include "bt_hf_mode.h"
#include "bt_hf_api.h"
#endif
#include "ctrlvars.h"
#ifdef CFG_FUNC_RTC_EN
#include "rtc_ctrl.h"
#endif
#include "breakpoint.h"

#ifdef CFG_FUNC_REMIND_SOUND_EN
#include "remind_sound.h"
#endif

#include "i2s.h"
#include "delay.h"
#include "hdmi_in_api.h"
#include "user_effect_parameter.h"

bool gIsVolSetEnable = FALSE;
int32_t SetChannel = 0xff;
static int32_t Cnt = -1;

uint8_t ChannelValid[ AUDIO_CORE_SOURCE_MAX_NUM + AUDIO_CORE_SINK_MAX_NUM] = {0};
extern HDMIInfo *gHdmiCt;
TIMER MenuTimer;//�˵��� ʱ����ƣ��������һ��ʱ������ذ����������رղ˵����ܡ�

uint8_t gBtAbsVolTable[17]={
	0x00, 0x07, 0x0f, 0x17, 0x1f, 0x27, 0x2f, 0x37, 0x3f, 0x47,
	0x4f, 0x57, 0x5f, 0x67, 0x6f, 0x77, 0x7f};

uint8_t gBtAbsVolSetTable[17]={
	0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38, 0x40, 0x48,
	0x50, 0x58, 0x60, 0x68, 0x70, 0x78, 0x7f};

uint8_t BtAbsVolume2VolLevel(uint8_t absValue)
{
	uint8_t i;
	for(i=0;i<=16;i++)
	{
		if(absValue == gBtAbsVolTable[i])
			return i;

		if(absValue < gBtAbsVolTable[i])
			return (i-1);
	}
	if(i>16) i=16;
	return i;
}

uint8_t BtLocalVolLevel2AbsVolme(uint8_t localValue)
{
	return gBtAbsVolSetTable[localValue];
}

void HardWareMuteOrUnMute(void)
{
	mainAppCt.gSysVol.MuteFlag = !mainAppCt.gSysVol.MuteFlag;
#ifdef CFG_RONGYUAN_CMD
#if CUSTOM_MODEL == MODEL_G1
	if(mainAppCt.gSysVol.MuteFlag)
	{

		//GPIO_PortAModeSet(GPIOA6, 0);  // Rx, A9:uart1_rxd_1
		//GPIO_RegOneBitClear(GPIO_A_IE, GPIO_INDEX16);
		//GPIO_RegOneBitSet(GPIO_A_OE, GPIO_INDEX16);
		//GPIO_RegOneBitClear(GPIO_A_OUT, GPIO_INDEX16);
		
		APP_DBG("++++++++++++++++++++++ HardWare Mute\n");
		EXT_MUTE_ON();
	}
	else
	{
		//GPIO_RegOneBitClear(GPIO_A_IE, GPIO_INDEX16);
		//GPIO_RegOneBitSet(GPIO_A_OE, GPIO_INDEX16);
		//GPIO_RegOneBitSet(GPIO_A_OUT, GPIO_INDEX16);
		
		APP_DBG("---------------------- HardWare UnMute\n");
		EXT_MUTE_OFF();

	}
#endif
#endif	
}

bool IsAudioPlayerMute(void)
{
	return mainAppCt.gSysVol.MuteFlag;
}

void AudioPlayerMenu(void)
{
	uint32_t ValidNum = 0;;
	uint32_t i;
	
	TimeOutSet(&MenuTimer, 5000);
	gIsVolSetEnable = TRUE;

	for(i=0; i<AUDIO_CORE_SOURCE_MAX_NUM; i++)
	{
		if(AudioCoreSourceIsEnable(i))
		{
			ChannelValid[ValidNum] =  i;
			ValidNum++;
		}
	}
	for(i=0; i<AUDIO_CORE_SINK_MAX_NUM; i++)
	{
		if(AudioCoreSinkIsEnable(i))
		{
			ChannelValid[ValidNum] = AUDIO_CORE_SOURCE_MAX_NUM + i;//VOL_SET_CHANNEL_BASE_SINK
			ValidNum++;
		}
	}

	APP_DBG("Cnt=%d\n", (int)Cnt);
	SetChannel = ChannelValid[Cnt > ValidNum ? ValidNum : Cnt];

	Cnt++;
	if(Cnt > ValidNum)
	{
		Cnt = 0;
	}
	
	//APP_DBG
	{
	switch(SetChannel)
	{
		  case MIC_SOURCE_NUM:
			APP_DBG("MIC_SOURCE_NUM vol set\n");
		break;
		  case APP_SOURCE_NUM:
			APP_DBG("APP_SOURCE_NUM vol set\n");
		break;
#ifdef  CFG_FUNC_REMIND_SOUND_EN
		  case REMIND_SOURCE_NUM:
			APP_DBG("REMIND_SOURCE_NUM vol set\n");
#endif
		break;
//		  case PLAYBACK_SOURCE_NUM:
//			APP_DBG("PLAYBACK_SOURCE_NUM vol set\n");
//		break;


	      case AUDIO_DAC0_SINK_NUM + AUDIO_CORE_SOURCE_MAX_NUM:
		       APP_DBG("Sink0 vol set(dac0)\n");
		break;

          #if defined(CFG_RES_AUDIO_DACX_EN )
	      case AUDIO_DACX_SINK_NUM + AUDIO_CORE_SOURCE_MAX_NUM:
		APP_DBG("Sink2 vol set(DACX)\n");
		break;
          #endif
		      default:
		         break;
	    }
	}
}

//���Menu ��ʱ����ʱ
void AudioPlayerMenuCheck(void)
{
	if(gIsVolSetEnable && IsTimeOut(&MenuTimer))
	{
		gIsVolSetEnable = FALSE;
		SetChannel = 0xff;
		if(Cnt > 0)
		{
			Cnt--;
		}
		else
		{
			Cnt = AUDIO_CORE_SOURCE_MAX_NUM + AUDIO_CORE_SINK_MAX_NUM;
		}
		APP_DBG("Menu Timer OUT\n");
	}
}

uint8_t AudioMusicVolGet(void)
{
	return mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM];
}

void AudioMusicVolUp(void)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}
	
#if (defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT))
	if(GetSystemMode() == ModeBtHfPlay)
	{
		if(mainAppCt.HfVolume < CFG_PARA_MAX_VOLUME_NUM)
		{
			mainAppCt.HfVolume++;
			#ifdef CFG_FUNC_BREAKPOINT_EN
			BackupInfoUpdata(BACKUP_SYS_INFO);
			#endif
		}
	    mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = mainAppCt.HfVolume;
	}
	else
#endif
	{
		if(MusicVolume < CFG_PARA_MAX_VOLUME_NUM)
		{
			MusicVolume++;
			#ifdef CFG_FUNC_BREAKPOINT_EN
			BackupInfoUpdata(BACKUP_SYS_INFO);
			#endif
		}else
		{
#ifdef CFG_FUNC_REMIND_SOUND_EN
//			RemindSoundServiceItemRequest(SOUND_REMIND_VOLMAX, REMIND_SOUND_INTTERRUPT_PLAY);
#endif
		}
	    mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = MusicVolume;
	}
	
	APP_DBG("APP_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(APP_SOURCE_NUM);

#if 0//def CFG_FUNC_REMIND_SOUND_EN
	#if CFG_PARAM_FIXED_REMIND_VOL
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = CFG_PARAM_FIXED_REMIND_VOL;
	#else
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = mainAppCt.MusicVolume;
	#endif
	APP_DBG("REMIND_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(REMIND_SOURCE_NUM);
#endif
#ifdef CFG_FUNC_RECORDER_EN
	mainAppCt.gSysVol.AudioSourceVol[PLAYBACK_SOURCE_NUM] = mainAppCt.MusicVolume;
	APP_DBG("PLAYBACK_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[PLAYBACK_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(PLAYBACK_SOURCE_NUM);
#endif

#ifdef CFG_APP_BT_MODE_EN
#if (BT_AVRCP_VOLUME_SYNC)
	//add volume sync(bluetooth play mode)
	if(GetSystemMode() == ModeBtAudioPlay)
	{
		MessageContext		msgSend;

		SetBtSyncVolume(mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);

		msgSend.msgId		= MSG_BT_PLAY_VOLUME_SET;
		MessageSend(GetSysModeMsgHandle(), &msgSend);
	}
#endif

#if (BT_HFP_SUPPORT)
	if(GetSystemMode() == ModeBtHfPlay)
	{
		SetBtHfSyncVolume(mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	}
#endif
#endif
#ifdef CFG_APP_HDMIIN_MODE_EN
	if(GetSystemMode() == ModeHdmiAudioPlay)
	{
		gHdmiCt->hdmiActiveReportVolUpDownflag = 2;
	}
#endif
}

void AudioMusicVolDown(void)
{
	//if(mainAppCt.gSysVol.MuteFlag)
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}
	
#if (defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT))
	if(GetSystemMode() == ModeBtHfPlay)
	{
		if(mainAppCt.HfVolume > 0)
		{
			mainAppCt.HfVolume--;
			#ifdef CFG_FUNC_BREAKPOINT_EN
			BackupInfoUpdata(BACKUP_SYS_INFO);
			#endif
		}
		mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = mainAppCt.HfVolume;
	}
	else
#endif
	{
		if(MusicVolume > 0)
		{
			MusicVolume--;
			#ifdef CFG_FUNC_BREAKPOINT_EN
			BackupInfoUpdata(BACKUP_SYS_INFO);
			#endif
		}
	    mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = MusicVolume;
	}
	
	APP_DBG("APP_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(APP_SOURCE_NUM);

#if 0//def CFG_FUNC_REMIND_SOUND_EN
	#if CFG_PARAM_FIXED_REMIND_VOL
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = CFG_PARAM_FIXED_REMIND_VOL;
	#else
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = mainAppCt.MusicVolume;
	#endif
	APP_DBG("REMIND_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(REMIND_SOURCE_NUM);
#endif
#ifdef CFG_FUNC_RECORDER_EN
	mainAppCt.gSysVol.AudioSourceVol[PLAYBACK_SOURCE_NUM] = mainAppCt.MusicVolume;
	APP_DBG("PLAYBACK_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[PLAYBACK_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(PLAYBACK_SOURCE_NUM);
#endif

#ifdef CFG_APP_BT_MODE_EN
#if (BT_AVRCP_VOLUME_SYNC)
	//add volume sync(bluetooth play mode)
	if(GetSystemMode() == ModeBtAudioPlay)
	{
		MessageContext		msgSend;

		SetBtSyncVolume(mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);

		msgSend.msgId		= MSG_BT_PLAY_VOLUME_SET;
		MessageSend(GetSysModeMsgHandle(), &msgSend);
	}
#endif

#if (BT_HFP_SUPPORT)
	if(GetSystemMode() == ModeBtHfPlay)
	{
		SetBtHfSyncVolume(mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	}
#endif
#endif
#ifdef CFG_APP_HDMIIN_MODE_EN
	if(GetSystemMode() == ModeHdmiAudioPlay)
	{
		gHdmiCt->hdmiActiveReportVolUpDownflag = 2;
	}
#endif
}

void AudioMusicVolSet(uint8_t musicVol)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}

	if(musicVol > CFG_PARA_MAX_VOLUME_NUM)
		MusicVolume = CFG_PARA_MAX_VOLUME_NUM;
	else
		MusicVolume = musicVol;
	mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = MusicVolume;
#ifdef CFG_FUNC_REMIND_SOUND_EN
	#if CFG_PARAM_FIXED_REMIND_VOL
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = CFG_PARAM_FIXED_REMIND_VOL;
	#else
	mainAppCt.gSysVol.AudioSourceVol[REMIND_SOURCE_NUM] = mainAppCt.MusicVolume;
	#endif
#endif
//	APP_DBG("APP_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);

	AudioEffect_SourceGain_Update(APP_SOURCE_NUM);
}

#ifdef CFG_APP_BT_MODE_EN
void AudioHfVolSet(uint8_t HfVol)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}
	
	if(HfVol > CFG_PARA_MAX_VOLUME_NUM)
		mainAppCt.HfVolume = CFG_PARA_MAX_VOLUME_NUM;
	else
		mainAppCt.HfVolume = HfVol;
	mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = mainAppCt.HfVolume;
	
	APP_DBG("source1 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(APP_SOURCE_NUM);
}
#endif

#if CFG_RES_MIC_SELECT
void AudioMicVolUp(void)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}

	if(MicVolume < CFG_PARA_MAX_VOLUME_NUM)
	{
		MicVolume++;
		#ifdef CFG_FUNC_BREAKPOINT_EN
		BackupInfoUpdata(BACKUP_SYS_INFO);
		#endif
	}
    mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM] = MicVolume;
	APP_DBG("MIC_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(MIC_SOURCE_NUM);
}

void AudioMicVolDown(void)
{
	if(IsAudioPlayerMute() == TRUE)
	{
		HardWareMuteOrUnMute();
	}

	if(MicVolume > 0)
	{
		MicVolume--;
		#ifdef CFG_FUNC_BREAKPOINT_EN
		BackupInfoUpdata(BACKUP_SYS_INFO);
		#endif
	}
    mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM] = MicVolume;
	APP_DBG("MIC_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(MIC_SOURCE_NUM);
}

#ifdef CFG_RONGYUAN_CMD
uint8_t AudioMicVolGet(void)
{
	return mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM];
}

void AudioMicVolSet(uint8_t micVol)
{
	if(micVol > CFG_PARA_MAX_VOLUME_NUM)
		MicVolume = CFG_PARA_MAX_VOLUME_NUM;
	else
		MicVolume = micVol;

    mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM] = MicVolume;
	APP_DBG("MIC_SOURCE_NUM vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM]);
	AudioEffect_SourceGain_Update(MIC_SOURCE_NUM);
}


#endif

#endif

void SystemVolSet(void)
{
	uint32_t i;

	for(i=0; i<AUDIO_CORE_SOURCE_MAX_NUM; i++)
	{
		AudioEffect_SourceGain_Update(i);
	}
#ifdef CFG_FUNC_SHUNNING_EN
	mainAppCt.aux_out_dyn_gain = mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM];
#endif
}

void SystemVolSync(void)
{
	mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM] = MusicVolume;
	mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM] = MicVolume;
}

#ifdef CFG_ADC_LEVEL_KEY_EN	
/*
****************************************************************
* ��λ����Ϣ���մ�����
*˵��:
*    AdcLevelCh��app_config.h��ѡ���GPIO�����¶�Ӧ��ϵ:

       #define  ADCLEVL_CHANNEL_MAP            (ADC_GPIOA20|ADC_GPIOA21|ADC_GPIOA22)
       
       ADC_GPIOA20��ӦAdcLevelChΪ1��
       ADC_GPIOA21��ӦAdcLevelChΪ2��
       ADC_GPIOA22��ӦAdcLevelChΪ3��
****************************************************************
*/
void AdcLevelMsgProcess(uint16_t Msg)//Sliding resistance
{
	uint16_t AdcLevelCh, AdcValue;

    if( (Msg > MSG_ADC_LEVEL_MSG_START)&&(Msg < MSG_ADC_LEVEL_MSG_END) )
	{				
		AdcLevelCh      =   Msg&0xff00;
		AdcLevelCh      -=  MSG_ADC_LEVEL_MSG_START;
		AdcLevelCh      >>= 8;

		AdcValue      =   Msg &0x00ff;
        //APP_DBG("AdcLevelCh = %d\n", AdcLevelCh);
		//APP_DBG("AdcValue = %d\n",AdcValue);

		switch(AdcLevelCh)
		{

			case 1://ADC LEVEL Channel 1
			    #if CFG_RES_MIC_SELECT
				mainAppCt.MicVolumeBak = AdcValue;
				APP_DBG("MicVolumeBak = %d\n", mainAppCt.MicVolumeBak);
			    //mainAppCt.gSysVol.AudioSourceVol[0] = mainAppCt.MicVolume = AdcValue;
				//APP_DBG("source0 vol = %d\n", mainAppCt.gSysVol.AudioSourceVol[0]);
				//AudioEffect_SourceGain_Update(0);
				#endif
				break;
				
			case 2://ADC LEVEL Channel 2
				break;
				
			case 3://ADC LEVEL Channel 3
//				mainAppCt.MicBassStepBak = 15 - AdcValue/2;
//				mainAppCt.MicTrebStepBak = AdcValue/2;
				//MicBassTrebAjust(mainAppCt.MicBassStep, mainAppCt.MicTrebStep);
				break;

			case 4://ADC LEVEL Channel 4
				
				break;
			case 5://ADC LEVEL Channel 5
				
				break;
			case 6://ADC LEVEL Channel 6
				
				break;
			case 7://ADC LEVEL Channel 7
				
				break;
			case 8://ADC LEVEL Channel 8
				
				break;
			case 9://ADC LEVEL Channel 9
				
				break;
			case 10://ADC LEVEL Channel 10
				
				break;
			case 11://ADC LEVEL Channel 11
				
				break;
		}
	}
}
#endif

#ifdef  CFG_APP_HDMIIN_MODE_EN
void HDMISourceMute(void)
{
	mainAppCt.hdmiSourceMuteFlg = 1;
	AudioCoreSourceMute(APP_SOURCE_NUM, TRUE, TRUE);
	//APP_DBG("hdmi mute\n");
}

void HDMISourceUnmute(void)
{
	mainAppCt.hdmiSourceMuteFlg = 0;
	AudioCoreSourceUnmute(APP_SOURCE_NUM, TRUE, TRUE);
	//APP_DBG("hdmi unmute\n");
}

bool IsHDMISourceMute(void)
{
	return mainAppCt.hdmiSourceMuteFlg;
}
#endif
