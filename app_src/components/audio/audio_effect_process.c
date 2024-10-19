#include <string.h>
#include <nds32_intrinsic.h>
#include "math.h"
#include "debug.h"
#include "app_config.h"
#include "bt_config.h"
#include "rtos_api.h"
#include "audio_effect_library.h"
#include "audio_core_api.h"
#include "main_task.h"
#include "audio_effect.h"
#include "remind_sound.h"
#include "ctrlvars.h"
#include "bt_manager.h"
#include "mode_task.h"
#include "bt_hf_api.h"
#include "user_effect_parameter.h"

#ifdef CFG_FUNC_EQMODE_FADIN_FADOUT_EN
extern uint32_t music_eq_mode_unit;
#endif

AudioEffectContext 	AudioEffect[MaxNet];

#ifdef CFG_FUNC_AUDIO_EFFECT_EN
__attribute__((optimize("Og")))
void AudioMusicProcess(AudioCoreContext *pAudioCore)
{
	int16_t  s;
	uint16_t n = AudioCoreFrameSizeGet(AudioCore.CurrentMix);
//	PCM_DATA_TYPE *monitor_out	= NULL;
	PCM_DATA_TYPE *mic_in	= NULL;
#ifdef CFG_APP_USB_AUDIO_MODE_EN
	PCM_DATA_TYPE *usb_out	= NULL;
#endif

//	for(s = 0; s < AUDIO_CORE_SOURCE_MAX_NUM; s++)
//	{
//		if(AudioCoreSourceToRoboeffect(s) != AUDIOCORE_SOURCE_SINK_ERROR)
//		{
//			if(!pAudioCore->AudioSource[s].Active || mainAppCt.gSysVol.MuteFlag || pAudioCore->AudioSource[s].LeftMuteFlag)
//			{
//				memset(roboeffect_get_source_buffer(AudioEffect[DefaultNet].context_memory, AudioCoreSourceToRoboeffect(s)),
//									0, roboeffect_get_buffer_size(AudioEffect[DefaultNet].context_memory));
//			}
//		}
//	}

#ifdef CFG_APP_USB_AUDIO_MODE_EN
	if(pAudioCore->AudioSink[USB_AUDIO_SINK_NUM].Active == TRUE)
	{
		usb_out = pAudioCore->AudioSink[USB_AUDIO_SINK_NUM].PcmOutBuf;
	}
#endif

	if(pAudioCore->AudioSource[MIC_SOURCE_NUM].Active)
	{
		mic_in = pAudioCore->AudioSource[MIC_SOURCE_NUM].PcmInBuf;
		if(mic_in)
		{
			for(s = n-1; s >= 0; s--)
			{
				mic_in[s*2 + 0] = mic_in[s];
				mic_in[s*2 + 1] = mic_in[s];
			}
		}
	}

	roboeffect_apply(AudioEffect[DefaultNet].context_memory);

#ifdef CFG_APP_USB_AUDIO_MODE_EN
	if(usb_out)
	{
		if(pAudioCore->AudioSink[USB_AUDIO_SINK_NUM].Channels == 1)
		{
			for(s = 0; s < n; s++)
			{
#ifdef CFG_AUDIO_WIDTH_24BIT
				usb_out[s] = __nds32__clips(((int32_t)usb_out[2*s] + (int32_t)usb_out[2*s + 1] +1 ) >>1, 24-1);
#else
				usb_out[s] = __nds32__clips(((int32_t)usb_out[2*s] + (int32_t)usb_out[2*s + 1] +1 ) >>1, 16-1);
#endif
			}
		}
	}
#endif
}


void AudioUsbAECProcess(AudioCoreContext *pAudioCore)
{
	MIX_NET net;
	int16_t  s;
	uint16_t n = AudioCoreFrameSizeGet(AudioCore.CurrentMix);
//	PCM_DATA_TYPE *monitor_out	= NULL;
	PCM_DATA_TYPE *mic_in	= NULL;
#ifdef CFG_APP_USB_AUDIO_MODE_EN
	PCM_DATA_TYPE *usb_out	= NULL;
#endif

//#ifdef CFG_APP_USB_AUDIO_MODE_EN
//	if(pAudioCore->AudioSink[USB_AUDIO_SINK_NUM].Active == TRUE)
//	{
//		usb_out = pAudioCore->AudioSink[USB_AUDIO_SINK_NUM].PcmOutBuf;
//	}
//#endif

	for(net = DefaultNet; net < MaxNet; net++)
	{
		if(NET_BIT_GET(AudioCore.NetReady, net))
		{
			roboeffect_apply(AudioEffect[net].context_memory);
		}
	}

//#ifdef CFG_APP_USB_AUDIO_MODE_EN
//	if(usb_out)
//	{
//		if(pAudioCore->AudioSink[USB_AUDIO_SINK_NUM].Channels == 1)
//		{
//			for(s = 0; s < n; s++)
//			{
//				usb_out[s] = __nds32__clips(((int32_t)usb_out[2*s] + (int32_t)usb_out[2*s + 1] +1 ) >>1, 24-1);
//			}
//		}
//	}
//#endif
}

void AudioEffectProcessBTHF(AudioCoreContext *pAudioCore)
{
	int8_t  net;
	int16_t  s;
	uint16_t n = AudioCoreFrameSizeGet(AudioCore.CurrentMix);
//	PCM_DATA_TYPE *monitor_out	= NULL;
	PCM_DATA_TYPE *mic_in	= NULL;
#ifdef CFG_APP_USB_AUDIO_MODE_EN
	PCM_DATA_TYPE *usb_out	= NULL;
#endif

	roboeffect_apply(AudioEffect[DefaultNet].context_memory);
}
#endif
/*
****************************************************************
* ����Ч��Ƶ����������
*
****************************************************************
*/
void AudioBypassProcess(AudioCoreContext *pAudioCore)
{
	int16_t  s;
	uint16_t n = AudioCoreFrameSizeGet(AudioCore.CurrentMix);
	PCM_DATA_TYPE *mic_pcm    	= NULL;//pBuf->mic_in;///mic input
	PCM_DATA_TYPE *monitor_out  = NULL;//pBuf->dac0_out;
	PCM_DATA_TYPE *music_pcm    = NULL;//pBuf->app_in;
#ifdef CFG_APP_USB_AUDIO_MODE_EN
	PCM_DATA_TYPE *usb_out  	= NULL;//pBuf->app_out;
#endif
#ifdef CFG_FUNC_REMIND_SOUND_EN
	PCM_DATA_TYPE *remind_in 	= NULL;
#endif
	PCM_DATA_TYPE *i2s_out      = NULL;//pBuf->i2s0_out;

	for(s = 0; s < AUDIO_CORE_SOURCE_MAX_NUM; s++)
	{
		if(AudioCoreSourceToRoboeffect(s) != AUDIOCORE_SOURCE_SINK_ERROR)
		{
			if(!pAudioCore->AudioSource[s].Active || mainAppCt.gSysVol.MuteFlag || pAudioCore->AudioSource[s].LeftMuteFlag)
			{
				memset(roboeffect_get_source_buffer(AudioEffect[AudioCore.AudioSource[s].Net].context_memory, AudioCoreSourceToRoboeffect(s)),
									0, roboeffect_get_buffer_size(AudioEffect[AudioCore.AudioSource[s].Net].context_memory));
			}
		}
	}

#ifdef CFG_RES_AUDIO_DAC0_EN
	if(pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].Active == TRUE)   ////dac0 buff
	{
		monitor_out = pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf;
	}
#endif
#ifdef CFG_RES_AUDIO_I2SOUT_EN
	if(pAudioCore->AudioSink[AUDIO_STEREO_SINK_NUM].Active == TRUE)
	{
		i2s_out = pAudioCore->AudioSink[AUDIO_STEREO_SINK_NUM].PcmOutBuf;
	}
#endif
#ifdef CFG_APP_USB_AUDIO_MODE_EN
	if(pAudioCore->AudioSink[USB_AUDIO_SINK_NUM].Active == TRUE)
	{
		usb_out = pAudioCore->AudioSink[USB_AUDIO_SINK_NUM].PcmOutBuf;
	}
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
	remind_in = pAudioCore->AudioSource[REMIND_SOURCE_NUM].PcmInBuf;
#endif
	music_pcm = pAudioCore->AudioSource[APP_SOURCE_NUM].PcmInBuf;
	mic_pcm = pAudioCore->AudioSource[MIC_SOURCE_NUM].PcmInBuf;

	if(mic_pcm)
	{
		for(s = n-1; s >= 0; s--)
		{
			mic_pcm[2 * s + 0] = mic_pcm[s];
			mic_pcm[2 * s + 1] = mic_pcm[s];
		}
//			AudioCoreAppSourceVolApply(MIC_SOURCE_NUM, (int16_t *)mic_pcm, n, 2);
	}
	if(monitor_out || i2s_out)
	{
		if(monitor_out == NULL)
			monitor_out = i2s_out;
	#ifdef CFG_FUNC_REMIND_SOUND_EN
		if(pAudioCore->AudioSource[REMIND_SOURCE_NUM].Active)
		{
			for(s = 0; s < n; s++)
			{
				monitor_out[2 * s + 0] = remind_in[2 * s + 0];
				monitor_out[2 * s + 1] = remind_in[2 * s + 1];
			}
		}
		else
	#endif
		{
			if(music_pcm)
			{
				if(mic_pcm)
				{
					for(s = 0; s < n; s++)
					{
	#ifdef CFG_AUDIO_WIDTH_24BIT
						monitor_out[2 * s + 0] = __nds32__clips(((int32_t)music_pcm[2 * s + 0] + (int32_t)mic_pcm[2 * s + 0] +1 ) >>1, 24-1);
						monitor_out[2 * s + 1] = __nds32__clips(((int32_t)music_pcm[2 * s + 1] + (int32_t)mic_pcm[2 * s + 1] +1 ) >>1, 24-1);
	#else
						monitor_out[2 * s + 0] = __nds32__clips(((int32_t)music_pcm[2 * s + 0] + (int32_t)mic_pcm[2 * s + 0] +1 ) >>1, 16-1);
						monitor_out[2 * s + 1] = __nds32__clips(((int32_t)music_pcm[2 * s + 1] + (int32_t)mic_pcm[2 * s + 1] +1 ) >>1, 16-1);
	#endif
					}
				}
				else
				{
					for(s = 0; s < n; s++)
					{
						monitor_out[2 * s + 0] = music_pcm[2 * s + 0];
						monitor_out[2 * s + 1] = music_pcm[2 * s + 1];
					}
				}
			}
		}

		if(i2s_out && i2s_out != monitor_out)
		{
			for(s = 0; s < n; s++)
			{
				i2s_out[2*s + 0] = monitor_out[2*s + 0];
				i2s_out[2*s + 1] = monitor_out[2*s + 1];
			}
		}
	}

#ifdef CFG_APP_USB_AUDIO_MODE_EN
	if(usb_out)
	{
		if(pAudioCore->AudioSink[USB_AUDIO_SINK_NUM].Channels == 1)
		{
			for(s = 0; s < n; s++)
			{
#ifdef CFG_AUDIO_WIDTH_24BIT
				usb_out[s] = __nds32__clips(((int32_t)mic_pcm[2 * s] + (int32_t)mic_pcm[2 * s + 1] +1 ) >>1, 24-1);
#else
				usb_out[s] = __nds32__clips(((int32_t)mic_pcm[2 * s] + (int32_t)mic_pcm[2 * s + 1] +1 ) >>1, 16-1);
#endif
			}
		}
		else
		{
			for(s = 0; s < n; s++)
			{
				usb_out[2 * s + 0] = mic_pcm[s];
				usb_out[2 * s + 1] = mic_pcm[s];
			}
		}
	}
#endif

}

//��Appͨ·��Ƶ����������
void AudioNoAppProcess(AudioCoreContext *pAudioCore)
{
	int16_t  s;
	uint16_t n = AudioCoreFrameSizeGet(AudioCore.CurrentMix);
	PCM_DATA_TYPE *mic_pcm    	= NULL;//pBuf->mic_in;///mic input
	PCM_DATA_TYPE *monitor_out    = NULL;//pBuf->dac0_out;
#ifdef	CFG_FUNC_REMIND_SOUND_EN
	PCM_DATA_TYPE *remind_in = NULL;
#endif
	PCM_DATA_TYPE *i2s_out       = NULL;//pBuf->i2s0_out;

	if(pAudioCore->AudioSource[MIC_SOURCE_NUM].Active == TRUE)////mic buff
	{
		mic_pcm = pAudioCore->AudioSource[MIC_SOURCE_NUM].PcmInBuf;//˫mic����
	}

    if(pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].Active == TRUE)   ////dac0 buff
	{
    	monitor_out = pAudioCore->AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf;
	}

#if defined(CFG_FUNC_REMIND_SOUND_EN)
	if(pAudioCore->AudioSource[REMIND_SOURCE_NUM].Active == TRUE)////remind buff
	{
		remind_in = pAudioCore->AudioSource[REMIND_SOURCE_NUM].PcmInBuf;
	}
#endif
#ifdef CFG_RES_AUDIO_I2SOUT_EN
	if(pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].Active == TRUE)	////i2s buff
	{
		i2s_out = pAudioCore->AudioSink[AUDIO_I2SOUT_SINK_NUM].PcmOutBuf;
	}
#endif


    if(monitor_out)
	{
		memset(monitor_out, 0, roboeffect_get_buffer_size(AudioEffect[DefaultNet].context_memory));
    }
    if(i2s_out)
	{
		memset(i2s_out, 0, roboeffect_get_buffer_size(AudioEffect[DefaultNet].context_memory));
    }

	//DAC0������������Ч����
	if(monitor_out || i2s_out)
	{
		if(monitor_out == NULL)
			monitor_out = i2s_out;
		for(s = 0; s < n; s++)
		{
		#if defined(CFG_FUNC_REMIND_SOUND_EN)
			if(remind_in)
			{
				monitor_out[2*s + 0] = remind_in[2*s + 0];
				monitor_out[2*s + 1] = remind_in[2*s + 1];
			}
			else
		#endif
			if(mic_pcm)
			{
				monitor_out[2*s + 0] = mic_pcm[2*s + 0];
				monitor_out[2*s + 1] = mic_pcm[2*s + 1];
			}
		}

		if(i2s_out && i2s_out != monitor_out)
		{
			for(s = 0; s < n; s++)
			{
				i2s_out[2*s + 0] = monitor_out[2*s + 0];
				i2s_out[2*s + 1] = monitor_out[2*s + 1];
			}
		}		
	}

}

