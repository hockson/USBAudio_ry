/**
 *****************************************************************************
 * @file     device_stor_audio_request.c
 * @author   owen
 * @version  V1.0.0
 * @date     7-September-2015
 * @brief    device audio and mass-storage module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#include <string.h>
#include "type.h"
#include "debug.h"
#include "otg_device_hcd.h"
#include "otg_device_standard_request.h"
#include "otg_device_descriptor.h"
#include "otg_device_audio.h"

#ifdef CFG_APP_CONFIG
#include "app_config.h"
#include "main_task.h"
#if FLASH_BOOT_EN
void start_up_grate(uint32_t UpdateResource);
#endif
#endif

//------------------------------------//
void HIDUsb_Tx(uint8_t *buf,uint16_t len);
void HIDUsb_Rx(uint8_t *buf,uint16_t len);


uint8_t hid_tx_buf[256];
void IsAndroid(void);

//------------------------------------//


const uint8_t  DeviceQualifier[10] = {10,6,0x10,0x01,0,0,0,64,1,0};
extern void OnDeviceAudioRcvIsoPacket(void);
extern void OnDeviceAudioSendIsoPacket(void);

extern void OTG_DeviceAudioRequest(void);
void hid_recive_data(void);
void hid_send_data(void);
const uint8_t DeviceString_LangID[] = {0x04, 0x03, 0x09, 0x04};
const uint8_t MicByteSet[] = {0,MIC_ALT1_BITS,MIC_ALT2_BITS};
const uint8_t SpeakerByteSet[] = {0,SPEAKER_ALT1_BITS,SPEAKER_ALT2_BITS};
uint8_t Setup[8];
uint8_t Request[256];

uint8_t *ConfigDescriptor;
uint8_t *InterfaceNum;
const char *gDeviceProductString ="mvsilicon B5 usb audio";			//max length: 32bytes
const char *gDeviceString_Manu ="MV-SILICON";						//max length: 32bytes
const char *gDeviceString_SerialNumber ="20190808";					//max length: 32bytes

extern UsbAudio UsbAudioSpeaker;
extern UsbAudio UsbAudioMic;

void OTG_DeviceModeSel(uint8_t Mode,uint16_t UsbVid,uint16_t UsbPid)
{
	
	DeviceDescriptor[8] = UsbVid;
	DeviceDescriptor[9] = UsbVid>>8;
	DeviceDescriptor[10] = UsbPid;
	DeviceDescriptor[11] = UsbPid>>8;
	ConfigDescriptor = (uint8_t *)ConfigDescriptorTab[Mode];
	InterfaceNum = (uint8_t *)InterFaceNumTab[Mode];
}


/**
 * @brief  ���Ϳ��ƴ��������Ӧ������
 * @param  Resp Ӧ������
 * @param  n Ӧ�����ݳ��ȣ�1 or 2
 * @return NONE
 */
void OTG_DeviceSendResp(uint16_t Resp, uint8_t n)
{
	Resp = CpuToLe16(Resp);
	OTG_DeviceControlSend((uint8_t*)&Resp, n,3);
}

/**
 * @brief  �����ȡ����������
 * @param  NONE
 * @return NONE
 */
void OTG_DeviceGetDescriptor(void)
{
	uint8_t 	StringBuf[32 * 2 + 2];
	uint8_t*	UsbSendPtr = 0;
	uint16_t	Len = 0;
	switch(Setup[3])
	{
		case USB_DT_DEVICE:
		//	OTG_DBG("USB_DT_DEVICE\n");
			UsbSendPtr = (uint8_t*)DeviceDescriptor;
			Len = sizeof(DeviceDescriptor);
			break;

		case USB_DT_CONFIG:
			//OTG_DBG("USB_DT_CONFIG\n");
			UsbSendPtr = (uint8_t*)ConfigDescriptor;
            Len = UsbSendPtr[3];
            Len = Len<<8;
            Len = Len + UsbSendPtr[2];
			break;

		case USB_DT_STRING:
		//	OTG_DBG("USB_DT_STRING\n");
			if(Setup[2] == 0)			//lang ids
			{
				UsbSendPtr = (uint8_t*)DeviceString_LangID;
				Len = UsbSendPtr[0];
				break;
			}
			else if(Setup[2] == 1)		//manu
			{
				UsbSendPtr = (uint8_t*)gDeviceString_Manu;
			}
			else if(Setup[2] == 2)		//product
			{
				UsbSendPtr = (uint8_t*)gDeviceProductString;
			}
			else 						//serial number
			{
				UsbSendPtr = (uint8_t*)gDeviceString_SerialNumber;
			}

			for(Len = 0; Len < 32; Len++)
			{
				if(UsbSendPtr[Len] == '\0')
				{
					break;
				}
				StringBuf[2 + Len * 2 + 0] = UsbSendPtr[Len];
				StringBuf[2 + Len * 2 + 1] = 0x00;
			}

			Len = Len * 2 + 2;
			StringBuf[0] = Len;
			StringBuf[1] = 0x03;
			UsbSendPtr = StringBuf;
			break;

		case USB_DT_INTERFACE:
			//PCĬ�ϲ��ᷢ�͸�����
			//OTG_DBG("USB_DT_INTERFACE\n");
			break;

		case USB_DT_ENDPOINT:
			//PCĬ�ϲ��ᷢ�͸�����
		//	OTG_DBG("USB_DT_ENDPOINT\n");
			break;
			
		case USB_DT_DEVICE_QUALIFIER:
			UsbSendPtr = (uint8_t*)DeviceQualifier;
			Len = 10;
			break;

		case USB_HID_REPORT:
			if(Setup[4] == InterfaceNum[HID_DATA_INTERFACE_NUM])
			{
				//OTG_DBG("HID_DATA_INTERFACE_NUM REPORTR\n");
#if HID_DATA_FUN_EN
				UsbSendPtr = (uint8_t*)&HidDataReportDescriptor[0];
				Len = sizeof(HidDataReportDescriptor);
#endif
			}
			else if(Setup[4] == InterfaceNum[HID_CTL_INTERFACE_NUM])
			{
				//OTG_DBG("HID_CTL_INTERFACE_NUM REPORTR\n");
				UsbSendPtr = (uint8_t*)&AudioCtrlReportDescriptor[0];
				Len = sizeof(AudioCtrlReportDescriptor);
			}
			else
			{
				//OTG_DBG("NOT FOUND INTERFACE %d\n",Setup[4]);
			}
			break;

		default:
		//	OTG_DBG("UsbDeviceSendStall:100\n");
			OTG_DeviceStallSend(DEVICE_CONTROL_EP);
			return;
	}

	if(Len > (Setup[7] * 256 + Setup[6]))
	{

		Len = Setup[7] * 256 + Setup[6];
	}
	OTG_DeviceControlSend((uint8_t*)UsbSendPtr, Len,3);
}

void OTG_DeviceAudioInit();
void OTG_DeviceStandardRequest()
{
	uint8_t Resp[8];
	//OTG_DBG("\nSetup[1] = %d\n\n", Setup[1]);
	switch(Setup[1])
	{
		case USB_REQ_GET_STATUS:
			//OTG_DBG("GetStatus\n");
			//���ڻ�ȡUSB�豸�ӿڶ˵��״̬
			Resp[0] = 0x00;
			Resp[1] = 0x00;
			OTG_DeviceControlSend((uint8_t*)&Resp,2,10);
			break;

		case USB_REQ_CLEAR_FEATURE:
			//OTG_DBG("ClearFeature\n");
			//����������߽�ֹUSB�豸00,�ӿ�01,�˵�02,��ĳЩ���ԣ������ݴ���
			break;

		case USB_REQ_SET_FEATURE:
		//	OTG_DBG("SetFeature\n");
			//�������û���ʹUSB�豸00,�ӿ�01,�˵�02,��ĳЩ���ԣ������ݴ���
			OTG_DeviceStallSend(Setup[4]);
			break;

		case USB_REQ_SET_ADDRESS:
		//	OTG_DBG("SetAddress\n");
			OTG_DeviceAddressSet(Setup[2] & 0x7F);
			break;

		case USB_REQ_GET_DESCRIPTOR:
			//OTG_DBG("GetDescriptor\n");
			OTG_DeviceGetDescriptor();
			break;
		
		case USB_REQ_SET_DESCRIPTOR:
			//OTG_DBG("GetDescriptor111\n");
			//DeviceGetDescriptor();
			break;

		case USB_REQ_GET_CONFIGURATION:
		//	OTG_DBG("GetConfiguration\n");
			Resp[0] = 0x01;
			//OtgDeviceControlSend(Resp, 1,3);
			OTG_DeviceControlSend((uint8_t*)&Resp, 1,3);
			break;

		case USB_REQ_SET_CONFIGURATION:
			{
				//	DBG("Audio_ISO sam test\n");
#if(CFG_PARA_USB_MODE == READER)
				OTG_DeviceEndpointReset(DEVICE_BULK_OUT_EP,TYPE_BULK_OUT);
				OTG_DeviceEndpointReset(DEVICE_BULK_IN_EP,TYPE_BULK_IN);
#else
				OTG_DeviceEndpointReset(DEVICE_INT_IN_EP,TYPE_INT_IN);
				OTG_DeviceEndpointReset(DEVICE_ISO_OUT_EP,TYPE_ISO_OUT);
				OTG_DeviceEndpointReset(DEVICE_ISO_IN_EP,TYPE_ISO_IN);
				OTG_DeviceEndpointPacketSizeSet(DEVICE_ISO_OUT_EP,DEVICE_FS_ISO_OUT_MPS);
				OTG_DeviceEndpointPacketSizeSet(DEVICE_ISO_IN_EP,DEVICE_FS_ISO_IN_MPS);
#ifdef CFG_APP_USB_AUDIO_MODE_EN
				OTG_EndpointInterruptEnable(DEVICE_ISO_OUT_EP,OnDeviceAudioRcvIsoPacket);
				OTG_EndpointInterruptEnable(DEVICE_ISO_IN_EP,OnDeviceAudioSendIsoPacket);
				OTG_DeviceAudioInit();
				UsbAudioMic.InitOk = 1;
				UsbAudioSpeaker.InitOk = 1;
#endif
#endif
			}
			break;

		case USB_REQ_GET_INTERFACE:
			OTG_DeviceControlSend((uint8_t*)&Resp, 1,3);
			break;

		case USB_REQ_SET_INTERFACE:
		#ifdef CFG_APP_USB_AUDIO_MODE_EN
			if(Setup[4] == InterfaceNum[AUDIO_SRM_IN_INTERFACE_NUM])
			{
				DBG("mic %d",Setup[2]);
				UsbAudioMic.AltSet = Setup[2];
				if(UsbAudioMic.AltSet == 0)
				{
					OTG_EndpointInterruptDisable(DEVICE_ISO_IN_EP);
					OTG_DeviceEndpointReset(DEVICE_ISO_IN_EP,TYPE_ISO_IN);
					OTG_DeviceEndpointPacketSizeSet(DEVICE_ISO_IN_EP,DEVICE_FS_ISO_IN_MPS);
				}else{
					OTG_EndpointInterruptEnable(DEVICE_ISO_IN_EP,OnDeviceAudioSendIsoPacket);
					OTG_DeviceISOSend(DEVICE_ISO_IN_EP,0,0);
				}
				UsbAudioMic.ByteSet = MicByteSet[UsbAudioMic.AltSet];
			}
			else if(Setup[4] == InterfaceNum[AUDIO_SRM_OUT_INTERFACE_NUM])
			{
				UsbAudioSpeaker.AltSet = Setup[2];
				DBG("speaker %d",Setup[2]);
				UsbAudioSpeaker.ByteSet = SpeakerByteSet[UsbAudioSpeaker.AltSet];
				if(UsbAudioSpeaker.AltSet == 0)
				{
					UsbAudioSpeaker.AltSlow = 1;
				}
			}
		#endif
			break;

		case USB_REQ_SYNCH_FRAME:
			//OTG_DBG("SYNC FRAME\n");
			break;

		default:
		//	OTG_DBG("UsbDeviceSendStall 006\n");
			OTG_DeviceStallSend(DEVICE_CONTROL_EP);
			break;
	}
}

uint32_t pc_upgrade = 0;
#ifdef CFG_FUNC_USBDEBUG_EN
extern MCU_CIRCULAR_CONTEXT usb_fifo;
const uint8_t usbdebug_head[] = {0xA5,0x5A,0x30,0x01};
#endif
//�豸������
void OTG_DeviceClassRequest()
{
	if((Setup[0] == 0x22) && (Setup[1] == 0x01))
	{
#ifdef CFG_APP_USB_AUDIO_MODE_EN
		OTG_DeviceAudioRequest();
#endif
		return;
	}
	//CLASS ���ݽӿ�����������,ÿ���ӿ��¿����ж�����Ҳ�п���û���������ʵ�ʸ����Э�����
	if(Setup[4] == InterfaceNum[MSC_INTERFACE_NUM])
	{
	//	OTG_DBG("MSC_INTERFACE_NUM\n");
		OTG_DeviceSendResp(0x0000, 1);
	}

#ifdef CFG_APP_USB_AUDIO_MODE_EN
	else if(Setup[4] ==  InterfaceNum[AUDIO_ATL_INTERFACE_NUM])
	{
		//OTG_DBG("AUDIO_ATL_INTERFACE_NUM\n");
		OTG_DeviceAudioRequest();
	}
	else if(Setup[4] == InterfaceNum[AUDIO_SRM_OUT_INTERFACE_NUM])
	{
	//	OTG_DBG("AUDIO_SRM_OUT_INTERFACE_NUM\n");
		OTG_DeviceAudioRequest();
	}
	else if(Setup[4] == InterfaceNum[AUDIO_SRM_IN_INTERFACE_NUM])
	{
	//	OTG_DBG("AUDIO_SRM_IN_INTERFACE_NUM\n");
		OTG_DeviceAudioRequest();
	}
#endif
	else if(Setup[4] == InterfaceNum[HID_CTL_INTERFACE_NUM])
	{
		//OTG_DBG("HID_CTL_INTERFACE_NUM\n");
		if(Setup[1] == 0x01)//get report
		{
			if(Setup[3] == 0x01)//get report
			{
				Setup[0] = 0;
				OTG_DeviceControlSend((uint8_t*)Setup, 1,3);
			}
		}
	}
	else if(Setup[4] == InterfaceNum[HID_DATA_INTERFACE_NUM])
	{
		//uint32_t len=0;
		//OTG_DBG("HID_DATA_INTERFACE_NUM\n");//hid_send_data();
		if((Setup[3] == 0x02)&&(Setup[0] == 0x21))//out
		{
			hid_recive_data();
		}
		else if((Setup[3] == 0x01)&&(Setup[0] == 0xA1))//int
		{
#ifdef CFG_FUNC_USBDEBUG_EN
			extern bool hid_tx_buf_is_used;
			if(!hid_tx_buf_is_used)
			{
				memcpy(hid_tx_buf,usbdebug_head,sizeof(usbdebug_head));
				uint32_t datalen = MCUCircular_GetData(&usb_fifo,&hid_tx_buf[4],252);
				if(datalen < 252)
				{
					memset(&hid_tx_buf[4]+datalen,0,252-datalen);
				}
			}
			hid_tx_buf_is_used = 0;
#endif
			hid_send_data();
		}
#if FLASH_BOOT_EN
		else if((Setup[3] == 0x03)&&(Setup[0] == 0xA1))//GetReport (Feature Report)
		{
#ifdef CFG_FUNC_FLASH_PARAM_ONLINE_TUNING_EN
			if(Request[3] == 0x01 &&  Request[0] == 0xA1)
			{
				extern bool FlashParamUsb_Tx(void);

				if(!FlashParamUsb_Tx())
					hid_send_data();  //�����ݷ��ͣ������������
			}
			else
#endif
			{
				DBG("pc_upgrade start 1\n");
				if(pc_upgrade)
				{
					DBG("pc_upgrade start 2\n");
					Setup[0] = 0x55;
					OTG_DeviceControlSend(Setup,Setup[7]*256+Setup[6],1);
					start_up_grate(SysResourceUsbDevice);
				}
				else
				{
					Setup[0] = 0;
					OTG_DeviceControlSend(Setup,Setup[7]*256+Setup[6],1);
				}
			}
		}
		else if((Setup[3] == 0x03)&&(Setup[0] == 0x21))//SetReport (Feature Report)
		{
			pc_upgrade = 0;
			DBG("pc_upgrade start 0\n");
			if(Request[0] == 0x55)//��CODE����
			{
				//uint8_t *p = (uint8_t *)(0x10000 + 0xB8);
				//p = Request+1;// bkd // 2019.5.7
				if(memcmp((uint8_t*)(0x10000 + 0xB8),Request + 1,4) != 0)//����Ҫ������
				{
					pc_upgrade = 1;
				}
			}
			else if(Request[0] == 0xAA)//��code����������������
			{
				pc_upgrade = 1;
			}
		}
#endif
		else
		{
           // OTG_DBG("Others Cmd\n");
		}		
	}
	
#ifdef CFG_APP_USB_AUDIO_MODE_EN
	else
	{
		OTG_DeviceAudioRequest();
	}
#endif

}


//�����Զ������ݴ���
void OTG_DeviceManufacturerRequest()
{
	//�����Զ������ݴ���
}


//δ֪����
void OTG_DeviceOtherRequest()
{
	//OTG_DBG("UsbDeviceSendStall\n");
	OTG_DeviceStallSend(DEVICE_CONTROL_EP);
}

//__attribute__((weak))// bkd // 2019.5.7
//void start_up_grate(uint32_t UpdateResource)
//{
//}

/**
 * @brief  ����PC�����Ŀ�������
 * @param  NONE
 * @return NONE
 */
void OTG_DeviceRequestProcess(void)
{
	uint8_t BusEvent = OTG_DeviceBusEventGet();
	uint32_t DataLeng;
	uint8_t ReqType;
	if(BusEvent & 0x04)
	{
//		OTG_DeviceAddressSet(0);
#ifdef CFG_APP_USB_AUDIO_MODE_EN
		UsbAudioMic.InitOk = 0;
		UsbAudioSpeaker.InitOk = 0;
#endif
	}
	if(OTG_DeviceSetupReceive(Setup, 8, &DataLeng) != DEVICE_NONE_ERR)
	{
		return;
	}
	else if(DataLeng != 8)
	{
		DBG("Setup Packet error!\n");
		return;
	}
	//IsAndroid();
	//�жϷ���
	//�����out ��Ҫ�����������,Ȼ��������
	//�����in����Ҫ��׼�����ݣ�Ȼ��������
	if((Setup[0]&0x80) == 0)//out
	{
		//if(!((Setup[3] == 0x02)&&(Setup[0] == 0x21)&&(Setup[1] == 0x09)))//audio effect out
		{
			uint32_t temp=0;
			temp = Setup[7]*256 + Setup[6];
			if(temp)
			{
				int i;
				memset(Request,0,sizeof(Request));
				for(i=0;i<temp/64;i++)
				{
					OTG_DeviceControlReceive(Request+i*64,64,&DataLeng,10);
				}
				if(temp%64)
				{
					OTG_DeviceControlReceive(Request+i*64,temp%64,&DataLeng,10);
				}
			}
		}
	}

	ReqType = (Setup[0]&0x60)>>5;
	//DBG("ReqType:%d\n",ReqType);
	switch(ReqType)
	{
		case 0:
			//��׼����
			OTG_DeviceStandardRequest();
			break;

		case 1:
			//������
			OTG_DeviceClassRequest();
			break;

		case 2:
			//��������
			OTG_DeviceManufacturerRequest();
			break;

		case 3:
			//��������
			OTG_DeviceOtherRequest();
			break;			
	}
}

//*************************************************//
//*************************************************//
//*************************************************//



void hid_recive_data(void)
{
#ifdef CFG_COMMUNICATION_BY_USB
	HIDUsb_Rx(Request,256);
#endif

#ifdef CFG_FUNC_FLASH_PARAM_ONLINE_TUNING_EN
	if((Request[0] == 0xA5 && Request[1] == 0x5A) && //֡ͷ
		(Request[2] == 0x30 || Request[2] == 0x20))  //������0x30/������0x20
	{
		extern void FlashParamUsb_Rx(uint8_t *buf,uint16_t buf_len);
		extern void FlashSn_Rx(uint8_t *buf,uint16_t buf_len);

		if(Request[2] == 0x30)
			FlashParamUsb_Rx(Request+3,256-3);
		else
			FlashSn_Rx(Request+3,256-3);
	}
#endif
}


void hid_send_data(void)
{
	OTG_DeviceControlSend(hid_tx_buf,256,6);
}

void IsAndroid(void)
{
	/////�ж��Ƿ�Android�ֻ� "A1 01 00 01 03 00 01 00"
	if( (Setup[0]==0xA1) && (Setup[1]==0x01) )//
	{
		if( (Setup[2]==0x00) && (Setup[3]==0x01) )
		{
			if( (Setup[4]==0x03) && (Setup[5]==0x00) )
			{
				if( (Setup[6]==0x01) && (Setup[7]==0x00) )
				{
					//gCtrlVars.usb_android = 1;
				}
			}
		}
	}
}
