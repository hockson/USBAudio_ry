
#include "i2c_host.h"
#include "mygpio.h"

/*************************************************
 *
 * 此处代码将实现GPIO的功能控制，包括初始化，设置引脚电平和读取引脚电平。
 *
 *************************************************/
 
static uint32_t get_gpio_pin(uint8_t gpio)
{
    uint8_t pin_t = gpio % PINS_IN_GROUP;

    return (uint32_t)1<<pin_t;
}

static int32_t get_gpio_group(uint8_t gpio, GPIO_TypeDef **GPIOx)
{
    uint16_t group = 0;
    int32_t ret = SUCCESS;

    if (gpio >= GPIO_max) {
        return FAIL;
    }

    group = gpio / PINS_IN_GROUP;

    switch (group) {
    case GROUP_GPIOA:
        *GPIOx = GPIOA;
        break;
    case GROUP_GPIOB:
        *GPIOx = GPIOB;
        break;
    default:
        ret = FAIL;
        break;
    }

    return ret;
}


int32_t hal_gpio_init(uint8_t gpio, GPIOMode_TypeDef gpiomode)
{
    int32_t ret = SUCCESS;
	uint32_t gpio_pin;
    GPIO_TypeDef *GPIOx = NULL;

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == FAIL){
        return ret;
    }
    gpio_pin = get_gpio_pin(gpio);

    switch (gpiomode) {
    case GPIO_MODE_AIN:
        bit_set(GPIOx->AIN, gpio_pin);
        break;
    case GPIO_MODE_IN:
        bit_clr(GPIOx->OE, gpio_pin);
        bit_set(GPIOx->IE, gpio_pin);
        bit_clr(GPIOx->PU, gpio_pin);
        bit_clr(GPIOx->PD, gpio_pin);
        break;
    case GPIO_MODE_IN_PD:
        bit_clr(GPIOx->OE, gpio_pin);
        bit_set(GPIOx->IE, gpio_pin);
        bit_clr(GPIOx->PU, gpio_pin);
        bit_set(GPIOx->PD, gpio_pin);

        break;
    case GPIO_MODE_IN_PU:
        bit_clr(GPIOx->OE, gpio_pin);
        bit_set(GPIOx->IE, gpio_pin);
        bit_set(GPIOx->PU, gpio_pin);
        bit_clr(GPIOx->PD, gpio_pin);
        break;
    case GPIO_MODE_IN_PUPD:
        bit_clr(GPIOx->OE, gpio_pin);
        bit_set(GPIOx->IE, gpio_pin);
        bit_set(GPIOx->PU, gpio_pin);
        bit_set(GPIOx->PD, gpio_pin);
        break;
    case GPIO_MODE_OUT_OD:
        bit_set(GPIOx->OE, gpio_pin);
        bit_clr(GPIOx->IE, gpio_pin);
        bit_clr(GPIOx->PU, gpio_pin);
        bit_clr(GPIOx->PD, gpio_pin);
        break;
    case GPIO_MODE_OUT_PU:
        bit_set(GPIOx->OE, gpio_pin);
        bit_clr(GPIOx->IE, gpio_pin);
        bit_set(GPIOx->PU, gpio_pin);
        bit_clr(GPIOx->PD, gpio_pin);
        break;
    case GPIO_MODE_OUT_PD:
        bit_set(GPIOx->OE, gpio_pin);
        bit_clr(GPIOx->IE, gpio_pin);
        bit_clr(GPIOx->PU, gpio_pin);
        bit_set(GPIOx->PD, gpio_pin);
        break;
    default:
        ret = FAIL;
        break;
    }

    return ret;
}

int32_t hal_gpio_out_set(uint8_t gpio)
{
	uint32_t gpio_pin;
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = SUCCESS;

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == SUCCESS) {
        gpio_pin = get_gpio_pin(gpio);
        //bit_set(GPIOx->SET, gpio_pin);
        GPIOx->SET = gpio_pin;
    };

    return ret;
}

int32_t hal_gpio_out_clr(uint8_t gpio)
{
	uint32_t gpio_pin;
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = SUCCESS;

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == SUCCESS) {
        gpio_pin = get_gpio_pin(gpio);
        // bit_set(GPIOx->CLR, gpio_pin);
        GPIOx->CLR = gpio_pin;
    };

    return ret;
}

int32_t hal_gpio_out_toggle(uint8_t gpio)
{
	uint32_t gpio_pin;
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = SUCCESS;

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == SUCCESS) {
        gpio_pin = get_gpio_pin(gpio);
        // bit_set(GPIOx->TOG, gpio_pin);
        GPIOx->TOG = gpio_pin;
    };

    return ret;
}

int32_t hal_gpio_input_get(uint8_t gpio)
{
	uint32_t gpio_pin;
    GPIO_TypeDef* GPIOx = NULL;
    int32_t ret = SUCCESS;

    ret = get_gpio_group(gpio, &GPIOx);
    if (ret == SUCCESS) {
        gpio_pin = get_gpio_pin(gpio);
        return (bit_istrue(GPIOx->IN, gpio_pin));
    };

    return FAIL;
}


void* Host_i2c_Init(void)
{
    GPIO_PortAModeSet((1<<5), 0x0);
    GPIO_PortAModeSet((1<<4), 0x0);
    return (I2cMasterCreate(PORT_B, 5, PORT_B, 4));
}



