////////////////////////////////////////////////
//
//
#include "debug.h"

#include "ble_api.h"

#ifdef CFG_APP_CONFIG
#include "app_config.h"
#include "bt_play_mode.h"
#endif
#include "rtos_api.h"
#include "bt_config.h"
#if (BLE_SUPPORT)

uint8_t BleConnectFlag = 0;

#ifdef CFG_RONGYUAN_CMD
void bt_tx_data(uint8_t *buf, uint8_t len)
{
	if(BleConnectFlag == 0)	return;
	
	if(len>0 && len<240)
	{
		ble_send_data(0x0,0,0,0x0e,buf,len,0);
	}
}

#endif

static void btAddrToString(unsigned char addr[6], char *str)
{
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

uint8_t GetBLEConnState(void)
{
    return BleConnectFlag;
}

void LeAppRegCB(LeParamCB LeCBFunc)
{
    LeCallBackAPP = LeCBFunc;
}
uint8_t test_buff[] = {'O','K',0x0d,0x0a};
uint8_t err_buff[] = {'e','r','r',0x0d,0x0a};

void AppEventCallBack(LE_CB_EVENT event, LE_CB_PARAMS *param)
{
    char addr[12];
    switch (event)
    {
    case LE_INIT_STATUS:
        if (param->le_init_status == 0x00)
        {
            BLE_INFO("BLE INIT OK!!!\n");
        }
        else
        {
            BLE_INFO("\n----> LE INIT FAILED <-----\n status: 0x%02x\n", param->le_init_status);
        }
        break;
#ifdef DUOBLE_ROLE
    case LE_ADV_REPORT_EVENT:
    {
    	printf("LE_ADV_REPORT_EVENT\n");
        if (!prase_adv_data("BP15_BLE_LYM", param->adv_report.buf, param->adv_report.data_len))
            break;
        le_scan_stop();
        le_connect_param_t con_param =
            {
                .prop = PROP_1M_BIT,
                .conn_to = 0,

                .scan_param_1m.scan_intv = 160,  // 100ms
                .scan_param_1m.scan_wd = 80,     // 50ms
                .scan_param_coded.scan_intv = 0, // disabled
                .scan_param_coded.scan_wd = 0,   // disabled

                .conn_param_1m.conn_intv_min = 40, // 50ms
                .conn_param_1m.conn_intv_max = 40, // 50ms
                .conn_param_1m.conn_latency = 5,
                .conn_param_1m.supervision_to = 100, // 1000 ms
                .conn_param_1m.ce_len_min = 5,
                .conn_param_1m.ce_len_max = 10,

                .conn_param_2m.conn_intv_min = 40, // 50ms
                .conn_param_2m.conn_intv_max = 40, // 50ms
                .conn_param_2m.conn_latency = 5,
                .conn_param_2m.supervision_to = 100, // 1000 ms
                .conn_param_2m.ce_len_min = 5,
                .conn_param_2m.ce_len_max = 10,

                .conn_param_coded.conn_intv_min = 40, // 50ms
                .conn_param_coded.conn_intv_max = 40, // 50ms
                .conn_param_coded.conn_latency = 5,
                .conn_param_coded.supervision_to = 100, // 1000 ms
                .conn_param_coded.ce_len_min = 5,
                .conn_param_coded.ce_len_max = 10,


            };
        con_param.peer_addr_r = param->adv_report.trans_addr,
        le_create_connection(&con_param);
    }
    break;
#endif
    case LE_CONNECTED:
        printf("\n***********LE_CONNECTED************\n");
        BLE_INFO("connect handle: 0x%04x\n", param->con_params.conhdl);
        btAddrToString(&param->con_params.peer_addr.addr[0], addr);
        BLE_INFO("Address: %s", addr);
        break;
    case LE_DISCONNECT:
        printf("\n***********LE_DISCONNECT************\n");
        BLE_INFO("disconnect reson: 0x%02x\ndisconnect handle: 0x%04x\nindex: 0x%04x\n",
        param->dis_params.reason, param->dis_params.conhdl, param->dis_params.conidx);
	 BleConnectFlag = 0;
        break;
    case LE_CONNECT_PARAMS_UPDATE:
        printf("\n*****LE_CONNECT_PARAMS_UPDATE*****\n");
        BLE_INFO("con_interval: %d\ncon_latency: %d\nSupervision timeout: %d\n",
        param->con_update_param.con_interval, param->con_update_param.con_latency, param->con_update_param.sup_to);
	 BleConnectFlag = 1;
        break;
    case LE_MTU_EXCHANGE_RESULT:
    {
        printf("\n*****LE_MTU_EXCHANGE_RESULT*****\n");
//		le_connection_param_t par;
//		par.intv_max = 25;
//		par.intv_min = 25;
//		par.latency = 0;
//		par.time_out = 2000;
//		app_update_param(&par);
        BLE_INFO("MTU: %d\n", param->mtu_size);
    }
    break;
    case LE_RCV_DATA_EVENT:
    {
        uint8_t i;
        printf("\n*****LE_RCV_DATA_EVENT*****\n");
        BLE_INFO("connect handle: 0x%04x,att_handle: 0x%04x\n", param->rcv_data.conhdl, param->rcv_data.handle);
#if 1
        BLE_INFO("RCV DATA: ");
        for (i = 0; i < param->rcv_data.len; i++)
        {
            printf("0x%02x,", param->rcv_data.data[i]);
        }
        printf("\n");
#endif

#ifdef CFG_RONGYUAN_CMD
		if((param->rcv_data.data[0] == 0xA5 && param->rcv_data.data[1] == 0x5A)) //֡ͷ
		{
			roboeffect_prot_parse_big_block(param->rcv_data.data, param->rcv_data.len);
//			if(ble_tx_length>0 && ble_tx_length<240)
//			{
//				ble_send_data(param->rcv_data.conhdl,0,0,0x0e,hid_tx_buf,ble_tx_length,0);
//				ble_tx_length = 0;
//				break;
//			}
		}
#else		
        //����ʾ��
        ble_send_data(param->rcv_data.conhdl,0,0,0x0e,err_buff,sizeof(err_buff),0);
#endif  
        break;
    }
    case LE_APP_READ_DATA_EVENT:
    {
        printf("\n*****LE_APP_READ_DATA_EVENT*****\n");
        switch (param->read_data.handle)
        {
        case 0:
            break;

        default:
            BLE_INFO("\n*****test_buff*****\n");
            BLE_INFO("param->read_data.connect_handle: 0x%04x\n", param->read_data.connect_handle);
            BLE_INFO("param->read_data.handle: 0x%04x\n", param->read_data.handle);
            BLE_INFO("param->read_data.offset: 0x%04x\n", param->read_data.offset);
            param->read_data.len = sizeof(test_buff);
            param->read_data.data = test_buff;//���������ڣ���Ϊȫ��BUFF
            break;
        }

        break;
    }
    default:
        break;
    }
}

#endif
