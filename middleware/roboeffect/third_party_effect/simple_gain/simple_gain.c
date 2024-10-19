/**
 **************************************************************************************
 * @file    split_gain.c
 * @brief   interface for user defined effect algorithm
 *
 * @author  Castle Cai
 * @version V1.0.0
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <nds32_utils_math.h>
#include "roboeffect_api.h"
#include "simple_gain.h"
/**
 * user gain begin
*/
bool roboeffect_simple_gain_init_if(void *node)
{
	uint8_t *context_ptr;
	roboeffect_user_defined_effect_info info;
	simple_gain_struct *gain_info;

	/**
	 * get information struct from instance
	*/
	roboeffect_user_defined_get_info(node, &info);

	/**
	 * get user defined struct context
	*/
	gain_info = info.context_memory;

	/**
	 * dummy initialize the context of the user_gain
	*/
	gain_info->data_a = 0xAB;
	gain_info->data_b = 0xCD;

	/**
	 * get additional memory pointer
	*/
	gain_info->ptr = roboeffect_user_defined_malloc(node, 16);

	/*Use gain_info->ptr to cache something*/

	// printf("%s called.", __func__);
	return TRUE;
}

bool roboeffect_simple_gain_config_if(void *node, int16_t *new_param, uint8_t param_num, uint8_t len)
{
	int ret;
	uint8_t method_flag = 0;
	roboeffect_user_defined_effect_info info;
	simple_gain_struct *gain_info;

	/**
	 * check parameters and update to effect instance
	*/
	if(ROBOEFFECT_ERROR_OK > roboeffect_user_defined_params_check(node, new_param, param_num, len, &method_flag))
	{
		return FALSE;
	}

	/**
	 * get information struct from instance
	*/
	roboeffect_user_defined_get_info(node, &info);

	/**
	 * get user defined struct context
	*/
	gain_info = info.context_memory;

	if((method_flag & METHOD_CFG_1) && info.is_active)
	{

	}

	return TRUE;
}

#define SCALING_Q12_MAX (0x1000)
static inline uint32_t db_to_scaling(float db, uint32_t scaling_max)
{
	return (uint32_t)roundf(powf(10.0f,((float)db/20.0f)) * scaling_max);
}

bool roboeffect_simple_gain_apply_if(void *node, int16_t *pcm_in1, int16_t *pcm_in2, int16_t *pcm_out, int32_t n)
{
	int i, s;
	int32_t pregain, *pcm_in_24 = (int32_t*)pcm_in1, *pcm_out_24 = (int32_t*)pcm_out;
	roboeffect_user_defined_effect_info info;
	simple_gain_struct *gain_info;

	/**
	 * get information struct from instance
	*/
	roboeffect_user_defined_get_info(node, &info);

	/**
	 * get user defined struct context
	*/
	gain_info = info.context_memory;
	
	/**
	 * apply data
	*/

	//info.parameters[0] is a dB, convert it to Q12.
	
	pregain = db_to_scaling(((float)info.parameters[1])/100, SCALING_Q12_MAX);

	if(info.width == BITS_24)
	{
		for(s = 0; s < n; s++)
		{
			if(info.ch_num == 2)
			{
				pcm_out_24[2 * s + 0] = __nds32__clips((((int64_t)pcm_in_24[2 * s + 0] * pregain + 2048) >> 12), (24)-1);
				pcm_out_24[2 * s + 1] = __nds32__clips((((int64_t)pcm_in_24[2 * s + 1] * pregain + 2048) >> 12), (24)-1);
			}
			else
			{
				pcm_out_24[s] = __nds32__clips((((int64_t)pcm_in_24[s] * pregain + 2048) >> 12), (24)-1);
			}
		}
	}
	else if(info.width == BITS_16)
	{
		for(s = 0; s < n; s++)
		{
			if(info.ch_num == 2)
			{
				pcm_out[2 * s + 0] = __nds32__clips((((int32_t)pcm_in1[2 * s + 0] * pregain + 2048) >> 12), (16)-1);
				pcm_out[2 * s + 1] = __nds32__clips((((int32_t)pcm_in1[2 * s + 1] * pregain + 2048) >> 12), (16)-1);
			}
			else
			{
				pcm_out[s] = __nds32__clips((((int32_t)pcm_in1[s] * pregain + 2048) >> 12), (16)-1);
			}
		}
	}

	return TRUE;
}

bool roboeffect_simple_gain_memory_size_if(roboeffect_memory_size_query *query, roboeffect_memory_size_response *response)
{
	int32_t max_delay_samples = ceilf(query->params[0] * query->sample_rate / 1000.0f);

	response->context_memory_size = sizeof(simple_gain_struct);

	if(query->params[0] == 0)
	{
		response->additional_memory_size = ceilf(max_delay_samples/32.0f)*19;
	}
	else if(query->params[0] == 1)
	{
		response->additional_memory_size = max_delay_samples * 2;
	}

	response->scratch_memory_size = 0;

	return TRUE;
}