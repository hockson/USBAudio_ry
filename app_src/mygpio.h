#ifndef __HW_GPIO_H__
#define __HW_GPIO_H__	
//#include "gpio.h"

////******************************  gpio macro *****************************
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */

/** 
  * @brief General Purpose I/O
  */

typedef struct
{
  __IO uint32_t IN;
  __IO uint32_t OUT;
  __IO uint32_t SET;
  __IO uint32_t CLR;
  __IO uint32_t TOG;
  __IO uint32_t IE;
  __IO uint32_t OE;
  __IO uint32_t AIN;
  __IO uint32_t PU;
  __IO uint32_t PD;
  __IO uint32_t CMSK;   // CORE MASK
  __IO uint32_t DMSK;   // DMA MASK
} GPIO_TypeDef;
																	    

/** 
  * @brief  Configuration Mode enumeration  
  */

typedef enum
{ GPIO_MODE_AIN = 0x0,
  GPIO_MODE_IN,
  GPIO_MODE_IN_PD,
  GPIO_MODE_IN_PU,
  GPIO_MODE_IN_PUPD,
  GPIO_MODE_OUT_OD,
  GPIO_MODE_OUT_PU,
  GPIO_MODE_OUT_PD,
}GPIOMode_TypeDef;


typedef enum __GPIO_PIN__{
	// 1 : gpioa group
	GPIO0 = 0,                        /**<GPIO A0 macro*/
	GPIO1 = 1,                        /**<GPIO A1 macro*/
	GPIO2 = 2,                        /**<GPIO A2 macro*/
	GPIO3 = 3,                        /**<GPIO A3 macro*/
	GPIO4 = 4,                        /**<GPIO A4 macro*/
	GPIO5 = 5,                        /**<GPIO A5 macro*/
	GPIO6 = 6,                        /**<GPIO A6 macro*/
	GPIO7 = 7,                        /**<GPIO A7 macro*/
	GPIO8 = 8,                        /**<GPIO A8 macro*/
	GPIO9 = 9,                        /**<GPIO A9 macro*/
	GPIO10 = 10,                      /**<GPIO A10 macro*/
	GPIO11 = 11,                      /**<GPIO A11 macro*/
	GPIO12 = 12,                      /**<GPIO A12 macro*/
	GPIO13 = 13,                      /**<GPIO A13 macro*/
	GPIO14 = 14,                      /**<GPIO A14 macro*/
	GPIO15 = 15,                      /**<GPIO A15 macro*/
	GPIO16 = 16,                      /**<GPIO A16 macro*/
	GPIO17 = 17,                      /**<GPIO A17 macro*/
	GPIO18 = 18,                      /**<GPIO A18 macro*/
	GPIO19 = 19,                      /**<GPIO A19 macro*/
	GPIO20 = 20,                      /**<GPIO A20 macro*/
	GPIO21 = 21,                      /**<GPIO A21 macro*/
	GPIO22 = 22,                      /**<GPIO A22 macro*/
	GPIO23 = 23,                      /**<GPIO A23 macro*/
	GPIO24 = 24,                      /**<GPIO A24 macro*/
	GPIO25 = 25,                      /**<GPIO A25 macro*/
	GPIO26 = 26,                      /**<GPIO A26 macro*/
	GPIO27 = 27,                      /**<GPIO A27 macro*/
	GPIO28 = 28,                      /**<GPIO A28 macro*/
	GPIO29 = 29,                      /**<GPIO A29 macro*/
	GPIO30 = 30,                      /**<GPIO A30 macro*/
	GPIO31 = 31,                      /**<GPIO A31 macro*/

	// 2 : gpiob group
	GPIO32 = 32,                      /**<GPIO B0 macro*/
	GPIO33 = 33,					  /**<GPIO B1 macro*/
	GPIO34 = 34,					  /**<GPIO B2 macro*/
	GPIO35 = 35,					  /**<GPIO B3 macro*/
	GPIO36 = 36,					  /**<GPIO B4 macro*/
	GPIO37 = 37,					  /**<GPIO B5 macro*/
	GPIO38 = 38,					  /**<GPIO B6 macro*/
	GPIO39 = 39,					  /**<GPIO B7 macro*/
	GPIO_max = 40,					  /**<GPIO NONE macro*/	
}GPIO_PIN;


//IO¿ÚµØÖ·Ó³Éä
#define GPIOA_BASE          (0x40010000)
#define GPIOB_BASE          (0x40010030)

#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)

//#define bit_set_atomic(x,mask)      {  DISABLE_INT();(x) |= (mask);  ENABLE_INT();}	  
//#define bit_clr_atomic(x,mask)      {  DISABLE_INT();(x) &= ~(mask); ENABLE_INT();}	  
//#define bit_tog_atomic(x,mask)      {  DISABLE_INT();(x) ^= (mask);  ENABLE_INT();}	  
#define bit_set(x,mask)		        (x) |= (mask)   
#define bit_clr(x,mask)		        (x) &= ~(mask) 
#define bit_tog(x,mask)		        (x) ^= (mask)  
#define bit_istrue(x,mask)		    ((x & mask) != 0)   
#define bit_isfalse(x,mask) 	    ((x & mask) == 0)   

#ifndef SUCCESS
#define	SUCCESS				0
#define	FAIL				-1
#endif

#define PINS_IN_GROUP  32
#define GROUP_GPIOA    0
#define GROUP_GPIOB    1

#define BIT_RESET   0
#define BIT_SET     1


int32_t hal_gpio_init(uint8_t gpio, GPIOMode_TypeDef gpiomode);
int32_t hal_gpio_out_set(uint8_t gpio);
int32_t hal_gpio_out_clr(uint8_t gpio);
int32_t hal_gpio_out_toggle(uint8_t gpio);
int32_t hal_gpio_input_get(uint8_t gpio);
void* Host_i2c_Init(void);

#endif
