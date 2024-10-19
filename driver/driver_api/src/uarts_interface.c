/**
 **************************************************************************************
 * @file    uart_interface.c
 * @brief   uart interface
 *
 * @author  Messi
 * @version V1.0.0
 *
 * $Created: 2015-10-28 16:15:00$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
 
#include "type.h"
#include "uarts.h"
#include "gpio.h"
#include "dma.h"
#include "timeout.h"
#include "uarts_interface.h"

#ifdef FUNC_OS_EN
#include "rtos_api.h"

osMutexId UART0Mutex = NULL;
osMutexId UART1Mutex = NULL;
#endif
//#define OS_RELATIVE//only test

//��������APIs�İ�װ����������OS����

uint32_t UARTS_Send(UART_PORT_T UART_MODULE,uint8_t* SendBuf, uint32_t BufLen,uint32_t TimeOut)
{
	uint32_t Len;
#ifdef FUNC_OS_EN
	osMutexLock(UART0Mutex);
#endif
	Len =  UART_Send(UART_MODULE,SendBuf, BufLen,TimeOut);
#ifdef FUNC_OS_EN
	osMutexUnlock(UART0Mutex);
#endif
	return Len;
}

void UARTS_SendByte(UART_PORT_T UART_MODULE,uint8_t SendByte)
{
#ifdef FUNC_OS_EN
	osMutexLock(UART0Mutex);
#endif

	UART_SendByte(UART_MODULE,SendByte);

#ifdef FUNC_OS_EN
	osMutexUnlock(UART0Mutex);
#endif
}

void UARTS_SendByte_In_Interrupt(UART_PORT_T UART_MODULE,uint8_t SendByte)
{
#ifdef FUNC_OS_EN
	osMutexLock(UART0Mutex);
#endif

	UART_SendByte_In_Interrupt(UART_MODULE,SendByte);

#ifdef FUNC_OS_EN
	osMutexUnlock(UART0Mutex);
#endif
}

//Ŀǰȷ��UART-DMA����Circular��ʽ���ͺͽ�������
//���շ�ʽ��ʼ��
bool UARTS_DMA_RxInit(UART_PORT_T UART_MODULE,void* RxBufAddr, uint16_t RxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_RX;
	else
		dma_id = PERIPHERAL_ID_UART1_RX;
    if(DMA_CircularConfig(dma_id, 0, RxBufAddr, RxBufLen) != DMA_OK)
    {
        return FALSE;
    }
	
    DMA_CircularThresholdLenSet(dma_id, ThresholdLen);
	DMA_InterruptFunSet(dma_id, DMA_THRESHOLD_INT, CallBack);
	DMA_InterruptEnable(dma_id, DMA_THRESHOLD_INT, 1);
    DMA_ChannelEnable(dma_id);
	UART_IOCtl(UART_MODULE, UART_IOCTL_DMA_RX_EN, 1);
    return TRUE;
}

//blockģʽ���ͷ�ʽ��ʼ��
bool UARTS_DMA_BlockTxInit(UART_PORT_T UART_MODULE,INT_FUNC CallBack)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;

	if(DMA_BlockConfig(dma_id)!=DMA_OK)
	{
		return FALSE;
	}
	DMA_InterruptFunSet(dma_id, DMA_DONE_INT, CallBack);
	DMA_InterruptEnable(dma_id, DMA_DONE_INT, 1);
    DMA_ChannelEnable(dma_id);
    UART_IOCtl(UART_MODULE, UART_IOCTL_DMA_TX_EN, 1);
    return TRUE;
}


uint32_t UARTS_DMA_BlockSend(UART_PORT_T UART_MODULE,uint8_t* SendBuf, uint16_t BufLen)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;

	DMA_BlockBufSet(dma_id,SendBuf,BufLen);
	DMA_ChannelEnable(dma_id);
	return 0;
}

//���ͷ�ʽ��ʼ��
bool UARTS_DMA_TxInit(UART_PORT_T UART_MODULE,void* TxBufAddr, uint16_t TxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;
    if(DMA_CircularConfig(dma_id, 0, TxBufAddr, TxBufLen) != DMA_OK)
    {
        return FALSE;
    }
    DMA_CircularThresholdLenSet(dma_id, ThresholdLen);
    DMA_InterruptFunSet(dma_id, DMA_THRESHOLD_INT, CallBack);
	//DMA_InterruptEnable(dma_id, DMA_THRESHOLD_INT, 1);
    DMA_ChannelEnable(dma_id);
    UART_IOCtl(UART_MODULE, UART_IOCTL_DMA_TX_EN, 1);
    return TRUE;
}

uint32_t UARTS_DMA_Send(UART_PORT_T UART_MODULE,uint8_t* SendBuf, uint16_t BufLen, uint32_t TimeOut)
{
    TIMER TimerRx;
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;
	TimeOutSet(&TimerRx, TimeOut);
    DMA_CircularFIFOClear(dma_id);//���ϴη�������û����ɣ����ڴ˴������
    DMA_CircularDataPut(dma_id, SendBuf, BufLen);
    while(DMA_CircularDataLenGet(dma_id) && !IsTimeOut(&TimerRx));
    while(!UART_IOCtl(UART_MODULE,UART_IOCTL_TX_FIFO_EMPTY, 0));
    return BufLen;
}

uint32_t UARTS_DMA_Recv(UART_PORT_T UART_MODULE,uint8_t* RecvBuf, uint16_t BufLen, uint32_t TimeOut)
{
    TIMER TimerRx;
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_RX;
	else
		dma_id = PERIPHERAL_ID_UART1_RX;
	TimeOutSet(&TimerRx, TimeOut);
    while(( DMA_CircularDataLenGet(dma_id) < BufLen) && !IsTimeOut(&TimerRx));
    return DMA_CircularDataGet(dma_id, RecvBuf, BufLen);
}

//��������ʽ��������(��ѯ�Ƿ���ɵĽӿں���)
//׼�����ݣ�ʹ�ܷ���
void UARTS_DMA_SendDataStart(UART_PORT_T UART_MODULE,uint8_t* SendBuf, uint16_t BufLen)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;
    DMA_CircularFIFOClear(dma_id);//���ϴη�������û����ɣ����ڴ˴������
    DMA_CircularDataPut(dma_id, SendBuf, BufLen);
}

//�ж��Ƿ������
bool UARTS_DMA_TxIsTransferDone(UART_PORT_T UART_MODULE)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;
	/*DMA BUFFER--->UART FIFO--->*/
	if(DMA_CircularDataLenGet(dma_id) == 0 && UART_IOCtl(UART_MODULE,UART_IOCTL_TX_FIFO_EMPTY, 0))
		return TRUE;
	else
		return FALSE;
}

//��������ʽ��������(��ѯ�Ƿ���ɵĽӿں���)
//׼��������
int32_t UARTS_DMA_RecvDataStart(UART_PORT_T UART_MODULE,uint8_t* RecvBuf, uint16_t BufLen)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_RX;
	else
		dma_id = PERIPHERAL_ID_UART1_RX;

	return DMA_CircularDataGet(dma_id, RecvBuf, BufLen);
}

//�ж��Ƿ������
bool UARTS_DMA_RxIsTransferDone(UART_PORT_T UART_MODULE)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_RX;
	else
		dma_id = PERIPHERAL_ID_UART1_RX;
	/*UART FIFO--->DMA BUFFER--->*/
	if(DMA_CircularDataLenGet(dma_id) == 0 && UART_IOCtl(UART_MODULE,UART_IOCTL_RX_FIFO_EMPTY, 0))
		return TRUE;
	else
		return FALSE;
}

//��黺�������ݳ���
int32_t UARTS_DMA_RxDataLen(UART_PORT_T UART_MODULE)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_RX;
	else
		dma_id = PERIPHERAL_ID_UART1_RX;
	return DMA_CircularDataLenGet(dma_id);
}

int32_t UARTS_DMA_TxDataLen(UART_PORT_T UART_MODULE)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;
	return DMA_CircularDataLenGet(dma_id);
}

//DMA�жϷ�ʽ�ص�ע�ἰ�ر�
void UARTS_DMA_RxSetInterruptFun(UART_PORT_T UART_MODULE,DMA_INT_TYPE IntType, INT_FUNC CallBack)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_RX;
	else
		dma_id = PERIPHERAL_ID_UART1_RX;
	if(CallBack)
	{
		DMA_InterruptFunSet(dma_id, IntType, CallBack);
		DMA_InterruptEnable(dma_id, IntType, 1);
	}
	else
	{
		DMA_InterruptEnable(dma_id, IntType, 0);
	}
}

void UARTS_DMA_TxSetInterruptFun(UART_PORT_T UART_MODULE,DMA_INT_TYPE IntType, INT_FUNC CallBack)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;
	if(CallBack)
	{
		DMA_InterruptFunSet(dma_id, IntType, CallBack);
		DMA_InterruptEnable(dma_id, IntType, 1);
	}
	else
	{
		DMA_InterruptEnable(dma_id, IntType, 0);
	}
}

//DMA ͨ��ʹ�ܣ�����DMA��ʼ/ֹͣ����
void UARTS_DMA_RxChannelEn(UART_PORT_T UART_MODULE)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_RX;
	else
		dma_id = PERIPHERAL_ID_UART1_RX;
	DMA_ChannelEnable(dma_id);
}

void UARTS_DMA_TxChannelEn(UART_PORT_T UART_MODULE)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;
	DMA_ChannelEnable(dma_id);
}


void UARTS_DMA_RxChannelDisable(UART_PORT_T UART_MODULE)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_RX;
	else
		dma_id = PERIPHERAL_ID_UART1_RX;
	DMA_ChannelDisable(dma_id);
}


void UARTS_DMA_TxChannelDisable(UART_PORT_T UART_MODULE)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;
	DMA_ChannelDisable(dma_id);
}


//DMA �жϱ�־λ���
//TX flag clear
int32_t UARTS_DMA_TxIntFlgClr(UART_PORT_T UART_MODULE,DMA_INT_TYPE IntType)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_TX;
	else
		dma_id = PERIPHERAL_ID_UART1_TX;
	return DMA_InterruptFlagClear(dma_id, IntType);
}


//RX flag clear
int32_t UARTS_DMA_RxIntFlgClr(UART_PORT_T UART_MODULE,DMA_INT_TYPE IntType)
{
	DMA_PERIPHERAL_ID dma_id;
	if(UART_MODULE == UART_PORT0)
		dma_id = PERIPHERAL_ID_UART0_RX;
	else
		dma_id = PERIPHERAL_ID_UART1_RX;
	return DMA_InterruptFlagClear(dma_id, IntType);
}


