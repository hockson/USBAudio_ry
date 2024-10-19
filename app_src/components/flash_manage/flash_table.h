#ifndef __FLASH_TABLE_H__
#define __FLASH_TABLE_H__
#include "type.h"

// 8  --> 8Mb		
// 4/8/16/32/64/128
#define FLASH_CAPACTITY_TYPE	CHIP_FLASH_CAPACTITY

#define FLASH_CAPACTITY_ADDR	0xD4 

#define FLASH_DEFAULT_CAPACTITY	((FLASH_CAPACTITY_TYPE*1024*1024)/8)	

//flash data map
//���õ���ķ�ʽ(4KB����)
//������������£�SDKĬ��ʹ�����
//ʵ�ʵķֲ������\tools\merge_script\merge.ini�ļ�����
typedef struct _FLASH_DATA_MAP_
{
	//ĩβ8KB����������(���ݾ�SDK���ı���)
	uint8_t	reserve_1		[1024 * 4];		//size:4KB
	uint8_t	reserve_2		[1024 * 4];		//size:4KB

	uint8_t	flash_table		[1024 * 4];		//size:4KB	������
	uint8_t	sys_parameter	[1024 * 4];		//size:4KB	8MB SDK����������
#ifdef CFG_EFFECT_PARAM_IN_FLASH_EN
	uint8_t	effect_data		[1024 * CFG_EFFECT_PARAM_IN_FLASH_SIZE];	//��Ч��������
#endif
	uint8_t	bt_data			[1024 * 8];		//size:8KB  bt���� ��Լ�¼��
	uint8_t	bp_data			[1024 * 8];		//size:8KB	breakpoint����
}FLASH_DATA_MAP;

#define GET_MEMBER_SIZE(member)		sizeof(((FLASH_DATA_MAP *)0)->member)
#define GET_MEMBER_OFFSET(member)	(uint32_t)((((FLASH_DATA_MAP *)0)->member) + GET_MEMBER_SIZE(member))

#define BT_CONFIG_OFFSET			GET_MEMBER_OFFSET(reserve_1)
#define USER_CONFIG_OFFSET			GET_MEMBER_OFFSET(reserve_2)
#define FLASH_TABLE_OFFSET			GET_MEMBER_OFFSET(flash_table)
#define BT_DATA_OFFSET     			GET_MEMBER_OFFSET(bt_data)
#define BP_DATA_OFFSET     			GET_MEMBER_OFFSET(bp_data)
#define SYS_PARA_OFFSET				GET_MEMBER_OFFSET(sys_parameter)
#define EFFECT_DATA_OFFSET			GET_MEMBER_OFFSET(effect_data)

#define BP_DATA_FLASH_SIZE			GET_MEMBER_SIZE(bp_data)
#define BTDB_USERDATA_SIZE			GET_MEMBER_SIZE(bt_data)
#define BTDB_CONFIGDATA_SIZE		GET_MEMBER_SIZE(reserve_1)
#define FLASH_TABLE_SIZE			GET_MEMBER_SIZE(flash_table)

void flash_table_init(void);
bool flash_table_is_valid(void);

uint32_t get_remind_addr(void);			//��ʾ����ַ
uint32_t get_bp_data_addr(void);		//BT���ݣ���Լ�¼/linkkey�ȣ�
uint32_t get_bt_data_addr(void);		//ϵͳ���ݣ�ģʽ/����/�ϵ㲥�ŵȣ�
uint32_t get_user_config_addr(void);	//�û����ݣ������¼���ȹ���ʹ�ã�SN�����Ϣ��
uint32_t get_bt_config_addr(void);		//�����������ݣ������¼��/���Ժ�ʹ�ã�Ƶƫֵ/MAC��ַ�ȣ�
uint32_t get_sys_parameter_addr(void);	//flash ���������ݵ�ַ
uint32_t get_effect_data_addr(void);	//��Ч�������ݵ�ַ

#define REMIND_FLASH_STORE_BASE				(get_remind_addr())

/////////////////////////////////////////////////////////////////
//����flash�Ķ�������Ч�ģ�ֻ����ʱ����������
#define	CODE_ADDR				0x0
#define CONST_DATA_ADDR    		FLASH_DEFAULT_CAPACTITY
#define AUDIO_EFFECT_ADDR  		FLASH_DEFAULT_CAPACTITY		
#define FLASHFS_ADDR			FLASH_DEFAULT_CAPACTITY		
#define USER_DATA_ADDR     		FLASH_DEFAULT_CAPACTITY	
#define BP_DATA_ADDR     		FLASH_DEFAULT_CAPACTITY

#endif

