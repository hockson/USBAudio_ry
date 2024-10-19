/**
 * @file ble_api.h
 * @author Cole (cole@mvsilicon.com)
 * @brief Le API
 * @version 0.1
 * @date 2023-03-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef _LE_APP_API_H_
#define _LE_APP_API_H_

#include <stdint.h>
#include <stdio.h>

#define BLE_INFO(fmt, args...) printf("[BLE_INFO]:" fmt, ##args)
#define BLE_GATT_UUID_128_LEN (16)
#define BLE_DFLT_DEVICE_MAX_NAME_LEN (18)
//#define DUOBLE_ROLE //����һ�忪��
/*****************************************************************************************/
typedef struct set_adv_data
{
    uint8_t *adv_data;
    uint16_t adv_len;
} set_adv_data_t;

typedef struct set_rsp_adv_data
{
    uint8_t *adv_rsp_data;
    uint16_t adv_rsp_len;
} set_rsp_adv_data_t;

// ��׼16λuuidģ��
typedef struct ble_gatt_att16_desc
{
    /// Attribute UUID (16-bit UUID - LSB First)
    uint16_t uuid16;
    /// Attribute information bit field (see enum #gatt_att_info_bf)
    uint16_t info;
    /// Attribute extended information bit field (see enum #gatt_att_ext_info_bf)
    /// Note:
    ///   - For Included Services and Characteristic Declarations, this field contains targeted handle.
    ///   - For Characteristic Extended Properties, this field contains 2 byte value
    ///   - For Client Characteristic Configuration and Server Characteristic Configuration, this field is not used.
    uint16_t ext_info;
} ble_gatt_att16_desc_t;

// ��׼�Զ���128λuuidģ��
typedef struct ble_gatt_att128_desc
{
    /// Attribute UUID (LSB First)
    uint8_t uuid[BLE_GATT_UUID_128_LEN];
    /// Attribute information bit field (see enum #gatt_att_info_bf)
    uint16_t info;
    /// Attribute extended information bit field (see enum #gatt_att_ext_info_bf)
    /// Note:
    ///   - For Included Services and Characteristic Declarations, this field contains targeted handle.
    ///   - For Characteristic Extended Properties, this field contains 2 byte value
    ///   - For Client Characteristic Configuration and Server Characteristic Configuration, this field is not used.
    uint16_t ext_info;
} ble_gatt_att128_desc_t;

typedef struct set_adv_interval
{
    uint32_t adv_intv_min;
    uint32_t adv_intv_max;
    uint8_t ch_map;
} set_adv_interval_t;

typedef struct le_init_parameter
{
    set_adv_interval_t adv_interval_param;
    set_rsp_adv_data_t rsp_data;
    set_adv_data_t adv_data;
    uint16_t att_default_mtu;
    uint16_t ble_uuid16_service;
    uint16_t *ble_uuid128_service;
    uint16_t ble_service_idxnb;
    ble_gatt_att128_desc_t *profile_uuid128;
    ble_gatt_att16_desc_t *profile_uuid16;
    uint8_t ble_device_name_len;
    uint8_t ble_device_name[BLE_DFLT_DEVICE_MAX_NAME_LEN];
} le_init_parameter_t;

le_init_parameter_t le_user_config;
/***********************************************************************************************/
/**
 * @brief LE��ػص��¼�ö��
*/
typedef enum
{
    LE_INIT_STATUS = 0,
    LE_CONNECTED,
    LE_DISCONNECT,
    LE_CONNECT_PARAMS_UPDATE,
    LE_MTU_EXCHANGE_RESULT,
    LE_RCV_DATA_EVENT,
    LE_APP_READ_DATA_EVENT,
    LE_ADV_REPORT_EVENT,
} LE_CB_EVENT;

typedef struct le_addr
{
    /// BD Address of device
    uint8_t addr[6];
} le_addr_t;


typedef struct ble_app_read_data
{
	uint16_t connect_handle;
    uint8_t *data;
    /// value handle
    uint16_t handle;

    uint16_t offset;
    /// len
    uint16_t len;
} ble_app_read_data_t;

/**
 * @brief service �������ݽṹ��
 * 
 */
typedef struct ble_rcv_data
{
    /// data
    uint8_t *data;
    /// Connection handle
    uint16_t conhdl;
    /// value handle
    uint16_t handle;
    /// len
    uint32_t len;
} ble_rcv_data_t;

/**
 * @brief LE �Ͽ������¼�����
 * 
 */
typedef struct le_disconnect_params
{
    /// Connection index
    uint8_t conidx;
    /// Connection handle
    uint16_t conhdl;
    /// Reason of disconnection
    uint16_t reason;
} le_disconnect_params_t;

/**
 * @brief LE��������¼�����
 * 
 */
typedef struct _le_connected_complete
{
    /// Connection index
    uint8_t conidx;
    /// Connection handle
    uint16_t conhdl;
    /// Connection interval
    uint16_t con_interval;
    /// Connection latency
    uint16_t con_latency;
    /// Link supervision timeout
    uint16_t sup_to;
    /// Clock accuracy
    uint8_t clk_accuracy;
    /// Peer address type
    uint8_t peer_addr_type;
    /// Peer BT address
    le_addr_t peer_addr;
    /// Role of device in connection (0 = Central / 1 = Peripheral)
    uint8_t role;
} le_connected_complete;

/**
 * @brief app_update_param ʹ��
 *
 */
typedef struct le_connection_param
{
    /// Connection interval minimum
    uint16_t intv_min;
    /// Connection interval maximum
    uint16_t intv_max;
    /// Latency
    uint16_t latency;
    /// Supervision timeout
    uint16_t time_out;
} le_connection_param_t;
/**
 * @brief LE���Ӳ��������¼�
 * 
 */
typedef struct _le_con_update_params
{
    /// LE Subevent code
    uint8_t subcode;
    /// Status of received command
    uint8_t status;
    /// Connection handle
    uint16_t conhdl;
    /// Connection interval value
    uint16_t con_interval;
    /// Connection latency value
    uint16_t con_latency;
    /// Supervision timeout
    uint16_t sup_to;
} le_con_update_params;
/**
 * @brief GAP�豸��ַ�ṹ��
 * 
 */
typedef struct gap_bdaddr
{
    /// BD Address of device
    uint8_t addr[6];
    /// Address type of the device 0=public/1=private random 
    uint8_t addr_type;
} gap_bdaddr_t;
enum le_phy_prop
{
    /// Scan connectable advertisements on the LE 1M PHY. Connection parameters for the LE 1M PHY are provided
    PROP_1M_BIT = (1 << 0),
    /// Connection parameters for the LE 2M PHY are provided
    PROP_2M_BIT = (1 << 1),
    /// Scan connectable advertisements on the LE Coded PHY. Connection parameters for the LE Coded PHY are provided
    PROP_CODED_BIT = (1 << 2),
};
/*************ע�⣺ֻ�е���ɫΪCentralʱ�Ż���������API START**************/
/// Scanning Types
enum le_scan_type
{
    /// General discovery
    SCAN_TYPE_GEN_DISC = 0,
    /// Limited discovery
    SCAN_TYPE_LIM_DISC,
    /// Observer
    SCAN_TYPE_OBSERVER,
    /// Selective observer
    SCAN_TYPE_SEL_OBSERVER,
    /// Connectable discovery
    SCAN_TYPE_CONN_DISC,
    /// Selective connectable discovery
    SCAN_TYPE_SEL_CONN_DISC,
};

/// Scanning properties bit field bit value
enum le_scan_prop
{
    /// Scan advertisement on the LE 1M PHY
    SCAN_PROP_PHY_1M_BIT = (1 << 0),
    SCAN_PROP_PHY_1M_POS = 0,
    /// Scan advertisement on the LE Coded PHY
    SCAN_PROP_PHY_CODED_BIT = (1 << 1),
    SCAN_PROP_PHY_CODED_POS = 1,
    /// Active scan on LE 1M PHY (Scan Request PDUs may be sent)
    SCAN_PROP_ACTIVE_1M_BIT = (1 << 2),
    SCAN_PROP_ACTIVE_1M_POS = 2,
    /// Active scan on LE Coded PHY (Scan Request PDUs may be sent)
    SCAN_PROP_ACTIVE_CODED_BIT = (1 << 3),
    SCAN_PROP_ACTIVE_CODED_POS = 3,
    /// Accept directed advertising packets if we use a RPA and target address cannot be solved by the
    /// controller
    SCAN_PROP_ACCEPT_RPA_BIT = (1 << 4),
    SCAN_PROP_ACCEPT_RPA_POS = 4,
    /// Filter truncated advertising or scan response reports
    SCAN_PROP_FILT_TRUNC_BIT = (1 << 5),
    SCAN_PROP_FILT_TRUNC_POS = 5,
};

/// Filtering policy for duplicated packets
enum le_scan_dup_filter_pol
{
    /// Disable filtering of duplicated packets
    DUP_FILT_DIS = 0,
    /// Enable filtering of duplicated packets
    DUP_FILT_EN,
    /// Enable filtering of duplicated packets, reset for each scan period
    DUP_FILT_EN_PERIOD,
};

typedef struct _le_adv_report
{
    /// Transmitter device address
    gap_bdaddr_t trans_addr;
    /// Target address (in case of a directed advertising report)
    gap_bdaddr_t target_addr;
    int8_t rssi;
    uint16_t data_len;
    uint8_t *buf;
} le_adv_report;

/// Connection parameters
/*@TRACE*/
typedef struct le_init_con_param
{
    /// Minimum value for the connection interval (in unit of 1.25ms). Shall be less than or equal to
    /// conn_intv_max value. Allowed range is 7.5ms to 4s.
    uint16_t conn_intv_min;
    /// Maximum value for the connection interval (in unit of 1.25ms). Shall be greater than or equal to
    /// conn_intv_min value. Allowed range is 7.5ms to 4s.
    uint16_t conn_intv_max;
    /// Slave latency. Number of events that can be missed by a connected slave device
    uint16_t conn_latency;
    /// Link supervision timeout (in unit of 10ms). Allowed range is 100ms to 32s
    uint16_t supervision_to;
    /// Recommended minimum duration of connection events (in unit of 625us)
    uint16_t ce_len_min;
    /// Recommended maximum duration of connection events (in unit of 625us)
    uint16_t ce_len_max;
} le_init_con_param_t;
/// Scan Window operation parameters
/*@TRACE*/
typedef struct le_scan_wd_op_param
{
    /// Scan interval (N * 0.625 ms)
    uint16_t scan_intv;
    /// Scan window (N * 0.625 ms)
    uint16_t scan_wd;
} le_scan_wd_op_param_t;

typedef struct le_scan_param
{
    /// Type of scanning to be started (see enum #gapm_le_scan_type)
    uint8_t type;
    /// Properties for the scan procedure (see enum #gapm_le_scan_prop for bit signification)
    uint8_t prop;
    /// Duplicate packet filtering policy
    uint8_t dup_filt_pol;
    /// Reserved for future use
    uint8_t rsvd;
    /// Scan window opening parameters for LE 1M PHY
    le_scan_wd_op_param_t scan_param_1m;
    /// Scan window opening parameters for LE Coded PHY
    le_scan_wd_op_param_t scan_param_coded;
    /// Scan duration (in unit of 10ms). 0 means that the controller will scan continuously until
    /// reception of a stop command from the application
    uint16_t duration;
    /// Scan period (in unit of 1.28s). Time interval betweem two consequent starts of a scan duration
    /// by the controller. 0 means that the scan procedure is not periodic
    uint16_t period;
} le_scan_param_t;

typedef struct le_connect_param
{
    /// Properties for the initiating procedure (see enum #gapm_le_init_prop for bit signification)
    uint8_t prop;
    /// Timeout for automatic connection establishment (in unit of 10ms). Cancel the procedure if not all
    /// indicated devices have been connected when the timeout occurs. 0 means there is no timeout
    uint16_t conn_to;
    /// Scan window opening parameters for LE 1M PHY
    le_scan_wd_op_param_t scan_param_1m;
    /// Scan window opening parameters for LE Coded PHY
    le_scan_wd_op_param_t scan_param_coded;
    /// Connection parameters for LE 1M PHY
    le_init_con_param_t conn_param_1m;
    /// Connection parameters for LE 2M PHY
    le_init_con_param_t conn_param_2m;
    /// Connection parameters for LE Coded PHY
    le_init_con_param_t conn_param_coded;
    /// Address of peer device in case filter accept list is not used for connection
    gap_bdaddr_t peer_addr_r;
} le_connect_param_t;

/*************ע�⣺ֻ�е���ɫΪCentralʱ�Ż���������API END**************/

typedef struct _LE_CB_PARAMS
{
    le_adv_report adv_report;
    le_disconnect_params_t dis_params;
    le_connected_complete con_params;
    le_con_update_params con_update_param;
    uint16_t le_init_status;
    uint16_t mtu_size;
    ble_rcv_data_t rcv_data;      //LE_RCV_DATA_EVENT
    ble_app_read_data_t read_data;//LE_APP_READ_DATA_EVENT
} LE_CB_PARAMS;

typedef void (*LeParamCB)(LE_CB_EVENT event, LE_CB_PARAMS *param);

LeParamCB LeCallBackAPP;

/*******************************FOR GATT CLIENTS START********************************/

/*************ע�⣺ֻ�е���ɫΪCentralʱ�Ż���������API START**************/
/**
 * @brief gatt client�ص�����
 */
typedef struct gatt_client_example_appli_itf
{
    void (*cb_discover_cmp)(uint8_t conidx, uint8_t user_lid, uint16_t metainfo, uint16_t status);
    void (*cb_read_cmp)(uint8_t conidx, uint8_t user_lid, uint16_t metainfo, uint16_t status, uint16_t hdl, uint16_t offset, const char *p_info);
    void (*cb_write_cmp)(uint8_t conidx, uint8_t user_lid, uint16_t metainfo, uint16_t status);
    void (*cb_info_recv)(uint8_t conidx, uint8_t event_type, uint16_t hdl, uint8_t info_len, const uint8_t *p_info);

} gatt_client_appli_itf_t;

/**
 * @brief gatt clientע��ص�����
 *
 * @param p_appli_itf
 * @return uint16_t
 */
uint16_t gatt_client_init(gatt_client_appli_itf_t *p_appli_itf);
/**
 * @brief gatt client����(���������API)
 *
 * @return uint16_t
 */
uint16_t gatt_client_config(void);
/**
 * @brief gatt client ��������
 *
 * @param conidx �豸���
 * @param hdl vlaue handle ���Ծ��
 * @param metainfo Ԫ����Ϣ ����callback���� ����client_user_lid;extern uint8_t client_user_lid
 * @param write_type �������� 0x00 GATT_WRITE, 0x01 GATT_WRITE_NO_RSP
 * @param p_data ����BUFF
 * @param offset ����ƫ��ֵ0 buffer��ʼλ��
 * @param length ���ͳ���
 * @return uint16_t
 */

uint16_t gatt_client_send_data(uint8_t conidx, uint16_t hdl, uint16_t metainfo, uint8_t write_type, uint8_t *p_data, uint16_t offset, uint16_t length);
/**
 * @brief gatt client ��ȡ����
 *
 * @param conidx �豸���
 * @param hdl value handle ���Ծ��
 * @param metainfo Ԫ����Ϣ ����callback����
 * @param offset ƫ��ֵ
 * @param length ����
 * @return uint16_t
 */

uint16_t gatt_client_read_data(uint8_t conidx, uint16_t hdl, uint16_t metainfo, uint16_t offset, uint16_t length);

/*******************************FOR GATT CLIENTS END********************************/

/*********************FOR CENTRAL ROlE START****************/
/**
 * @brief �����㲥����
*/
int prase_adv_data(const char *name, uint8_t *adv_data, uint16_t adv_len);
/**
 * @brief ��ʼɨ��
 * 
 * @param p_param 
 * @return uint16_t 
 */
uint16_t le_scan_start(le_scan_param_t* p_param);
/**
 * @brief ֹͣɨ��
 * 
 * @return uint16_t 
 */
uint16_t le_scan_stop(void);
/**
 * @brief �����豸
 * 
 * @param param 
 * @return uint16_t 
 */
uint16_t le_create_connection(le_connect_param_t *param);

/********************FOR CENTRAL ROlE END*********************/

/*************ע�⣺ֻ�е���ɫΪCentralʱ�Ż���������API END***********************/

/**
 * @brief ע���¼��ص�����
 *
 */
void LeAppRegCB(LeParamCB LeCBFunc);
/**
 * @brief ��ʼ��BLE��������
 *
 * @return uint8_t
 */
uint8_t LeInitConfigParams(void);

/**
 * @brief Le�¼��ص�����
 *
 * @param event
 * @param param
 */
void AppEventCallBack(LE_CB_EVENT event, LE_CB_PARAMS *param);
/*****************************�̻�API���Ǳ�Ҫ��ֹ�Ķ�******************************/

/**
 * @brief @brief ���ù㲥����(MAx LEN < 31)
 *
 * @param data
 * @param len
 */
void app_set_adv_data(uint8_t *data, uint16_t len);
/**
 * @brief @brief ���ù㲥�ظ�����(MAx LEN < 31)
 *
 * @param data
 * @param len
 */
void app_set_scan_rsp_data(uint8_t *data, uint16_t len);
/**
 * @brief           �������ݽӿ�
 *
 * @param conidx    0x00
 * @param user_lid  0x00
 * @param metainfo  0x00
 * @param hdl       ����handle-1Ϊvalue handle
 * @param p         ����
 * @param len       ����
 * @param nf_or_nd  0x00 GATT_NOTIFY, 0x01 GATT_INDICATE ��ʽ
 * @return uint32_t
 */
uint32_t ble_send_data(uint8_t conidx, uint8_t user_lid, uint16_t metainfo, uint16_t hdl, uint8_t *p, uint32_t len, uint8_t nf_or_nd);
/**
 * @brief �����Ͽ�����
 *
 */
void ble_app_disconnect(void);
/**
 * @brief ��ʼ�㲥
 *
 */
void app_start_advertising(void);
/**
 * @brief ֹͣ�㲥
 *
 */
void app_stop_advertising(void);
/**
 * @brief ���ù㲥���
 *
 * @param adv_param
 */
void app_set_adv_param(set_adv_interval_t adv_param);

/**
 * @brief �������Ӳ���
 *
 */
void app_update_param(struct le_connection_param *p_conn_param);
/**
 * @brief ��õ�ǰ�豸��
 *
 * @param p_name
 * @return uint8_t
 */
uint8_t app_get_dev_name(uint8_t *p_name);
#endif
