/**
 **************************************************************************************
 * @file    uarts_interface.h
 * @brief   uarts_interface
 *
 * @author  Sam
 * @version V1.1.0
 *
 * $Created: 2018-06-05 15:17:05$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef __UARTS_INTERFACE_H__
#define __UARTS_INTERFACE_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 

#include "type.h"
#include "uarts.h"
#include "dma.h"

/*UART MCU mode*/
/**
 * @brief      ��ʼ�����ں���
 *             ���û��������������ʣ�����λ��У��λ��ֹͣλ����
 * @param[in]  UartNum    UARTӲ��ģ��ѡ��0:UART0 1:UART1
 * @param[in]  BaudRate   ���ڲ��������ã�DPLL@288MHz���� AUPLL@240MHzģʽ��:�����ʷ�Χ1200bps~3000000bps��RC@12M�����ʷ�Χ1200bps~115200bps
 * @param[in]  DataBits   ����λ����ѡ��֧��5-8bit�����������루5��6��7��8����
 * @param[in]  Parity     У��λѡ����������ֱ��Ӧ 0:��У�� 1:��У�� 2:żУ��
 * @param[in]  StopBits   ֹͣλѡ����������ֱ��Ӧ1��1λֹͣλ 2��2λֹͣλ
 * @return     			  FALSE��������	TRUE������ʼ�����
 */
#define UARTS_Init(UartNum,BaudRate,DataBits,Parity,StopBits)  UART_Init(UartNum,BaudRate,DataBits,Parity,StopBits)

/**
 * @brief      ���ڿ���λ��������
 *             ͨ��ѡ�񴮿ڿ���λCmd�Լ�����Ĳ���Arg������������û��߶�ȡ�������
 * @param[in]  UartNum  UARTӲ��ģ��ѡ��0:UART0 1:UART1
 * @param[in]  Cmd  	����ָ��ѡ��ѡ��UART_IOCTL_CMD_Tö����
 * @param[in]  Arg  	ָ�����������Cmdָ��д����Ҫ�Ĳ���
 * @return     �ڶԿ���ָ������ɹ���ǰ���£����������������н����
 * 				��UART_IOCTL_TXSTAT_GET�Ȼ�ȡ״̬��������״ֵ̬
 * 				��UART_IOCTL_TXINT_SET�����ÿ���λ��������0�������ɹ�
 * 			   ����EINVAL��22�����Ƿ�����������Cmd��Arg�����Ƿ���ȷ
 */
#define UARTS_IOCTL(UartNum,cmd,Arg)  UART_IOCtl(UartNum,cmd,Arg)

/**
 * @brief      ���ڶ��ֽڽ��պ���
 * @param[in]  UartNum      UARTӲ��ģ��ѡ��0:UART0 1:UART1
 * @param[in]  RecvBuf  	�������ݻ�������ַ
 * @param[in]  BufLen   	�������ݻ������ֽڳ���
 * @param[in]  TimeOut   	��ʱ�˳�ʱ��
 * @return     ���յ������ݳ���
 */
#define UARTS_Recv(UartNum, RecvBuf, BufLen,TimeOut) UART_Recv(UartNum, RecvBuf, BufLen,TimeOut)

/**
 * @brief      ���ڵ��ֽڽ��պ���
 * @param[in]  UartNum  UARTӲ��ģ��ѡ��0:UART0 1:UART1
 * @param[in]  Val  	�������ݴ�ŵ�ַ
 * @return     FALSE    δ���յ�����	TRUE���յ�����
 */
#define UARTS_RecvByte(UartNum, Val) UART_RecvByte(UartNum, Val)

/**
 * @brief      ���ڶ��ֽڷ��ͺ���
 * @param[in]  UartNum      UARTӲ��ģ��ѡ��0:UART0 1:UART1
 * @param[in]  SendBuf  	�������ݻ�������ַ
 * @param[in]  BufLen   	���������ֽڳ���
 * @param[in]  TimeOut   	��ʱ�˳�ʱ��
 * @return     ���ͳ���
 */
uint32_t UARTS_Send(UART_PORT_T UART_MODULE,uint8_t* SendBuf, uint32_t BufLen,uint32_t TimeOut);

/**
 * @brief      ���ڵ��ֽڷ��ͺ���
 * @param[in]  UartNum      UARTӲ��ģ��ѡ��0:UART0 1:UART1
 * @param[in]  SendByte  	�跢�͵�����
 * @return     �޷���ֵ
 */
void UARTS_SendByte(UART_PORT_T UART_MODULE,uint8_t SendByte);

/**
 * @brief      ���ڵ��ֽڷ���
 *             ע�����ͺ���Ҫ�ȴ������Ƿ���ɣ�����������ɺ󣬻ᷢ��������жϡ������жϺ�������Ҫע����TX DONE�жϣ�
 * @param[in]  UartNum      UARTӲ��ģ��ѡ��0:UART0 1:UART1
 * @param[in]  SendByte  	�跢�͵�����
 * @return     �޷���ֵ
 */
void UARTS_SendByte_In_Interrupt(UART_PORT_T UART_MODULE,uint8_t SendByte);

/*UART DMA mode*/

/**
 * @brief      UART DMA ����ģʽ��ʼ�������û������������ջ����ַ�����ջ��泤�ȣ��жϴ����ż�ֵ���ص�����
 * @param[in]  RxBufAddr  	���ջ�������ַ
 * @param[in]  RxBufLen   	���ջ�������������
 * @param[in]  ThresholdLen �жϴ����ż�ֵ���ã�ע������ֵС��RxBufLen
 * @param[in]  CallBack   	�жϻص�����
 * @return     FALSE��ʼ������ʧ��	TRUE��ʼ�����óɹ�
 */
bool UARTS_DMA_RxInit(UART_PORT_T UART_MODULE,void* RxBufAddr, uint16_t RxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack);

/**
 * @brief      UART DMA ����ģʽ��ʼ�������û������������ͻ����ַ�����ͻ��泤�ȣ��жϴ����ż�ֵ���ص�����
 * @param[in]  TxBufAddr  	���ͻ�������ַ
 * @param[in]  TxBufLen   	���ͻ�������������
 * @param[in]  ThresholdLen �жϴ����ż�ֵ���ã�ע������ֵС��RxBufLen
 * @param[in]  CallBack   	�жϻص�����
 * @return     FALSE��ʼ������ʧ��	TRUE��ʼ�����óɹ�
 */
bool UARTS_DMA_TxInit(UART_PORT_T UART_MODULE,void* TxBufAddr, uint16_t TxBufLen, uint16_t ThresholdLen, INT_FUNC CallBack);

/**
 * @brief      UART DMA ����ģʽ��ʼ�������ͻ�������ַ�����ͻ��泤�ȣ��жϴ����ż�ֵ���ص�����
 * @param[in]  TxBufAddr  	���ͻ�������ַ
 * @param[in]  TxBufLen   	�����ַ�����
 * @param[in]  TimeOut 		��ʱʱ�����ã����ﵽ���õĳ�ʱʱ�仹δ������ɣ����������
 * @return     ʵ�ʷ������ݳ���
 */
uint32_t UARTS_DMA_Send(UART_PORT_T UART_MODULE,uint8_t* SendBuf, uint16_t BufLen, uint32_t TimeOut);

/**
 * @brief      UART DMA �������ݣ����ý��ջ����ַ�������ַ����ȣ����ճ�ʱ����
 * @param[in]  RecvBuf  	���ջ�������ַ
 * @param[in]  BufLen   	���ճ�������
 * @param[in]  TimeOut		��ʱʱ�����ã��涨ʱ���ڽ���û�дﵽԤ�ڳ��Ȼ���û�յ��վ����������
 * @return     ���յ����ݵ�ʵ�ʳ���
 */
uint32_t UARTS_DMA_Recv(UART_PORT_T UART_MODULE,uint8_t* RecvBuf, uint16_t BufLen, uint32_t TimeOut);

/**
 * @brief      UART DMA �������ݣ����ô��봫�ͻ�������ַ�������ַ�����
 * @param[in]  RecvBuf  	���ͻ�������ַ
 * @param[in]  BufLen   	�����ַ���������
 * @return     �޷���ֵ
 */
void UARTS_DMA_SendDataStart(UART_PORT_T UART_MODULE,uint8_t* SendBuf, uint16_t BufLen);

/**
 * @brief      �ж����ݴ����Ƿ����
 * @return     TURE������� 	 FALSE����δ���
  */
bool UARTS_DMA_TxIsTransferDone(UART_PORT_T UART_MODULE);

/**
 * @brief      UART DMA 	��������ʽʹ�ܽ��գ����ý��ջ����ַ�������ַ�����
 * @param[in]  RecvBuf  	���ջ�������ַ
 * @param[in]  BufLen   	���ճ�������
 * @return     ���յ����ݵ�ʵ�ʳ���
 */
int32_t UARTS_DMA_RecvDataStart(UART_PORT_T UART_MODULE,uint8_t* RecvBuf, uint16_t BufLen);

/**
 * @brief      �ж��Ƿ������
 * @return     TURE������� 	 FALSE����δ���
  */
bool UARTS_DMA_RxIsTransferDone(UART_PORT_T UART_MODULE);

/**
 * @brief      ��ѯDMA���ջ��������ݳ���
 * @return     �������ݳ���
  */
int32_t UARTS_DMA_RxDataLen(UART_PORT_T UART_MODULE);

/**
 * @brief      ��ѯDMA���ͻ��������ݳ���
 * @return     �������ݳ���
  */
int32_t UARTS_DMA_TxDataLen(UART_PORT_T UART_MODULE);

/**
 * @brief      ע��UART_DMAģʽ�µ��жϻص�������ע�������ж�����
 * @param[in]  IntType  	�ж���������: 1.DMA_DONE_INT���ж�    2.DMA_THRESHOLD_INT��ֵ�����ж�    3.DMA_ERROR_INT�����ж�
 * param[in]   CallBack		��ע����жϻص���������д��NULL����رջص�����
 * @return     �������ݳ���
  */
void UARTS_DMA_RxSetInterruptFun(UART_PORT_T UART_MODULE,DMA_INT_TYPE IntType, INT_FUNC CallBack);
void UARTS_DMA_TxSetInterruptFun(UART_PORT_T UART_MODULE,DMA_INT_TYPE IntType, INT_FUNC CallBack);

/**
 * @brief      UART_DMAʹ�ܺ���
 * @param[in]
 * @return
  */
void UARTS_DMA_RxChannelEn(UART_PORT_T UART_MODULE);
void UARTS_DMA_TxChannelEn(UART_PORT_T UART_MODULE);

/**
 * @brief      UART_DMA���ú���
 * @param[in]
 * @return
  */
void UARTS_DMA_RxChannelDisable(UART_PORT_T UART_MODULE);
void UARTS_DMA_TxChannelDisable(UART_PORT_T UART_MODULE);

/**
 * @brief      UART_DMA �жϱ�־λ���
 * @param[in]   IntType  	�ж���������	DMA_DONE_INT���ж�DMA_THRESHOLD_INT��ֵ�����ж�DMA_ERROR_INT�����ж�
 * @return  	DMA_ERROR
 * @note	�жϱ�־�ڲ������жϵ������Ҳ����λ
  */
int32_t UARTS_DMA_TxIntFlgClr(UART_PORT_T UART_MODULE,DMA_INT_TYPE IntType);
//RX flag clear
int32_t UARTS_DMA_RxIntFlgClr(UART_PORT_T UART_MODULE,DMA_INT_TYPE IntType);
//add new APIs here...

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif//__UARTS_INTERFACE_H__
