#include "user_effect_flow_aecMusic.h"
#include "user_effect_flow_aecMic.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA aec_effect_para[MaxNet] =
{
	{
		.user_effect_name = (uint8_t *)"AEC_Music",
		.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_aecMusic,
		.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_aecMusic,
		.user_effects_script = (uint8_t *)user_effects_script_aecMusic,
		.user_effect_parameters = (uint8_t *)user_effect_parameters_aecMusic_AEC_Music,
		.user_module_parameters = (uint8_t *)user_module_parameters_aecMusic_AEC_Music,
		.get_user_effects_script_len = get_user_effects_script_len_aecMusic,
	},
	{
		.user_effect_name = (uint8_t *)"AEC_Mic",
		.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_aecMic,
		.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_aecMic,
		.user_effects_script = (uint8_t *)user_effects_script_aecMic,
		.user_effect_parameters = (uint8_t *)user_effect_parameters_aecMic_AEC_Mic,
		.user_module_parameters = (uint8_t *)user_module_parameters_aecMic_AEC_Mic,
		.get_user_effects_script_len = get_user_effects_script_len_aecMic,
	}
};

const AUDIOEFFECT_SOURCE_SINK_NUM aec_mode =
{
	//²»ÒªÉ¾³ý£¬source/sinkÄ¬ÈÏÖµ
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCEÓ³Éä
	.mic_source = AECMIC_SOURCE_MIC_SOURCE,
	.app_source = AECMUSIC_SOURCE_APP_SOURCE,
	.remind_source = AECMUSIC_SOURCE_REMIND_SOURCE,
	.aec_ref_source = AECMIC_SOURCE_AEC_REF_SOURCE,

	//ROBOEFFECT effect SINKÓ³Éä
	.dac0_sink = AECMUSIC_SINK_DAC0_SINK,
	.app_sink = AECMIC_SINK_APP_SINK,
	.aec_ref_sink = AECMUSIC_SINK_AEC_REF_SINK,
};

const uint16_t aec_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX] =
{
	[EQ_MODE_ADJUST] = AUDIOEFFECT_SET_EFFECTADDR(DefaultNet, AECMUSIC_music_EQ_ADDR),
	[MUSIC_VOLUME_ADJUST] = AUDIOEFFECT_SET_EFFECTADDR(DefaultNet, AECMUSIC_gain_control0_ADDR),
	[VIRTURAL_71CHN_ENABLE] = AUDIOEFFECT_SET_EFFECTADDR(DefaultNet, AECMUSIC_virtual_surround_2ch0_ADDR),
	[EARPHONE_EQ_ADJUST] = AUDIOEFFECT_SET_EFFECTADDR(DefaultNet, AECMUSIC_eq0_ADDR),
	[MIC_VOLUME_ADJUST] = AUDIOEFFECT_SET_EFFECTADDR(AECNet, AECMIC_gain_control0_ADDR),
	[MIC_AEC_ENABLE] = AUDIOEFFECT_SET_EFFECTADDR(AECNet, AECMIC_aec0_ADDR),
	
	
};
