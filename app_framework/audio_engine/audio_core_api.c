/**
 **************************************************************************************
 * @file    audio_core.c
 * @brief   audio core 
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-6-29 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <string.h>
#include <nds32_intrinsic.h>
#include "main_task.h"
#include "audio_core_service.h"
#include "ctrlvars.h"
#include "audio_effect.h"
#include "mcu_circular_buf.h"
#include "beep.h"
#include "dma.h"

#ifdef CFG_APP_BT_MODE_EN
#include "bt_config.h"
#include "bt_play_api.h"
#include "bt_manager.h"
#if BT_HFP_SUPPORT
#include "bt_hf_api.h"
#endif
#endif
#include "dac_interface.h"
#include "audio_vol.h"
#include "user_effect_parameter.h"

#ifdef CFG_APP_BT_MODE_EN
uint32_t gACBtMonitor = 0; //audio core���:����ģʽ�����ݲ��ռ�����
#endif
/*******************************************************************************************************************************
 *
 *				 |***GetAdapter***|	  			 |***********CoreProcess***********|			  |***SetAdapter***|
 * ************	 ******************	 **********	 ***********************************  **********  ******************  **********
 *	SourceFIFO*->*SRCFIFO**SRAFIFO*->*PCMFrame*->*PreGainEffect**MixNet**GainEffect*->*PCMFrame*->*SRAFIFO**SRCFIFO*->*SinkFIFO*
 * ************  ******************	 **********	 ***********************************  **********  ******************  **********
 * 				 |*Context*|																			 |*Context*|
 *
 *******************************************************************************************************************************/

typedef enum
{
	AC_RUN_CHECK,//���ڼ���Ƿ���Ҫ��ͣ���������Ҫ��ͣ������ͣ���ٸ�״̬
	AC_RUN_GET,
	AC_RUN_PROC,
	AC_RUN_PUT,
}AudioCoreRunState;

static AudioCoreRunState AudioState = AC_RUN_CHECK;
AudioCoreContext		AudioCore;

extern uint32_t gSysTick;

void AudioCoreSourcePcmFormatConfig(uint8_t Index, uint16_t Format)
{
	if(Index < AUDIO_CORE_SOURCE_MAX_NUM)
	{
		AudioCore.AudioSource[Index].Channels = Format;
	}
}

void AudioCoreSourceEnable(uint8_t Index)
{
	if(Index < AUDIO_CORE_SOURCE_MAX_NUM)
	{
		AudioCore.AudioSource[Index].Enable = TRUE;
	}
}

void AudioCoreSourceDisable(uint8_t Index)
{
	if(Index < AUDIO_CORE_SOURCE_MAX_NUM)
	{
		AudioCore.AudioSource[Index].Enable = FALSE;
	}
}

bool AudioCoreSourceIsEnable(uint8_t Index)
{
	return AudioCore.AudioSource[Index].Enable;
}

void AudioCoreSourceMute(uint8_t Index, bool IsLeftMute, bool IsRightMute)
{
	if(IsLeftMute)
	{
		AudioCore.AudioSource[Index].LeftMuteFlag = TRUE;
	}
	if(IsRightMute)
	{
		AudioCore.AudioSource[Index].RightMuteFlag = TRUE;
	}
}

void AudioCoreSourceUnmute(uint8_t Index, bool IsLeftUnmute, bool IsRightUnmute)
{
	if(IsLeftUnmute)
	{
		AudioCore.AudioSource[Index].LeftMuteFlag = FALSE;
	}
	if(IsRightUnmute)
	{
		AudioCore.AudioSource[Index].RightMuteFlag = FALSE;
	}
}

void AudioCoreSourceConfig(uint8_t Index, AudioCoreSource* Source)
{
	memcpy(&AudioCore.AudioSource[Index], Source, sizeof(AudioCoreSource));
}

void AudioCoreSinkEnable(uint8_t Index)
{
	AudioCore.AudioSink[Index].Enable = TRUE;
}

void AudioCoreSinkDisable(uint8_t Index)
{
	AudioCore.AudioSink[Index].Enable = FALSE;
}

bool AudioCoreSinkIsEnable(uint8_t Index)
{
	return AudioCore.AudioSink[Index].Enable;
}

void AudioCoreSinkConfig(uint8_t Index, AudioCoreSink* Sink)
{
	memcpy(&AudioCore.AudioSink[Index], Sink, sizeof(AudioCoreSink));
}


void AudioCoreProcessConfig(AudioCoreProcessFunc AudioEffectProcess)
{
	AudioCore.AudioEffectProcess = AudioEffectProcess;
}

///**
// * @func        AudioCoreConfig
// * @brief       AudioCore�����飬���ػ�API
// * @param       AudioCoreContext *AudioCoreCt
// * @Output      None
// * @return      None
// * @Others      �ⲿ���õĲ����飬����һ�ݵ�����
// */
//void AudioCoreConfig(AudioCoreContext *AudioCoreCt)
//{
//	memcpy(&AudioCore, AudioCoreCt, sizeof(AudioCoreContext));
//}

bool AudioCoreInit(void)
{
	DBG("AudioCore init\n");
	return TRUE;
}

void AudioCoreDeinit(void)
{
	AudioState = AC_RUN_CHECK;
}

/**
 * @func        AudioCoreRun
 * @brief       ��Դ����->��Ч����+����->����
 * @param       None
 * @Output      None
 * @return      None
 * @Others      ��ǰ��audioCoreservice�����ϴ˹�����Ч������
 * Record
 */
extern uint32_t 	IsAudioCorePause;
extern uint32_t 	IsAudioCorePauseMsgSend;
void AudioProcessMain(void);
__attribute__((optimize("Og")))
void AudioCoreRun(void)
{
	bool ret;
	switch(AudioState)
	{
		case AC_RUN_CHECK:
			if(IsAudioCorePause == TRUE)
			{
				if(IsAudioCorePauseMsgSend == TRUE)
				{
					MessageContext		msgSend;
					msgSend.msgId		= MSG_AUDIO_CORE_HOLD;
					MessageSend(GetAudioCoreServiceMsgHandle(), &msgSend);

					IsAudioCorePauseMsgSend = FALSE;
				}
				return;
			}
		case AC_RUN_GET:
			AudioCoreIOLenProcess();
#ifdef CFG_APP_BT_MODE_EN
			//ÿ��1ms��ѯһ��,app_source ���ݲ���,���еǼ�
			if(SOURCE_BIT_GET(AudioCore.FrameReady, APP_SOURCE_NUM) == 0)
			{
				extern uint32_t gBtPlaySbcDecoderInitFlag;
				if((GetSystemMode() == ModeBtAudioPlay)//����ģʽ
					&&((GetA2dpState(0) == BT_A2DP_STATE_STREAMING)||(GetA2dpState(1) == BT_A2DP_STATE_STREAMING))//���ڲ�������
					&&(gBtPlaySbcDecoderInitFlag)//decoder��ʼ���Ѿ����
					&&(AudioCoreSourceIsEnable(APP_SOURCE_NUM))//sourceͨ·�Ѿ�����
					)
				{
					if(++gACBtMonitor>=80)//80ms����,����decoder
					{
						gACBtMonitor = 0;
						a2dp_sbc_decoer_init();
					}
				}
				else
				{
					gACBtMonitor = 0;
				}
			}
#endif
			ret = AudioCoreSourceSync();
			if(ret == FALSE)
			{
				return;
			}

		case AC_RUN_PROC:
			AudioProcessMain();
			AudioState = AC_RUN_PUT;

		case AC_RUN_PUT:
			ret = AudioCoreSinkSync();
			if(ret == FALSE)
			{
//				AudioCoreIOLenProcess();
				return;
			}
			AudioState = AC_RUN_CHECK;
			break;
		default:
			break;
	}
}

//��Ч�������������
//��micͨ·���ݰ������ͳһ����
//micͨ·���ݺ;���ģʽ�޹�
//��ʾ��ͨ·����Ч���������sink�˻�����
void AudioProcessMain(void)
{
	AudioCoreSourceMuteApply();
#ifdef CFG_FUNC_RECORDER_EN
	if(AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].Enable == TRUE)
	{
		if(AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].Channels == 1)
		{
			uint16_t i;
			for(i = SOURCEFRAME(PLAYBACK_SOURCE_NUM) * 2 - 1; i > 0; i--)
			{
				AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].PcmInBuf[i] = AudioCore.AudioSource[PLAYBACK_SOURCE_NUM].PcmInBuf[i / 2];
			}
		}
	}
#endif
	if(AudioCore.AudioSource[APP_SOURCE_NUM].Active == TRUE)////music buff
	{
		#if (BT_HFP_SUPPORT) && defined(CFG_APP_BT_MODE_EN)
		if(GetSystemMode() != ModeBtHfPlay)
		#endif
		{
			if(AudioCore.AudioSource[APP_SOURCE_NUM].Channels == 1)
			{
				uint16_t i;
				for(i = SOURCEFRAME(APP_SOURCE_NUM) * 2 - 1; i > 0; i--)
				{
					AudioCore.AudioSource[APP_SOURCE_NUM].PcmInBuf[i] = AudioCore.AudioSource[APP_SOURCE_NUM].PcmInBuf[i / 2];
				}
			}
		}
	}	
		
#if defined(CFG_FUNC_REMIND_SOUND_EN)
	if(AudioCore.AudioSource[REMIND_SOURCE_NUM].Active == TRUE)////remind buff
	{
		if(AudioCore.AudioSource[REMIND_SOURCE_NUM].Channels == 1)
		{
			uint16_t i;
			for(i = SOURCEFRAME(REMIND_SOURCE_NUM) * 2 - 1; i > 0; i--)
			{
				AudioCore.AudioSource[REMIND_SOURCE_NUM].PcmInBuf[i] = AudioCore.AudioSource[REMIND_SOURCE_NUM].PcmInBuf[i / 2];
			}
		}
	}	
#endif


	if(AudioCore.AudioEffectProcess != NULL)
	{
		AudioCore.AudioEffectProcess((AudioCoreContext*)&AudioCore);
	}
	
#ifdef CFG_FUNC_BEEP_EN
    if(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].Active == TRUE)   ////dacx buff
	{
		Beep(AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf, SINKFRAME(AUDIO_DAC0_SINK_NUM));
	}
#endif
    AudioCoreSinkMuteApply();
}

//�������뵭��
#define MixerFadeVolume(a, b, c)  	\
    if(a > b + c)		    \
	{						\
		a -= c;				\
	}						\
	else if(a + c < b)	   	\
	{						\
		a += c;				\
	}						\
	else					\
	{						\
		a = b;				\
	}


void AudioCoreSourceMuteApply(void)
{
	uint32_t i;
	uint8_t j;
	uint16_t LeftVol, RightVol, TargetVol, LeftVolStep, RightVolStep;
	bool mute;

	for(j = 0; j < AUDIO_CORE_SOURCE_MAX_NUM; j++)
	{
		mute = AudioCore.AudioSource[j].LeftMuteFlag || AudioCore.AudioSource[j].RightMuteFlag || mainAppCt.gSysVol.MuteFlag;
		if((!AudioCore.AudioSource[j].Active) || (!AudioCore.AudioSource[j].Enable) || (mute == AudioCore.AudioSource[j].MuteFlagbk))
		{
			if(AudioCoreSourceToRoboeffect(j) != AUDIOCORE_SOURCE_SINK_ERROR)
			{
				if(!AudioCore.AudioSource[j].Active || !AudioCore.AudioSource[j].Enable
						|| mainAppCt.gSysVol.MuteFlag || AudioCore.AudioSource[j].LeftMuteFlag)
				{
					memset(roboeffect_get_source_buffer(AudioEffect[AudioCore.AudioSource[j].Net].context_memory, AudioCoreSourceToRoboeffect(j)),
										0, roboeffect_get_buffer_size(AudioEffect[AudioCore.AudioSource[j].Net].context_memory));
				}
			}
			AudioCore.AudioSource[j].MuteFlagbk = mute;
			continue;
		}

		TargetVol = mute ? 0:4096;
		LeftVol = 4096 - TargetVol;
		RightVol = LeftVol;
		LeftVolStep = 4096 / SOURCEFRAME(j) + (4096 % SOURCEFRAME(j) ? 1 : 0);
		RightVolStep = LeftVolStep;
#ifdef CFG_AUDIO_WIDTH_24BIT
		if(AudioCore.AudioSource[j].BitWidth == PCM_DATA_24BIT_WIDTH //24bit ����
		 || AudioCore.AudioSource[j].BitWidthConvFlag     //��AudioCoreSourceGet���䵽24bit
			)
		{
			if(AudioCore.AudioSource[j].Channels == 2)
			{
				for(i=0; i < SOURCEFRAME(j); i++)
				{
					AudioCore.AudioSource[j].PcmInBuf[2 * i + 0] = __nds32__clips((((int64_t)AudioCore.AudioSource[j].PcmInBuf[2 * i + 0]) * LeftVol + 2048) >> 12, (24)-1);
					AudioCore.AudioSource[j].PcmInBuf[2 * i + 1] = __nds32__clips((((int64_t)AudioCore.AudioSource[j].PcmInBuf[2 * i + 1]) * RightVol + 2048) >> 12, (24)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
					MixerFadeVolume(RightVol, TargetVol, RightVolStep);
				}
			}
			else if(AudioCore.AudioSource[j].Channels == 1)
			{
				for(i=0; i<SOURCEFRAME(j); i++)
				{
					AudioCore.AudioSource[j].PcmInBuf[i] = __nds32__clips((((int64_t)AudioCore.AudioSource[j].PcmInBuf[i]) * LeftVol + 2048) >> 12, (24)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
				}
			}
		}
		else
#endif
		{
			int16_t * PcmInBuf = (int16_t *)AudioCore.AudioSource[j].PcmInBuf;
			if(AudioCore.AudioSource[j].Channels == 2)
			{
				for(i=0; i < SOURCEFRAME(j); i++)
				{
					PcmInBuf[2 * i + 0] = __nds32__clips((((int32_t)PcmInBuf[2 * i + 0]) * LeftVol + 2048) >> 12, (16)-1);
					PcmInBuf[2 * i + 1] = __nds32__clips((((int32_t)PcmInBuf[2 * i + 1]) * RightVol + 2048) >> 12, (16)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
					MixerFadeVolume(RightVol, TargetVol, RightVolStep);
				}
			}
			else if(AudioCore.AudioSource[j].Channels == 1)
			{
				for(i=0; i<SOURCEFRAME(j); i++)
				{
					PcmInBuf[i] = __nds32__clips((((int32_t)PcmInBuf[i]) * LeftVol + 2048) >> 12, (16)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
				}
			}
		}
		AudioCore.AudioSource[j].MuteFlagbk = mute;
	}
}

void AudioCoreSinkMuteApply(void)
{
	uint32_t i;
	uint8_t j;
	uint16_t LeftVol, RightVol, TargetVol, LeftVolStep, RightVolStep;
	bool mute;

	for(j = 0; j < AUDIO_CORE_SINK_MAX_NUM; j++)
	{
		mute = AudioCore.AudioSink[j].LeftMuteFlag || AudioCore.AudioSink[j].RightMuteFlag || mainAppCt.gSysVol.MuteFlag;
		if((!AudioCore.AudioSink[j].Active) || (!AudioCore.AudioSink[j].Enable) || (mute == AudioCore.AudioSink[j].MuteFlagbk))
		{
			if(AudioCoreSinkToRoboeffect(j) != AUDIOCORE_SOURCE_SINK_ERROR)
			{
				if(!AudioCore.AudioSink[j].Active || !AudioCore.AudioSink[j].Enable
						|| mainAppCt.gSysVol.MuteFlag || AudioCore.AudioSink[j].LeftMuteFlag)
				{
					memset(roboeffect_get_sink_buffer(AudioEffect[AudioCore.AudioSink[j].Net].context_memory, AudioCoreSinkToRoboeffect(j)),
										0, roboeffect_get_buffer_size(AudioEffect[AudioCore.AudioSink[j].Net].context_memory));
				}
			}
			AudioCore.AudioSink[j].MuteFlagbk = mute;
			continue;
		}

		TargetVol = mute ? 0:4096;
		LeftVol = 4096 - TargetVol;
		RightVol = LeftVol;
		LeftVolStep = 4096 / SINKFRAME(j) + (4096 % SINKFRAME(j) ? 1 : 0);
		RightVolStep = LeftVolStep;

#ifdef CFG_AUDIO_WIDTH_24BIT
		if(AudioCore.AudioSink[j].BitWidth == PCM_DATA_24BIT_WIDTH //24bit ����
		 //|| AudioCore.AudioSink[j].BitWidthConvFlag     //��AudioCoreSinkSet���䵽24bit,��ʱ����16bit
			)
		{
			if(AudioCore.AudioSink[j].Channels == 2)
			{
				for(i=0; i < SINKFRAME(j); i++)
				{
					AudioCore.AudioSink[j].PcmOutBuf[2 * i + 0] = __nds32__clips((((int64_t)AudioCore.AudioSink[j].PcmOutBuf[2 * i + 0]) * LeftVol + 2048) >> 12, (24)-1);
					AudioCore.AudioSink[j].PcmOutBuf[2 * i + 1] = __nds32__clips((((int64_t)AudioCore.AudioSink[j].PcmOutBuf[2 * i + 1]) * RightVol + 2048) >> 12, (24)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
					MixerFadeVolume(RightVol, TargetVol, RightVolStep);
				}
			}
			else if(AudioCore.AudioSink[j].Channels == 1)
			{
				for(i=0; i<SINKFRAME(j); i++)
				{
					AudioCore.AudioSink[j].PcmOutBuf[i] = __nds32__clips((((int64_t)AudioCore.AudioSink[j].PcmOutBuf[i]) * LeftVol + 2048) >> 12, (24)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
				}
			}
		}
		else
#endif
		{
			int16_t * PcmOutBuf = (int16_t *)AudioCore.AudioSink[j].PcmOutBuf;
			if(AudioCore.AudioSink[j].Channels == 2)
			{
				for(i=0; i < SINKFRAME(j); i++)
				{
					PcmOutBuf[2 * i + 0] = __nds32__clips((((int32_t)PcmOutBuf[2 * i + 0]) * LeftVol + 2048) >> 12, (16)-1);
					PcmOutBuf[2 * i + 1] = __nds32__clips((((int32_t)PcmOutBuf[2 * i + 1]) * RightVol + 2048) >> 12, (16)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
					MixerFadeVolume(RightVol, TargetVol, RightVolStep);
				}
			}
			else if(AudioCore.AudioSink[j].Channels == 1)
			{
				for(i=0; i<SINKFRAME(j); i++)
				{
					PcmOutBuf[i] = __nds32__clips((((int32_t)PcmOutBuf[i]) * LeftVol + 2048) >> 12, (16)-1);

					MixerFadeVolume(LeftVol, TargetVol, LeftVolStep);
				}
			}
		}
		AudioCore.AudioSink[j].MuteFlagbk = mute;
	}
}
