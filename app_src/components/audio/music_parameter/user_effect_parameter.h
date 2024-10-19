/**
 *************************************************************************************
 * @file	user_effect_parameter.h
 * @brief	Audio effect control interface provided by SDK
 *
 * &copy; Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *************************************************************************************
 */

#ifndef _USER_EFFECT_PARAMETER_H_
#define _USER_EFFECT_PARAMETER_H_
#include "audio_core_api.h"
#include "ctrlvars.h"
#include "eq.h"
#include "reverb.h"
#include "reverb_plate.h"
#include "reverb_pro.h"

#define AUDIOCORE_SOURCE_SINK_ERROR 0xFF

/**
 * @brief Audio effect adjust
 */
typedef enum _AUDIOEFFECT_EFFECT_CONTROL
{
	EQ_MODE_ADJUST = 0,
	MIC_BASS_ADJUST,
	MIC_TREB_ADJUST,
	MUSIC_BASS_ADJUST,
	MUSIC_TREB_ADJUST,
	MIC_VOLUME_ADJUST,
	MUSIC_VOLUME_ADJUST,
	ECHO_PARAM,
	MIC_SILENCE_DETECTOR_PARAM,
	MUSIC_SILENCE_DETECTOR_PARAM,
	_3D_ENABLE,
	VIRTURAL_71CHN_ENABLE,
	MIC_AEC_ENABLE,
	EARPHONE_EQ_ADJUST,
	AUDIOEFFECT_EFFECT_CONTROL_MAX
}AUDIOEFFECT_EFFECT_CONTROL;

/**
 * @brief Audio effect source/sink num
 */
typedef struct _AUDIOEFFECT_SOURCE_SINK_NUM
{
	uint8_t		mic_source;			/**< MIC_SOURCE_NUM                          */
	uint8_t		app_source;			/**< APP_SOURCE_NUM                          */
	uint8_t		remind_source;		/**< REMIND_SOURCE_NUM                    	 */
	uint8_t		rec_source;			/**< PLAYBACK_SOURCE_NUM                     */
	uint8_t		usb_source;			/**< USB_SOURCE                              */
	uint8_t		i2s_mix_source;		/**< I2S_MIX_SOURCE                          */
	uint8_t		i2s_mix2_source;	/**< I2S_MIX2_SOURCE                         */
	uint8_t		linein_mix_source;	/**< LINEIN_MIX_SOURCE_NUM                   */
	uint8_t		aec_ref_source;		/**< AEC_REF_SOURCE_NUM						 */

	uint8_t		dac0_sink;			/**< AUDIO_DAC0_SINK_NUM                     */
	uint8_t		app_sink;			/**< AUDIO_APP_SINK_NUM                      */
	uint8_t		stereo_sink;		/**< AUDIO_STEREO_SINK_NUM                   */
	uint8_t		rec_sink;			/**< AUDIO_RECORDER_SINK_NUM                 */
	uint8_t		i2s_mix_sink;		/**< AUDIO_I2S_MIX_OUT_SINK_NUM              */
	uint8_t		spdif_sink;			/**< AUDIO_SPDIF_SINK_NUM                    */
	uint8_t		aec_ref_sink;		/**< AEC_REF_SINK_NUM						 */
} AUDIOEFFECT_SOURCE_SINK_NUM;

//Ĭ������Ϊ��ʹ��,effect_mode��ҪĬ�ϼ��ڿ�ʼ��λ��
#define AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT	.mic_source = AUDIOCORE_SOURCE_SINK_ERROR,\
												.app_source = AUDIOCORE_SOURCE_SINK_ERROR,\
												.remind_source = AUDIOCORE_SOURCE_SINK_ERROR,\
												.rec_source = AUDIOCORE_SOURCE_SINK_ERROR,\
												.usb_source = AUDIOCORE_SOURCE_SINK_ERROR,\
												.i2s_mix_source = AUDIOCORE_SOURCE_SINK_ERROR,\
												.i2s_mix2_source = AUDIOCORE_SOURCE_SINK_ERROR,\
												.linein_mix_source = AUDIOCORE_SOURCE_SINK_ERROR,\
												.aec_ref_source = AUDIOCORE_SOURCE_SINK_ERROR,\
												.dac0_sink = AUDIOCORE_SOURCE_SINK_ERROR,\
												.app_sink = AUDIOCORE_SOURCE_SINK_ERROR,\
												.stereo_sink = AUDIOCORE_SOURCE_SINK_ERROR,\
												.rec_sink = AUDIOCORE_SOURCE_SINK_ERROR,\
												.i2s_mix_sink = AUDIOCORE_SOURCE_SINK_ERROR,\
												.spdif_sink = AUDIOCORE_SOURCE_SINK_ERROR,\
												.aec_ref_sink = AUDIOCORE_SOURCE_SINK_ERROR

#define		AUDIOEFFECT_SET_EFFECTADDR(net, rAddr)	((uint16_t)((net << 8) + rAddr))
#define 	AUDIOEFFECT_GET_NET(vAddr)         		((uint8_t)(((vAddr) >> 8) & 0xFF))
#define 	AUDIOEFFECT_GET_REALADDR(vAddr)    		((uint8_t)(((vAddr << 8) >> 8) & 0xFF))


/**
 * @brief Echo unit
 */
typedef struct __EchoUnit
{
	int16_t		fc;
	int16_t		attenuation;
	int16_t		delay;
	int16_t		max_delay;
	int16_t		quality_mode;//0,1=normal echo, 0,2=24bit echo
	int16_t		dry;
	int16_t		wet;
} EchoUnit;

/**
 * @brief SilenceDetector unit
 */
typedef struct __SilenceDetectorUnit
{
	uint16_t	level;
} SilenceDetectorUnit;

/**
 * @brief GainControl unit
 */
typedef struct __GainControlUnit
{
	uint16_t	mute;
	uint16_t	gain;
} GainControlUnit;

/**
 * @brief  Get some default parameters
 * @param  void
 * @return None
 */
void AudioEffect_GetAudioEffectValue(void);

/**
 * @brief  Set source gain
 * @param  source : AudioCore source enum less than AUDIO_CORE_SOURCE_MAX_NUM
 * @return None
 */
void AudioEffect_SourceGain_Update(uint8_t source);

/**
 * @brief  Sync some effect parameters when change effect mode
 * @param  void
 * @return None
 */
void AudioEffectParamSync(void);

/**
 * @brief  Get effect status
 * @param  targetNet : ennum MIX_NET
 * @param  effect_addr : expected audio effect addr
 * @return uint8_t : 0 or 1
 */
uint8_t AudioEffect_effect_status_Get(MIX_NET targetNet, uint8_t effect_addr);

/**
 * @brief  Onoff effect
 * @param  effect_addr : expected audio effect addr
 * @param  enable : 0 or 1
 * @return None
 */
void AudioEffect_effect_enable(uint8_t effect_addr, uint8_t enable);

/**
 * @brief  AudioCore source link to Roboeffect source
 * @param  source : AudioCore source enum less than AUDIO_CORE_SOURCE_MAX_NUM
 * @return uint8_t : Roboeffect source index
 */
uint8_t AudioCoreSourceToRoboeffect(int8_t source);

/**
 * @brief  AudioCore sink link to Roboeffect sink
 * @param  sink : AudioCore sink enum less than AUDIO_CORE_SINK_MAX_NUM
 * @return uint8_t : Roboeffect sink index
 */
uint8_t AudioCoreSinkToRoboeffect(int8_t sink);

/**
 * @brief  Update an audio effect parameter of SDK local backup
 * @param  targetNet : ennum MIX_NET
 * @param  addr : expected audio effect addr
 * @param  param_index : parameter index in an audio effect parameters
 * @param  param_input : parameter
 * @param  param_len : parameter length
 * @return None
 */
void AudioEffect_update_local_params(MIX_NET targetNet, uint8_t addr, uint8_t param_index, int16_t *param_input, uint8_t param_len);

/**
 * @brief  Update audio effect status of SDK local backup
 * @param  targetNet : ennum MIX_NET
 * @param  addr : expected audio effect addr
 * @param  effect_enable : 0 or 1
 * @return None
 */
void AudioEffect_update_local_effect_status(MIX_NET targetNet, uint8_t addr, uint8_t effect_enable);

/**
 * @brief  Update a block effect parameters of SDK local backup
 * @param  targetNet : ennum MIX_NET
 * @param  addr : expected audio effect addr
 * @return None
 */
void AudioEffect_update_local_block_params(MIX_NET targetNet, uint8_t addr);

/**
 * @brief  Get user effect parameters length
 * @param  user_effect_parameters : effect parameters data
 * @return uint16_t : parameters length in Bytes
 */
uint16_t get_user_effect_parameters_len(uint8_t *user_effect_parameters);

/**
 * @brief  Get user effect parameters for roboeffect engine
 * @param  mode : effect mode in enum EFFECT_MODE
 * @return AUDIOEFFECT_EFFECT_PARA : roboeffect parameters
 */
AUDIOEFFECT_EFFECT_PARA * get_user_effect_parameters(uint8_t mode);

/**
 * @brief  judge if there is only one flowchart for the effect mode
 * @param  mode : effect mode in enum EFFECT_MODE
 * @return bool : TRUE or FLASE
 */
bool isCurEffectModeSingleFlowChart(EFFECT_MODE effectMode);

/**
 * @brief  Save effect parameters to flash
 * @param  void
 * @return None
 */
void EffectParamFlashUpdata(void);

/**
 * @brief  Get effect hardware config from flash
 * @param  effectMode : effect mode in enum EFFECT_MODE
 * @param  hw_ct : effect hardware config data pointer
 * @return bool : TRUE or FALSE
 */
bool AudioEffect_GetFlashHwCfg(uint8_t effectMode, HardwareConfigContext *hw_ct);

/**
 * @brief  Get effect parameters from flash
 * @param  effectMode : effect mode in enum EFFECT_MODE
 * @param  targetNet : ennum MIX_NET
 * @param  hw_ct : effect parameters data pointer
 * @return bool : TRUE or FALSE
 */
bool AudioEffect_GetFlashEffectParam(uint8_t effectMode, MIX_NET targetNet, uint8_t *effect_param);

//ACPWorkbench V3.8.15�Ժ�汾�����Ĳ���������Roboeffect Version + 3rd part data
//ʹ�õ�ʱ��ע������İ汾���޸Ķ�Ӧ��ƫ��
typedef struct __EffectValidParamUnit
{
	uint8_t *	params_first_address;
	uint16_t 	data_len;
} EffectValidParamUnit;

EffectValidParamUnit AudioEffect_GetUserEffectValidParam(uint8_t *effect_param);


#endif
