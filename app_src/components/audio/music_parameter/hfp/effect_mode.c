#include "user_effect_flow_hfp.h"
#include "user_effect_parameter.h"

const AUDIOEFFECT_EFFECT_PARA hfp_effect_para[MaxNet] =
{
	{
		.user_effect_name = (uint8_t *)"Hfp",
		.user_effect_list = (roboeffect_effect_list_info *)&user_effect_list_hfp,
		.user_effect_steps = (roboeffect_effect_steps_table *)&user_effect_steps_hfp,
		.user_effects_script = (uint8_t *)user_effects_script_hfp,
		.user_effect_parameters = (uint8_t *)user_effect_parameters_hfp_Hfp,
		.user_module_parameters = (uint8_t *)user_module_parameters_hfp_Hfp,
		.get_user_effects_script_len = get_user_effects_script_len_hfp,
	}
};

const AUDIOEFFECT_SOURCE_SINK_NUM hfp_mode =
{
	//��Ҫɾ����source/sinkĬ��ֵ
	AUDIOEFFECT_SOURCE_SINK_DEFAULT_INIT,

	//ROBOEFFECT effect SOURCEӳ��
	.mic_source = HFP_SOURCE_MIC_SOURCE,
	.app_source = HFP_SOURCE_APP_SOURCE,
	.remind_source = HFP_SOURCE_REMIND_SOURCE,
	.rec_source = HFP_SOURCE_REC_SOURCE,

	//ROBOEFFECT effect SINKӳ��
	.dac0_sink = HFP_SINK_DAC0_SINK,
	.app_sink = HFP_SINK_APP_SINK,
	.stereo_sink = HFP_SINK_STEREO_SINK,
	.rec_sink = HFP_SINK_REC_SINK,
	.spdif_sink = HFP_SINK_SPDIF_SINK,
};

const uint16_t hfp_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX] =
{
	[MUSIC_VOLUME_ADJUST] = AUDIOEFFECT_SET_EFFECTADDR(DefaultNet, HFP_music_gain_ADDR),
	[MIC_VOLUME_ADJUST] = AUDIOEFFECT_SET_EFFECTADDR(DefaultNet, HFP_mic_gain_ADDR),
	[MIC_SILENCE_DETECTOR_PARAM] = AUDIOEFFECT_SET_EFFECTADDR(DefaultNet, HFP_silence_detector_mic_ADDR),
	[MUSIC_SILENCE_DETECTOR_PARAM] = AUDIOEFFECT_SET_EFFECTADDR(DefaultNet, HFP_silence_detector_music_ADDR),
};
