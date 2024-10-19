/***************************************************
 * @file     user_effect_flow_aecMic.h                      
 * @brief   auto generated                          
 * @author  ACPWorkbench: 3.11.4                 
 * @version V1.2.0                                  
 * @Created 2024-08-20T17:23:38                                      

 * @copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 ***************************************************/


#ifndef __USER_EFFECT_FLOW_AECMIC_H__
#define __USER_EFFECT_FLOW_AECMIC_H__

#include "stdio.h"
#include "type.h"
#include "roboeffect_api.h"

#define AECMIC_ROBOEFFECT_LIB_VER "2.23.3"

typedef enum _AECMIC_roboeffect_io_enum
{
    AECMIC_SOURCE_MIC_SOURCE,
    AECMIC_SOURCE_AEC_REF_SOURCE,

    AECMIC_SINK_APP_SINK,
} AECMIC_roboeffect_io_enum;


typedef enum _AECMIC_roboeffect_effect_list_enum{

    AECMIC_gain_control0_ADDR = 0x81,
    AECMIC_aec0_ADDR,
    AECMIC_mic_ns_ADDR,
    AECMIC_mic_EQ_ADDR,
    AECMIC_mic_drc_ADDR,
    AECMIC_gain_control4_ADDR,
    AECMIC_pcm_delay0_ADDR,
    AECMIC_COUNT_ADDR,

} AECMIC_roboeffect_effect_list_enum;

extern const unsigned char user_effects_script_aecMic[];

extern roboeffect_effect_list_info user_effect_list_aecMic;

extern const roboeffect_effect_steps_table user_effect_steps_aecMic;

extern uint32_t get_user_effects_script_len_aecMic(void);

extern char *parameter_group_name_aecMic[1];
extern const unsigned char user_effect_parameters_aecMic_AEC_Mic[];
extern const unsigned char user_module_parameters_aecMic_AEC_Mic[];
#endif/*__USER_EFFECT_FLOW_AECMIC_H__*/
