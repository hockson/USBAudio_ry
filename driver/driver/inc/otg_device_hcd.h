/**
 *****************************************************************************
 * @file     otg_device_hcd.h
 * @author   Owen
 * @version  V1.0.0
 * @date     27-03-2017
 * @brief    device module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */
 
/**
 * @addtogroup OTG
 * @{
 * @defgroup otg_device_hcd otg_device_hcd.h
 * @{
 */
 
#ifndef __OTG_DEVICE_HCD_H__
#define	__OTG_DEVICE_HCD_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

// Max packet size. Fixed, user should not modify.
#define	DEVICE_FS_CONTROL_MPS		64
#define	DEVICE_FS_INT_IN_MPS		64
#define	DEVICE_FS_INT_OUT_MPS		64
#define	DEVICE_FS_BULK_IN_MPS		64
#define	DEVICE_FS_BULK_OUT_MPS		64

// Endpoint number. Fixed, user should not modify.
#define	DEVICE_CONTROL_EP			0x00
#define	DEVICE_INT_IN_EP			0x81
#define	DEVICE_INT_OUT_EP			0x01
#define	DEVICE_BULK_IN_EP			0x81
#define	DEVICE_BULK_OUT_EP			0x01

#define	DEVICE_ISO_OUT_EP1			0x01
#define	DEVICE_ISO_IN_EP			0x82
#define	DEVICE_ISO_OUT_EP			0x02

typedef enum _DEVICE_TRANSFER_TYPE
{
    TYPE_SETUP 			= 0xFF,
    TYPE_CONTROL_IN		= 0xFE,
    TYPE_CONTROL_OUT	= 0xFD,
	TYPE_ISO_IN 		= 0x81,
	TYPE_ISO_OUT 		= 0x01,
	TYPE_BULK_IN 		= 0x82,
	TYPE_BULK_OUT 		= 0x02,
	TYPE_INT_IN 		= 0x83,
	TYPE_INT_OUT 		= 0x03,
} DEVICE_TRANSFER_TYPE;




typedef enum _OTG_DEVICE_ERR_CODE
{
	DEVICE_NONE_ERR = 0,
	DEVICE_UNLINK_ERR,
	
	CONTROL_SEND_ERR,
	CONTROL_RCV_ERR,
	CONTROL_SETUP_ERR,
	
	BULK_SEND_ERR,
	BULK_RCV_ERR0,
	BULK_RCV_ERR1,
	BULK_RCV_ERR2,

	INT_SEND_ERR,
	INT_RCV_ERR0,
	INT_RCV_ERR1,
	INT_RCV_ERR2,
}OTG_DEVICE_ERR_CODE;
/**
 * @brief  Device Fifo initial
 * @param  NONE
 * @return NONE
 */
void OTG_DeviceFifoInit(void);
/**
 * @brief  Device initial
 * @param  NONE
 * @return NONE
 */
void OTG_DeviceInit(void);


/**
 * @brief  Device software connect.
 * @param  NONE
 * @return NONE
 */
void OTG_DeviceConnect(void);


/**
 * @brief  Device software disconnect.
 * @param  NONE
 * @return NONE
 */
void OTG_DeviceDisConnect(void);


/**
 * @brief  ����STALLӦ��
 * @param  EndpointNum �˵��
 * @return NONE
 */
void OTG_DeviceStallSend(uint8_t EndpointNum);


/**
 * @brief  ��λĳ���˵�
 * @param  EndpointNum �˵��
 * @param  EndpointType �˵�����
 * @return NONE
 */
void OTG_DeviceEndpointReset(uint8_t EndpointNum, uint8_t EndpointType);


/**
 * @brief  ����ĳ���˵㵥�δ��������ֽ���
 * @param  EndpointNum �˵��
 * @param  Endpoint PacketSize
 * @return NONE
 */
void OTG_DeviceEndpointPacketSizeSet(uint8_t EndpointNum, uint16_t PacketSize);


/**
 * @brief  �����豸��ַ
 * @brief  ���߸�λ�������豸��ַΪ0����������PC�˷���SetAddress��������ָ�����豸��ַ
 * @param  Address �豸��ַ
 * @return NONE
 */
void OTG_DeviceAddressSet(uint8_t Address);


/**
 * @brief  ��ȡ�����ϵ��¼�
 * @param  NONE
 * @return �����¼����룬USB_RESET-���߸�λ�¼���...
 */
uint8_t OTG_DeviceBusEventGet(void);


/**
 * @brief  �ӿ��ƶ˵㷢������
 * @param  Buf ���ݻ�����ָ��
 * @param  Len ���ݳ���
 * @param  TimeOut��ʱʱ�䣬ms
 * @return OTG_DEVICE_ERR_CODE
 */
OTG_DEVICE_ERR_CODE OTG_DeviceControlSend(uint8_t* Buf, uint32_t Len, uint32_t TimeOut);


/**
 * @brief  �ӿ��ƶ˵����OUT����
 * @param  Buf ���ݻ�����ָ��
 * @param  MaxLen ������ݳ���
 * @param  *pTransferLenʵ�ʽ��յ������ݳ���
 * @param  TimeOut��ʱʱ�䣬ms
 * @return OTG_DEVICE_ERR_CODE
 */
OTG_DEVICE_ERR_CODE OTG_DeviceControlReceive(uint8_t* Buf, uint32_t MaxLen, uint32_t *pTransferLen, uint32_t TimeOut);


/**
 * @brief  �ӿ��ƶ˵����SETUP����
 * @param  Buf ���ݻ�����ָ��
 * @param  MaxLen ������ݳ��ȣ�һ��SETUP���ݰ����ȶ���8�ֽ�
 * @param  *pTransferLenʵ�ʽ��յ������ݳ���
 * @return OTG_DEVICE_ERR_CODE
 */
OTG_DEVICE_ERR_CODE OTG_DeviceSetupReceive(uint8_t* Buf, uint32_t MaxLen ,uint32_t *pTransferLen);


/**
 * @brief  ��BULK IN�˵㷢������
 * @param  EpNum �˵��
 * @param  Buf ���ݻ�����ָ��
 * @param  Len ���ݳ���
 * @param  TimeOut��ʱʱ�䣬ms
 * @return OTG_DEVICE_ERR_CODE
 */
OTG_DEVICE_ERR_CODE OTG_DeviceBulkSend(uint8_t EpNum, uint8_t* Buf, uint32_t Len, uint32_t TimeOut);


/**
 * @brief  ��BULK OUT�˵��������
 * @param  EpNum �˵��
 * @param  Buf ���ݻ�����ָ��
 * @param  MaxLen ������ݳ���
 * @param  *pTransferLenʵ�ʽ��յ������ݳ���
 * @param  TimeOut��ʱʱ�䣬ms
 * @return OTG_DEVICE_ERR_CODE
 */
OTG_DEVICE_ERR_CODE OTG_DeviceBulkReceive(uint8_t EpNum, uint8_t* Buf, uint16_t MaxLen, uint32_t *pTransferLen ,uint32_t TimeOut);


/**
 * @brief  ��ISO IN�˵㷢������
 * @param  EpNum �˵��
 * @param  Buf ���ݻ�����ָ��
 * @param  Len ���ݳ��ȣ����ܴ���DEVICE_FS_ISO_IN_MPS�ֽ�
 * @return OTG_DEVICE_ERR_CODE
 */
OTG_DEVICE_ERR_CODE OTG_DeviceISOSend(uint8_t EpNum, uint8_t* Buf, uint32_t Len);


/**
 * @brief  ��ISO OUT�˵��������
 * @param  EpNum �˵��
 * @param  Buf ���ݻ�����ָ��
 * @param  MaxLen ������ݳ���
 * @param  *pTransferLenʵ�ʽ��յ������ݳ���
 * @return OTG_DEVICE_ERR_CODE
 */
OTG_DEVICE_ERR_CODE OTG_DeviceISOReceive(uint8_t EpNum, uint8_t* Buf, uint32_t MaxLen ,uint32_t *pTransferLen);


/**
 * @brief  ��INT IN�˵㷢������
 * @param  EpNum �˵��
 * @param  Buf ���ݻ�����ָ��
 * @param  Len ���ݳ��ȣ����ܴ���DEVICE_FS_BULK_IN_MPS�ֽ�
 * @param  TimeOut��ʱʱ�䣬ms
 * @return OTG_DEVICE_ERR_CODE
 */
OTG_DEVICE_ERR_CODE OTG_DeviceInterruptSend(uint8_t EpNum, uint8_t* Buf, uint32_t Len, uint32_t TimeOut);


/**
 * @brief  ��INT OUT�˵��������
 * @param  EpNum �˵��
 * @param  Buf ���ݻ�����ָ��
 * @param  MaxLen ������ݳ���
 * @param  *pTransferLenʵ�ʽ��յ������ݳ���
 * @param  TimeOut��ʱʱ�䣬ms
 * @return OTG_DEVICE_ERR_CODE
 */
OTG_DEVICE_ERR_CODE OTG_DeviceInterruptReceive(uint8_t EpNum, uint8_t* Buf, uint16_t MaxLen, uint32_t *pTransferLen ,uint32_t TimeOut);


/**
 * @brief  DEVICEģʽ��ʹ��ĳ���˵��ж�
 * @param  Pipe Pipeָ��
 * @param  Func �жϻص�����ָ��
 * @return NONE
 */
void OTG_EndpointInterruptEnable(uint8_t EpNum, FPCALLBACK Func);


/**
 * @brief  DEVICEģʽ�½�ֹĳ���˵��ж�
 * @param  EpNum �˵��
 * @return NONE
 */
void OTG_EndpointInterruptDisable(uint8_t EpNum);

/**
 * @brief  OTG PowerDown
 * @param  NONE
 * @return NONE
 */
void OTG_PowerDown(void);

/**
 * @brief  OTG Pause detect & Backup reg &Powerdown
 * @param  NONE
 * @return NONE
 */
void OTG_DeepSleepBackup(void);

/**
 * @brief  OTG resume Reg/PowerOn & Detect
 * @param  NONE
 * @return NONE
 */
void OTG_WakeupResume(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif //__OTG_DEVICE_HCD_H__
/**
 * @}
 * @}
 */
