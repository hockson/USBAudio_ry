/**
 **************************************************************************************
 * @file    roboeffect_api.h
 * @brief   roboeffect memory management
 *
 * @author  Castle Cai
 * @version V1.0.3
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef __ROBOEFFECT_API_H__
#define __ROBOEFFECT_API_H__

/*Roboeffect Library version*/
#define ROBOEFFECT_LIB_VER "2.23.2"

#include <stdio.h>
#include <nds32_intrinsic.h>
#include "type.h"
// #include "audio_effect_library.h"
#include "fader.h"

#define EXEC_PROC_MEM_BLOCKS 10
#define EXEC_PROC_STEP_MAX 50
#define EXEC_EFFECT_NODE_MAX 40
#define EXEC_MEM_4K_SIZE (4096)

#define ROBOEFFECT_IO_TYPE_SRC 1
#define ROBOEFFECT_IO_TYPE_DES 2



#define ROBOEFFECT_CH_MONO          1
#define ROBOEFFECT_CH_STEREO        2
#define ROBOEFFECT_CH_MONO_STEREO   3

#define ALIGN4(x) (((x) + 3) & ~3)

#define IO_UNIT_ID(ptr) ((ptr)->io_id)
#define IO_UNIT_MEM_ID(ptr) ((ptr)->mem_id)
#define IO_UNIT_WIDTH(ptr) ((ptr)->width)
#define IO_UNIT_CH(ptr) ((ptr)->ch)
#define IO_UNIT_NAME(ptr) ((ptr)->name)
#define IO_UNIT_VALID(ptr) \
		((ptr)->width == BITS_16 || (ptr)->width == BITS_24 || (ptr)->width == BITS_32 ? TRUE : FALSE)
#define IO_UNIT_WIDTH_BYTES(ptr) \
    (((ptr)->width == BITS_16) ? 1 : (((ptr)->width == BITS_24) ? 2 : -1))



typedef enum _roboeffect_effect_type_enum
{
	ROBOEFFECT_AUTO_TUNE = 0x00,
	ROBOEFFECT_DC_BLOCKER,
	ROBOEFFECT_DRC,
	ROBOEFFECT_ECHO,
	ROBOEFFECT_EQ,
	ROBOEFFECT_NOISE_SUPPRESSOR_EXPANDER,
	ROBOEFFECT_FREQ_SHIFTER,
	ROBOEFFECT_HOWLING_SUPPRESSOR,
	ROBOEFFECT_NOISE_GATE,
	ROBOEFFECT_PITCH_SHIFTER,
	ROBOEFFECT_REVERB,
	ROBOEFFECT_SILENCE_DETECTOR,
	ROBOEFFECT_THREE_D,
	ROBOEFFECT_VIRTUAL_BASS,
	ROBOEFFECT_VOICE_CHANGER,
	ROBOEFFECT_GAIN_CONTROL,
	ROBOEFFECT_VOCAL_CUT,
	ROBOEFFECT_REVERB_PRO,
	ROBOEFFECT_VOICE_CHANGER_PRO,
	ROBOEFFECT_PHASE_CONTROL,
	ROBOEFFECT_VOCAL_REMOVER,
	ROBOEFFECT_PITCH_SHIFTER_PRO,
	ROBOEFFECT_VIRTUAL_BASS_CLASSIC,
	ROBOEFFECT_PCM_DELAY,
	ROBOEFFECT_HARMONIC_EXCITER,
	ROBOEFFECT_CHORUS,
	ROBOEFFECT_AUTO_WAH,
	ROBOEFFECT_STEREO_WIDENER,
	ROBOEFFECT_PINGPONG,
	ROBOEFFECT_THREE_D_PLUS,
	ROBOEFFECT_SINE_GENERATOR,
	ROBOEFFECT_NOISE_SUPPRESSOR_BLUE,
	ROBOEFFECT_FLANGER,
	ROBOEFFECT_FREQ_SHIFTER_FINE,
	ROBOEFFECT_OVER_DRIVE,
	ROBOEFFECT_DISTORTION_DS1,
	ROBOEFFECT_EQ_DRC,
	
	ROBOEFFECT_AEC,
	ROBOEFFECT_COMPANDER,
	ROBOEFFECT_LOW_LEVEL_COMPRESSOR,
	ROBOEFFECT_BEAT_TRACKER,
	ROBOEFFECT_ENGINE_SOUND,
	ROBOEFFECT_BIQUAD,
	ROBOEFFECT_CHORUS2,
	ROBOEFFECT_REVERB_PLATE,
	ROBOEFFECT_HOWLING_SUPPRESSOR_FINE,
	ROBOEFFECT_HOWLING_GUARD,
	ROBOEFFECT_VIRTUAL_BASS_TD,
	ROBOEFFECT_OVER_DRIVE_PLOY,
	ROBOEFFECT_TREMOLO,
	ROBOEFFECT_BEAMFORMING,
	ROBOEFFECT_VIRTUAL_SURROUND_2CH,
	ROBOEFFECT_ROBOT_TONE,
	ROBOEFFECT_NOISE_GENERATOR,
	ROBOEFFECT_NOISE_SUPPRESSOR_BLUE_DUAL,
	ROBOEFFECT_PHASE_INVERTER,
	ROBOEFFECT_FILTER_BUTTERWORTH,
	ROBOEFFECT_DYNAMIC_EQ,
	ROBOEFFECT_VAD,
	ROBOEFFECT_LR_BALANCER,
	ROBOEFFECT_HOWLING_SUPPRESSOR_SPECIFIED,

	/*node type below*/
	ROBOEFFECT_FADER,//
	ROBOEFFECT_DOWNMIX_2TO1,
	ROBOEFFECT_UPMIX_1TO2,

	//channel
	ROBOEFFECT_CHANNEL_COMBINER,
	ROBOEFFECT_CHANNEL_SELECTOR,
	ROBOEFFECT_ROUTE_SELECTOR,


	ROBOEFFECT_USER_DEFINED_EFFECT_BEGIN,//
} roboeffect_effect_type_enum;

typedef enum _ROBOEFFECT_ERROR_CODE
{
  ROBOEFFECT_EFFECT_NOT_EXISTED = -256,
	ROBOEFFECT_EFFECT_PARAMS_NOT_FOUND,
	ROBOEFFECT_INSUFFICIENT_MEMORY,
	ROBOEFFECT_EFFECT_INIT_FAILED,
	ROBOEFFECT_ILLEGAL_OPERATION,
	ROBOEFFECT_EFFECT_LIB_NOT_MATCH_1,//for parameters
	ROBOEFFECT_EFFECT_LIB_NOT_MATCH_2,//for lib Macro
	ROBOEFFECT_ADDRESS_NOT_EXISTED,
	ROBOEFFECT_PARAMS_ERROR,
	ROBOEFFECT_FRAME_SIZE_ERROR,
	ROBOEFFECT_MEMORY_SIZE_QUERY_ERROR,
	ROBOEFFECT_EFFECT_VER_NOT_MATCH_ERROR,//one effect version not match
	ROBOEFFECT_LIB_VER_NOT_MATCH_ERROR,//roboeffect lib version in parameters not match
	ROBOEFFECT_3RD_PARTY_LIB_NOT_MATCH_ERROR,//third party library not match

	ROBOEFFECT_FLASH_BIN_ERROR,//Invalid format for flash bin
	ROBOEFFECT_CONTEXT_MEMORY_ERROR,//roboeffect context memory error, maybe a NULL

	// No Error
	ROBOEFFECT_ERROR_OK = 0,					/**< no error */
} ROBOEFFECT_ERROR_CODE;

typedef enum _param_act_method
{
	ACTION_NONE     = 0x00,//
	ACTION_FADEOI   = 0x01,//
	ACTION_FADEOI_1 = 0x02,//
	ACTION_FADEOI_2 = 0x04,//
	ACTION_STEP     = 0x08,//
	ACTION_MAX      = 0xff,
}param_act_method;


typedef enum _roboeffect_width
{
	BITS_0  = 0,
	BITS_16 = 16,
	BITS_24 = 24,
	BITS_32 = 32,
} roboeffect_width;

typedef enum _roboeffect_channel
{
	CH_NONE = 0,
	CH_MONO = 1,
	CH_STEREO = 2,
} roboeffect_channel;

typedef enum _roboeffect_fader_step
{
	FADE_INOUT_NONE = 0,
	FADE_INOUT_OUT,
	FADE_INOUT_ZERO,
	FADE_INOUT_IN,
} roboeffect_fader_step;

typedef enum _roboeffect_operation
{
	MEM_COPY = 0x01,
	MEM_MIX,
	MEM_CLEAN,
} roboeffect_operation;

typedef enum _roboeffect_param_cfg_method
{
	METHOD_NONE = 0x00,//only update
	METHOD_INIT = 0x01,//need call init func
	METHOD_CFG_1 = 0x02,//need call config func 1
	METHOD_CFG_2 = 0x04,//need call config func 2
	METHOD_CFG_3 = 0x08,//need call config func 3
	METHOD_CFG_FADEOI = 0x10,//need do fade out/in
	METHOD_CFG_STEP = 0x20,//need apply new param step by step
	METHOD_MAX = 0xff,
}roboeffect_param_cfg_method;

typedef enum _roboeffect_frame_size
{
	FZ_64 = 0x01,//64 samples
	FZ_128 = 0x02,//128 samples
	FZ_256 = 0x04,//256 samples
	FZ_512 = 0x08,//512 samples
	FZ_ANY = 0xffff
}roboeffect_frame_size;

typedef enum _roboeffect_frame_operation
{
	FRAME_OP_INCREASE = 1,
	FRAME_OP_DECREASE = -1,

}roboeffect_frame_operation;

typedef struct _roboeffect_fader_context_inner
{
	uint8_t step;//roboeffect_fader_step
	FaderContext cnx;
} roboeffect_fader_context_inner;

typedef struct _roboeffect_proc_mm_node
{
	uint8_t id;
	void *mem;
	bool is_used;
	uint8_t width;//roboeffect_width
	uint8_t ch;//roboeffect_channel
	// fade_inout_step fade;
} roboeffect_proc_mm_node;

typedef struct _roboeffect_io_unit
{
	uint8_t io_id;
	uint8_t mem_id;
	uint8_t width;//roboeffect_width
	uint8_t ch;//roboeffect_channel
} roboeffect_io_unit;

typedef struct _roboeffect_step
{
	uint8_t id;
	uint8_t operate;
	uint8_t input_a;
	uint8_t input_b;
	uint8_t output_a;
} roboeffect_step;

typedef struct _roboeffect_effect_steps_table
{
	uint8_t step_num;
	uint8_t mem_used;
	uint8_t src_unit_num;
	uint8_t des_unit_num;
	// uint16_t mem_size_tb[EXEC_PROC_MEM_BLOCKS];
	const roboeffect_io_unit *src_unit;
	const roboeffect_io_unit *des_unit;
	const uint32_t *step;
} roboeffect_effect_steps_table;

typedef struct _roboeffect_exec_effect_info
{
	uint8_t addr;
	roboeffect_effect_type_enum type;
	roboeffect_width width;
	uint8_t ch_num;
} roboeffect_exec_effect_info;//parameter header for all

typedef struct _roboeffect_effect_list_info
{
	uint32_t count;//
	uint32_t sample_rate;
	uint32_t frame_size;
	const roboeffect_exec_effect_info *effect_info;
} roboeffect_effect_list_info;//parameter header for all

typedef struct _roboeffect_memory_size_query
{
	uint32_t sample_rate;
	uint32_t frame_size;
	uint32_t ch_num;
	uint32_t data_width;
	int16_t *params;
} roboeffect_memory_size_query;

typedef struct _roboeffect_memory_size_response
{
	uint32_t context_memory_size;
	uint32_t additional_memory_size;
	uint32_t scratch_memory_size;
} roboeffect_memory_size_response;

typedef bool (*roboeffect_effect_apply_func)(void *node, int16_t *pcm_in1, int16_t *pcm_in2, int16_t *pcm_out, int32_t n);
typedef bool (*roboeffect_effect_init_func)(void *node);
typedef bool (*roboeffect_effect_config_func)(void *node, int16_t *new_param, uint8_t param_num, uint8_t len);//can be all param(param_num=0xff) or only ONE param(param_num=1)
typedef int32_t (*roboeffect_effect_memory_size_func)(roboeffect_memory_size_query *query, roboeffect_memory_size_response *response);

/*********************************************flashbin start*********************************************************/
#define FLASHBIN_LEN_WIDTH (4)
#define FLASHBIN_SUBTYPE_WIDTH (4)
#define FLASHBIN_NAME_WIDTH (32)

#define PARAM_SUB_TYPE(index) ((0x01 << 8) | (index))
#define PRESET_SUB_TYPE(index) ((0x02 << 8) | (index))

#define FLASHBIN_MAGIC_NUM (0xA55AB44B)

#define FLASHBIN_VER_H 0
#define FLASHBIN_VER_M 2
#define FLASHBIN_VER_L 0

typedef enum _roboeffect_flashbin_sub_type
{
	ROBO_FB_SUBTYPE_SCRIPT = 0x00,
	ROBO_FB_SUBTYPE_EFFECTS_LIST,
	ROBO_FB_SUBTYPE_EFFECTS_INFO,
	ROBO_FB_SUBTYPE_SOURCE,
	ROBO_FB_SUBTYPE_SINK,
	ROBO_FB_SUBTYPE_STEPS,
	ROBO_FB_SUBTYPE_FLOW_INFO,
	ROBO_FB_SUBTYPE_PARAMS_MODE_INFO,
	ROBO_FB_SUBTYPE_PRESET_INFO,

} roboeffect_flashbin_sub_type;

#pragma pack(1)
typedef struct _roboeffect_flashbin_header
{
	char id_char[4];
	uint32_t total_length;
	uint8_t version[4];
	uint8_t robo_version[4];
	uint16_t flow_cnt;
	uint16_t flow_name_len;
#if 1	
	uint16_t current_flow_index;
	uint16_t current_mode_index;
	uint32_t reverse;
	char flow_name_ptr[];
#else
	uint16_t current_flow_cnt;
	uint16_t reverse;
	char flow_name_ptr[];
#endif
} roboeffect_flashbin_header;

typedef struct _roboeffect_flashbin_flow_pair
{
	uint16_t flow_index;
	uint16_t param_mode_index;
} roboeffect_flashbin_flow_pair;

typedef struct _roboeffect_flashbin_param_header
{
	uint32_t effect_param_len;
	uint32_t codec_param_len;
	uint32_t name_len;

	char name[];
} roboeffect_flashbin_param_header;

#pragma pack()

#define GET_EFFECT_PARAM_RAW(v) \
	((uint8_t*)v + sizeof(roboeffect_flashbin_param_header) + (v->name_len))

#define GET_CODEC_PARAM_RAW(v) \
	((uint8_t*)v + sizeof(roboeffect_flashbin_param_header) + (v->name_len) + (v->effect_param_len))


/*********************************************flashbin end*********************************************************/

typedef struct _roboeffect_effect_property_struct
{
	uint8_t type_num;
	uint8_t channel_support;//1=only mono, 2=only stereo, 3=mono or stereo
	uint16_t frame_size;//0xffff=anything
	uint16_t param_size;//param numbers one as half-word
	uint32_t cn_size;

	roboeffect_effect_init_func init_func;
	roboeffect_effect_config_func config_func;//if parameter change in init, should call init_func
	roboeffect_effect_apply_func apply_func;

	roboeffect_effect_memory_size_func memory_size_func;

	// int16_t param_list[ROBOEFFECT_PARAM_LEN_MAX];
	// roboeffect_param_cfg_method cfg_method[ROBOEFFECT_PARAM_LEN_MAX];//define how to config parameters
} roboeffect_effect_property_struct;//parameter header for all


typedef struct _roboeffect_user_defined_effect_info
{
	uint32_t sample_rate;
	uint8_t ch_num;
	uint8_t parameters_len;
	roboeffect_width width;
	void* context_memory;
	void* additional_memory;

	int16_t* parameters;
	int16_t* parameters_backup;
	param_act_method param_act;

	bool is_active;

	uint8_t *scratch_memory;
} roboeffect_user_defined_effect_info;//


/**
 * @brief Get context current size for roboeffect
 * 
 * @param effect_steps : execution table context
 * @param effect_list : execution effect list
 * @param parameters : effects parameters data
 * @return int32_t : memory size in Bytes; if < 0, check ROBOEFFECT_ERROR_CODE
 */
int32_t roboeffect_estimate_memory_size(const roboeffect_effect_steps_table *effect_steps, const roboeffect_effect_list_info *effect_list, const uint8_t *parameters);

/**
 * @brief Get context memory size of ONE effect according to address, can be called BEFORE roboeffect_init().
 * 
 * @param address : effect address 
 * @param effect_list : execution effect list
 * @param parameters : effects parameters data
 * @return int32_t : memory size in Bytes; if < 0, check ROBOEFFECT_ERROR_CODE
 */
int32_t roboeffect_estimate_effect_size(uint8_t address, const roboeffect_effect_list_info *effect_list, const uint8_t *parameters);

/**
 * @brief Get context memory size of ONE effect according to address, should be called AFTER roboeffect_init();
 *
 * @param main_context : main context memory allocated by user.
 * @param address : effect address .
 * @param context_size : output context size of effect, only valid when the return value is ROBOEFFECT_ERROR_CODE.
 * @return ROBOEFFECT_ERROR_CODE 
 */
ROBOEFFECT_ERROR_CODE roboeffect_get_effect_size(void *context, uint8_t address, uint32_t *context_size);

/**
 * @brief Estimate frame size of the flow chart based on parameters.
 * 
 * @param effect_list : execution effect list
 * @param parameters : effects parameters data
 * @return uint32_t : 0 for error
 */
uint32_t roboeffect_estimate_frame_size(roboeffect_effect_list_info *exec_efft, uint8_t *param);

/**
 * @brief initial context for roboeffect
 * 
 * @param main_context : main context memory allocated by user
 * @param context_size : context memory size
 * @param effect_steps : execution table context
 * @param effect_list : execution effect list
 * @param parameters : effects parameters data
 * @return ROBOEFFECT_ERROR_CODE 
 */
ROBOEFFECT_ERROR_CODE roboeffect_init(void *main_context, uint32_t context_size, const roboeffect_effect_steps_table *effect_steps, const roboeffect_effect_list_info *effect_list, const uint8_t *parameters);

/**
 * @brief main loop for roboeffect application
 * 
 * @param main_context : context memory allocated by user
 * @return ROBOEFFECT_ERROR_CODE 
 */
ROBOEFFECT_ERROR_CODE roboeffect_apply(void *main_context);

/**
 * @brief Get free memory space in main_context
 * 
 * @param main_context : main context memory allocated by user
 * @return uint32_t 
 */
uint32_t roboeffect_get_free_memory_space(void *main_context);

/**
 * @brief Get source buffer block by source id
 * 
 * @param main_context: context memory allocated by user
 * @param id: source id
 * @return void* NULL if failed
 */
void *roboeffect_get_source_buffer(void *main_context, uint8_t id);

/**
 * @brief Get sink buffer block by source id
 * 
 * @param main_context: context memory allocated by user
 * @param id: source id
 * @return void* NULL if failed
 */
void *roboeffect_get_sink_buffer(void *main_context,  uint8_t id);

/**
 * @brief Get sink/source buffer size in bytes
 * 
 * @param main_context: context memory allocated by user
 * @return buffer size in bytes
 */
uint32_t roboeffect_get_buffer_size(void *main_context);

/**
 * @brief Enable/Disable one effect
 * 
 * @param main_context: context memory allocated by user
 * @param addr: effect's address, start from 0x81
 * @param enable: 0 for disable, 1 for enable
 * @return ROBOEFFECT_ERROR_CODE 
 */
ROBOEFFECT_ERROR_CODE roboeffect_enable_effect(void *main_context, uint8_t addr, uint8_t enable);

/**
 * @brief Enable/Disable ALL effect
 * 
 * @param main_context 
 * @param enable: 0 for disable, 1 for enable
 * @return ROBOEFFECT_ERROR_CODE 
 */
ROBOEFFECT_ERROR_CODE roboeffect_enable_all_effects(void *main_context, uint8_t enable);

/**
 * @brief Get effect enable/disable status
 * 
 * @param main_context: context memory allocated by user
 * @param addr: effect node's address, start from 0x81
 * @return true: enabled
 * @return false: disabled 
 */
bool roboeffect_get_effect_status(void *main_context, uint8_t addr);

/**
 * @brief Set parameter for one effect by address
 * 
 * @param main_context: context memory allocated by user 
 * @param addr: effect node's address, start from 0x81 
 * @param param_index: parameter index, start with 0, 0xff indicates all parameters 
 * @param param_input: parameter data
 * @return ROBOEFFECT_ERROR_CODE 
 */
ROBOEFFECT_ERROR_CODE roboeffect_set_effect_parameter(void *main_context, uint8_t addr, uint8_t param_index, int16_t *param_input);


/**
 * @brief Get parameters for one effect by address
 * 
 * @param main_context: context memory allocated by user  
 * @param addr: effect node's address, start from 0x81  
 * @param param_index: parameter index, start with 0, 0xff indicates all parameters  
 * @return const int16_t*: parameters data
 */
const int16_t* roboeffect_get_effect_parameter(void *main_context, uint8_t addr, uint8_t param_index);

/**
 * @brief Get the number of effect parameters
 * 
 * @param main_context : context memory allocated by user  
 * @param address : effect node's address, start from 0x81  
 * @return int16_t 
 */
int16_t roboeffect_get_effect_parameter_count(void *main_context, uint8_t address);

/**
 * @brief Get suit frame size according to current_frame_size given
 * 
 * @param main_context : context memory allocated by user  
 * @param target_frame_size : Generally, it is the current frame_size, but can also fill in the desired frame_size.
 * @param address : effect node's address, start from 0x81  
 * @param operation : 1 for increasing node, -1 for decreasing node.
 * @return error if 0 
 */

uint32_t roboeffect_recommend_frame_size_upon_effect_change(void *main_contex, uint32_t target_frame_size, uint8_t address, int8_t operation);
/**
 * @brief Get effect type name.
 * 
 * @param main_context : context memory allocated by user  
 * @param address : effect node's address, start from 0x81  
 * @param type_output : name string, user need send a block of memory to hold it.
 * @return ROBOEFFECT_ERROR_CODE 
 */
ROBOEFFECT_ERROR_CODE roboeffect_get_effect_type(void *main_context, uint8_t address, char *type_output);

/**
 * @brief Get effect class version number.
 * 
 * @param main_context : context memory allocated by user  
 * @param address : effect node's address, start from 0x81  
 * @param ver_output : output version number at ver_output[0], ver_output[1], ver_output[2]
 * @return ROBOEFFECT_ERROR_CODE 
 */
ROBOEFFECT_ERROR_CODE roboeffect_get_effect_version(void *main_context, uint8_t address, uint8_t ver_output[]);

/**
 * @brief Get roboeffect error code
 * 
 * @param main_context : context memory allocated by user  
 * @return ROBOEFFECT_ERROR_CODE 
 */
ROBOEFFECT_ERROR_CODE roboeffect_get_error_code(void *main_context);

/**
 * @brief Get effect context memory pointer
 * 
 * @param main_context : context memory allocated by user  
 * @param address : effect node's address, start from 0x81  
 * @return the pointer of context memory, NULL for error 
 */
void *roboeffect_get_effect_context(void *main_context, uint8_t addr);

/**
 * @brief For user defined effect api, check parameters, should be called in interface named roboeffect_XXXX_config_if
 * 
 * @param node : 1st parameter by roboeffect_XXXX_config_if, the context of effect instance.
 * @param new_param : 2nd parameter by roboeffect_XXXX_config_if
 * @param param_index : 3rd parameter by roboeffect_XXXX_config_if
 * @param len : 4th parameter by roboeffect_XXXX_config_if
 * @param *flag : output flag
 * @return ROBOEFFECT_ERROR_CODE 
 */
ROBOEFFECT_ERROR_CODE roboeffect_user_defined_params_check(void *node, int16_t *new_param, uint8_t param_index, uint8_t len, uint8_t *flag);

/**
 * @brief For user defined effect api, allocate memory, should be called in interface named roboeffect_XXXX_init_if, roboeffect_XXXX_config_if, roboeffect_XXXX_apply_if
 * 		Each effect entity can only allocate one memory block. 
 * 
 * @param node : 1st parameter by roboeffect_XXXX_config_if, the context of effect instance.
 * @param size : the size of the memory that needs to be allocated
 * @return the pointer of the memory allocated .
 */
void* roboeffect_user_defined_malloc(void *node, uint32_t size);


/**
 * @brief Get current effect information block
 * 
 * @param node : 1st parameter by roboeffect_XXXX_init_if, or roboeffect_XXXX_config_if, or roboeffect_XXXX_apply_if, the context of effect instance.
 * @param info : output, current effect information block, refers to roboeffect_user_defined_effect_info.
 * @return ROBOEFFECT_ERROR_CODE.
 */
ROBOEFFECT_ERROR_CODE roboeffect_user_defined_get_info(void *node, roboeffect_user_defined_effect_info *info);

#endif/*__ROBOEFFECT_API_H__*/

