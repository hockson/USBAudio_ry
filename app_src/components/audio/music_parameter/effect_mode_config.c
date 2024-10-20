#include <nds32_utils_math.h>
#include <string.h>
#include <math.h>
#include "roboeffect_api.h"
#include "user_defined_effect_api.h"
#include "user_effect_parameter.h"
#include "nn_denoise_api.h"
#include "main_task.h"
#include "bt_config.h"
#include "breakpoint.h"
//#include "auto_gen_msg_process.h"

typedef enum
{
	EffectModeStateSuspend	= 0,//EffectMode挂起态，按键不能直接切换到该音效模式。固定模式使用，比如HFP AEC
	EffectModeStateReady,		//EffectMode就绪态，按键可以切换
}EffectModeState;

typedef struct __UserEffectModeValidConfig
{
	uint8_t 						effect_mode;
	EffectModeState 				effect_mode_state;
	const AUDIOEFFECT_SOURCE_SINK_NUM *source_sink;
	const AUDIOEFFECT_EFFECT_PARA	 *effect_para;
	const uint16_t 					*effect_control;
	uint8_t 						(*msg_process[MaxNet])(int msg);
}UserEffectModeValidConfig;

extern const AUDIOEFFECT_EFFECT_PARA bypass_effect_para;
extern const AUDIOEFFECT_SOURCE_SINK_NUM bypass_mode;

extern const AUDIOEFFECT_EFFECT_PARA HunXiang_effect_para;
extern const AUDIOEFFECT_EFFECT_PARA DianYin_effect_para;
extern const AUDIOEFFECT_EFFECT_PARA MoYin_effect_para;
extern const AUDIOEFFECT_EFFECT_PARA HanMai_effect_para;
extern const AUDIOEFFECT_EFFECT_PARA NanBianNv_effect_para;
extern const AUDIOEFFECT_EFFECT_PARA NvBianNan_effect_para;
extern const AUDIOEFFECT_EFFECT_PARA WaWaYin_effect_para;
extern const AUDIOEFFECT_SOURCE_SINK_NUM karaoke_mode;

extern const AUDIOEFFECT_EFFECT_PARA mic_effect_para[MaxNet];
extern const AUDIOEFFECT_SOURCE_SINK_NUM mic_mode;
extern const uint16_t mic_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX];

extern const AUDIOEFFECT_EFFECT_PARA music_effect_para[MaxNet];
extern const AUDIOEFFECT_SOURCE_SINK_NUM music_mode;
extern const uint16_t music_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX];

extern const AUDIOEFFECT_EFFECT_PARA micusbAI_effect_para;
extern const AUDIOEFFECT_SOURCE_SINK_NUM micusbAI_mode;

extern const AUDIOEFFECT_EFFECT_PARA hfp_effect_para[MaxNet];
extern const AUDIOEFFECT_SOURCE_SINK_NUM hfp_mode;
extern const uint16_t hfp_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX];

extern const AUDIOEFFECT_EFFECT_PARA aec_effect_para[MaxNet];
extern const AUDIOEFFECT_SOURCE_SINK_NUM aec_mode;
extern const uint16_t aec_effect_ctrl[AUDIOEFFECT_EFFECT_CONTROL_MAX];

//音效模式按键切换配置表，顺序切换
//包含音效参数节点，MSG消息处理
static const UserEffectModeValidConfig EffectModeToggleMap[] =
{
#ifdef CFG_AI_DENOISE_EN
	{EFFECT_MODE_MICUSBAI,	1,	&micusbAI_mode,	&micusbAI_effect_para,	NULL},
#endif
#ifdef CFG_FUNC_EFFECT_BYPASS_EN
	{EFFECT_MODE_BYPASS,	EffectModeStateSuspend,	&bypass_mode,	&bypass_effect_para,	NULL},
#else
	#ifdef CFG_FUNC_MIC_KARAOKE_EN
		{EFFECT_MODE_HunXiang,	EffectModeStateReady,	&karaoke_mode,	&HunXiang_effect_para,	msg_process_karaoke},
		{EFFECT_MODE_DianYin,	EffectModeStateReady,	&karaoke_mode,	&DianYin_effect_para,	msg_process_karaoke},
		{EFFECT_MODE_MoYin,		EffectModeStateReady,	&karaoke_mode,	&MoYin_effect_para,		msg_process_karaoke},
		{EFFECT_MODE_HanMai,	EffectModeStateReady,	&karaoke_mode,	&HanMai_effect_para,	msg_process_karaoke},
		{EFFECT_MODE_NanBianNv,	EffectModeStateReady,	&karaoke_mode,	&NanBianNv_effect_para,	msg_process_karaoke},
		{EFFECT_MODE_NvBianNan,	EffectModeStateReady,	&karaoke_mode,	&NvBianNan_effect_para,	msg_process_karaoke},
		{EFFECT_MODE_WaWaYin,	EffectModeStateReady,	&karaoke_mode,	&WaWaYin_effect_para,	msg_process_karaoke},
	#else
		{EFFECT_MODE_MIC,		EffectModeStateReady,	&mic_mode,		&mic_effect_para,		mic_effect_ctrl,	NULL	},
		{EFFECT_MODE_MUSIC,		EffectModeStateReady,	&music_mode,	&music_effect_para,		music_effect_ctrl,	NULL	},
		{EFFECT_MODE_USBAEC,	EffectModeStateSuspend,	&aec_mode,		&aec_effect_para,		aec_effect_ctrl,	NULL	},
	#endif
#endif
#if defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT)
	{EFFECT_MODE_HFP_AEC,	EffectModeStateSuspend,	&hfp_mode,		&hfp_effect_para,	hfp_effect_ctrl,	NULL},
#endif
};

#define 	EffectModeToggleMapMaxIndex		(sizeof(EffectModeToggleMap)/sizeof(EffectModeToggleMap[0]))

//获取框图的source/sink配置
AUDIOEFFECT_SOURCE_SINK_NUM * get_user_effect_source_sink(void)
{
	uint32_t i = 0;

	for(i=0;i<EffectModeToggleMapMaxIndex;i++)
	{
		if(mainAppCt.EffectMode == EffectModeToggleMap[i].effect_mode)
			return EffectModeToggleMap[i].source_sink;
	}
	return EffectModeToggleMap[0].source_sink;
}

//获取框图音效参数
AUDIOEFFECT_EFFECT_PARA * get_user_effect_parameters(uint8_t mode)
{
	uint32_t i = 0;

	for(i=0;i<EffectModeToggleMapMaxIndex;i++)
	{
		if(mode == EffectModeToggleMap[i].effect_mode)
			return EffectModeToggleMap[i].effect_para;
	}
	return EffectModeToggleMap[0].effect_para;
}

//获取默认的第一个音效
uint8_t GetFristEffectMode(void)
{
	uint8_t i;

	for(i = 0; i < EffectModeToggleMapMaxIndex; i++)
	{
		if(EffectModeToggleMap[i].effect_mode_state != EffectModeStateSuspend)
		{
			return EffectModeToggleMap[i].effect_mode;
		}
	}

	return EffectModeToggleMap[0].effect_mode;
}

//切换到下一个音效模式
uint8_t GetNextEffectMode(void)
{
	uint8_t i,j;

	for(i = 0; i < EffectModeToggleMapMaxIndex; i++)
	{
		if(mainAppCt.EffectMode == EffectModeToggleMap[i].effect_mode)
		{	//切换到下一个音效模式
			for(j = (i+1) % EffectModeToggleMapMaxIndex; j != i ; j = (j+1) % EffectModeToggleMapMaxIndex)
			{
				if(EffectModeToggleMap[j].effect_mode_state != EffectModeStateSuspend)
				{
					return EffectModeToggleMap[j].effect_mode;
				}
			}
		}
	}

	//没有找到合法的音效模式，不切换
	return mainAppCt.EffectMode;

	//没有找到音效模式，默认第一个
//	return GetFristEffectMode();
}

//音效按键消息处理
uint8_t EffectModeMsgProcess(uint16_t Msg)
{
	uint8_t i;

	for(i = 0; i < EffectModeToggleMapMaxIndex; i++)
	{
		if(mainAppCt.EffectMode == EffectModeToggleMap[i].effect_mode &&
		   EffectModeToggleMap[i].msg_process[DefaultNet])
		{
			return EffectModeToggleMap[i].msg_process[DefaultNet](Msg);
		}
	}

	return 0;
}
uint8_t EffectModeMsgProcessForNet(uint16_t Msg, uint8_t net)
{
	uint8_t i;

	for(i = 0; i < EffectModeToggleMapMaxIndex; i++)
	{
		if(mainAppCt.EffectMode == EffectModeToggleMap[i].effect_mode &&
		   EffectModeToggleMap[i].msg_process[net])
		{
			return EffectModeToggleMap[i].msg_process[net](Msg);
		}
	}

	return 0;
}

//比较2个Mode是否是同一个框图
bool EffectModeCmp(uint8_t mode1,uint8_t mode2)
{
	uint8_t i,index1,index2;

	index1 = 0xff;
	index2 = 0xff;
	for(i = 0; i < EffectModeToggleMapMaxIndex; i++)
	{
		if(mode1 == EffectModeToggleMap[i].effect_mode)
			index1 = i;
		if(mode2 == EffectModeToggleMap[i].effect_mode)
			index2 = i;
		if(index1 != 0xff && index2 != 0xff)
		{
			if(EffectModeToggleMap[index1].source_sink == EffectModeToggleMap[index2].source_sink)
				return 1;
			else
				return 0;
		}
	}

	return 0;
}

//获取当前框图有多少个音效参数
uint8_t GetEffectModeParaCount(void)
{
	uint8_t i,count,index = 0xff;

	count = 0;

	for(i=0;i<EffectModeToggleMapMaxIndex;i++)
	{
		if(mainAppCt.EffectMode == EffectModeToggleMap[i].effect_mode)
		{
			index = i;
			break;
		}
	}

	if(index < EffectModeToggleMapMaxIndex)
	{
		for(i = 0; i < EffectModeToggleMapMaxIndex; i++)
		{
			if(EffectModeToggleMap[i].source_sink == EffectModeToggleMap[index].source_sink)
				count++;
		}
	}

	return count;
}

//获取第num个音效参数 在当前框图中的数组序号
uint8_t	GetEffectModeParaIndex(uint8_t num)
{
	uint8_t i,cur_index = 0xff;

	for(i=0;i<EffectModeToggleMapMaxIndex;i++)
	{
		if(mainAppCt.EffectMode == EffectModeToggleMap[i].effect_mode)
		{
			cur_index = i; 	// 记录当前音效下标
			break;
		}
	}

	if(cur_index < EffectModeToggleMapMaxIndex)
	{
		for(i = 0; i < EffectModeToggleMapMaxIndex; i++)
		{
			if(EffectModeToggleMap[i].source_sink == EffectModeToggleMap[cur_index].source_sink)
			{
				if(num == 0)
					return i; //找到，返回参数
				else
					num--;
			}
		}

		return cur_index; //错误，返回原来参数
	}

	return 0;
}

//找到当前框图中的第index个音效的effect mode
uint8_t GetCurSameEffectModeID(uint8_t index)
{
	return EffectModeToggleMap[GetEffectModeParaIndex(index)].effect_mode;
}

//找到当前框图中的第index个音效的参数
AUDIOEFFECT_EFFECT_PARA * GetCurSameEffectModeAudioPara(uint8_t index)
{
	return EffectModeToggleMap[GetEffectModeParaIndex(index)].effect_para;
}


//获取effect mode 在当前框图音效参数的编号
uint8_t GetCurEffectModeNum(void)
{
	uint8_t i,num,index = 0xff;

	num = 0;
	for(i=0;i<EffectModeToggleMapMaxIndex;i++)
	{
		if(mainAppCt.EffectMode == EffectModeToggleMap[i].effect_mode)
		{
			index = i;
			break;
		}
	}

	if(index < EffectModeToggleMapMaxIndex)
	{
		for(i = 0; i < index; i++)
		{
			if(EffectModeToggleMap[i].source_sink == EffectModeToggleMap[index].source_sink)
				num++;
		}
	}

	return num;
}

//获取音效控制节点index
uint16_t GetEffectControlIndex(AUDIOEFFECT_EFFECT_CONTROL type)
{
	uint8_t i;

	if(type >= AUDIOEFFECT_EFFECT_CONTROL_MAX)
		return 0;

	for(i = 0; i < EffectModeToggleMapMaxIndex; i++)
	{
		if(mainAppCt.EffectMode == EffectModeToggleMap[i].effect_mode &&
		   EffectModeToggleMap[i].effect_control)
		{
			return EffectModeToggleMap[i].effect_control[type];
		}
	}

	return 0;
}
