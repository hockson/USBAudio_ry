/**
  *****************************************************************************
  * @file:			spim.c
  * @author			Lilu
  * @version		V1.0.0
  * @data			2014-11-20
  * @Brief			SPI Master driver interface
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
  * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
  */

/**
 * @addtogroup SPIM
 * @{
 * @defgroup spim spim.h
 * @{
 */
 
 
#ifndef __SPIM_H__
#define __SPIM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


//SPIMʱ��=PLL_CLK/2/(REG_SPIM_CLK_DIV_NUM+1)/2*(SREG_SPIM_CTRL.CRAT+1)     REG_SPIM_CLK_DIV_NUMĬ�ϵ���2
/*ע�⣺�˴��궨���������PLLʱ��Ϊ288M�����*/
//Max 30Mhz
#define	SPIM_CLK_DIV_24M	0x0			/**< SPI master clock Div 30MHz*/
#define	SPIM_CLK_DIV_12M	0x1			/**< SPI master clock Div 15MHz*/
#define	SPIM_CLK_DIV_6M 	0x2			/**< SPI master clock Div 7.5MHz*/
#define	SPIM_CLK_DIV_3M     0x3			/**< SPI master clock Div 3.75MHz*/
#define	SPIM_CLK_DIV_1M5	0x4			/**< SPI master clock Div 1.875MHz*/
#define	SPIM_CLK_DIV_750K	0x5			/**< SPI master clock Div 937.5KHz*/









typedef enum
{
	SPIM_PORT0_A5_A6_A7_A9			= 0,
	SPIM_PORT1_A20_A21_A22_A28		= 1,
}SPIM_PORT_MODE;




/**
 * err code define
 */
typedef enum _SPI_MASTER_ERR_CODE
{
    ERR_SPIM_TIME_OUT = -255,			/**<function execute time out*/
    ERR_SPIM_DATALEN_OUT_OF_RANGE,		/**<data len is out of range < 0*/
    SPIM_NONE_ERR = 0,
} SPI_MASTER_ERR_CODE;





/**
 * @brief
 *		��ʼ��SPIMģ��
 * @param	Mode
 *				0 - CPOL = 0 & CPHA = 0, idleΪ�͵�ƽ,��һ�����ز���(������)
 *				1 - CPOL = 0 & CPHA = 1, idleΪ�͵�ƽ,�ڶ������ز���(�½���)
 *				2 - CPOL = 1 & CPHA = 0, idleΪ�ߵ�ƽ,��һ�����ز���(�½���)
 *				3 - CPOL = 1 & CPHA = 1, idleΪ�ߵ�ƽ,��һ�����ز���(������)
 * @param	ClkDiv 	��Ƶϵ��:0 ~ 15
 * 				spi clock = system clock / (2 * (ClkDiv + 1)).
 * @return
 *		��
 * @note
 */
bool SPIM_Init(uint8_t Mode, uint8_t ClkDiv);

/**
 * @brief
 *		ѡ��SPIMģ�鹦������
 * @param	PortSel    SPIM_PORT0_A5_A6_A7_A9   SPIM_PORT1_A20_A21_A22_A28
 * @return
 *		��
 * @note
 */
SPI_MASTER_ERR_CODE SPIM_IoConfig(uint8_t PortSel);

//--------------------------------MCU MODE-------------------------------------
/**
 * @brief
 *		MCUģʽ��������
 * @param	SendBuf		���������׵�ַ
 * @param	Length		�������ݳ��ȣ���λ��Byte��
 * @return
 *		��
 * @note
 */
void SPIM_Send(uint8_t* SendBuf, uint32_t Length);

/**
 * @brief
 *		MCUģʽ��������
 * @param	RecvBuf		���������׵�ַ
 * @param	Length		�������ݳ��ȣ���λ��Byte��
 * @return
 *		��
 * @note
 */
void SPIM_Recv(uint8_t* RecvBuf, uint32_t Length);



//--------------------------------DMA MODE-------------------------------------
/**
 * @brief
 *		����DMAģʽ�������ݣ�������
 * @param	Length	���ݳ��ȣ���λ��Byte�������65536 byte
 * 			TXEN    DMA�������ݿ���
 * 			RXEN    DMA�������ݿ���
 * @return
 *		��
 * @Note SPIM�жϱ�־��λ��־�շ�������ɣ���ʹ�ò�ѯ���жϷ�ʽ
 */
void SPIM_DMAStart(uint32_t Length,bool TXEN,bool RXEN);



//--------------------------------DMA CTL-------------------------------------

/**
 * @brief
 *		�ж�ʹ��
 * @param	��
 * @return
 *		��
 */
void SPIM_SetIntEn(bool EN);
/**
 * @brief
 *		����жϱ�־
 * @param	��
 * @return
 *		��
 */
void SPIM_ClrIntFlag(void);
/**
 * @brief
 *		�������ݷ���ģʽ
 * @param	EN ��Ĭ��Ϊ1��
 *      EN=0:���ݵ�λ�ȴ���     EN=1�����ݸ�λ�ȴ���
 * @return
 *		��
 */
void SPIM_SetMSB(bool EN);
/**
 * @brief
 *		�������״̬��־
 * @param	��
 * @return
 *		��
 */
void SPIM_ClrAllStatus();
/**
 * @brief
 *		����SPIM���ݴ���DMA��ʽʹ��
 * @param	EN��  EN=0��DMA����      EN=1��DMA����
 * @return
 *		��
 */
void SPIM_SetDmaEn(bool EN);
/**
 * @brief
 *		 specify spi transfer mode, default set standard mode
 * @param	PortSel    	SPIM_PORT0_A5_A6_A7   SPIM_PORT1_A20_A21_A22
 * 	 		mode		0: standard mode  1: quad mode
 * @return
 *		��
 * @note
 */
void SPIM_SetMode(uint8_t PortSel, uint8_t mode);

void SPIM_ClrOverwr();
//--------------------------------DMA STATUS-------------------------------------
/**
 * @brief
 *		��ȡ�жϱ�־
 * @param	��
 * @return
 *		�����жϱ�־
 */
bool SPIM_GetIntFlag(void);
/**
 * @brief
 *		��ȡ���ߵ�ǰ״̬
 * @param	��
 * @return
 *		�������ߵ�ǰ״̬
 */
bool SPIM_GetBusy();
/**
 * @brief
 *		��ȡMSB����
 * @param	��
 * @return
 *		����MSB����λ��ǰ����
 */
bool SPIM_GetMsb();
/**
 * @brief
 *		��ȡ����Fifoд�����־
 * @param	��
 * @return
 *		���� 0�������  1�����������
 */
bool SPIM_GetTxOverwr();
/**
 * @brief
 *		��ȡ����FIFOд�����־
 * @param	��
 * @return
 *		���� 0�������  1�����������
 */
bool SPIM_GetRxOverwr();
/**
 * @brief
 *		��ȡSPIM Mode��־
 * @param	��
 * @return
 *		���� 0��standard mode  1��quad mode
 */
bool SPIM_GetMode();
/**
 * @brief
 *		Init default CS pin
 * @param	PortSel    SPIM_PORT0_A5_A6_A7_A9   SPIM_PORT1_A20_A21_A22_A28
 * @return	none
 *
 */
SPI_MASTER_ERR_CODE SPIM_CS_Init(uint8_t PortSel);
/**
 * @brief
 *		Enable default CS pin
 * @param	none
 * @return	none
 *
 */
void SPIM_CS_Enable();
/**
 * @brief
 *		Disable default CS pin
 * @param	none
 * @return	none
 *
 */
void SPIM_CS_Disable();

#ifdef __cplusplus
}
#endif//__cplusplus



#endif

/**
 * @}
 * @}
 */
