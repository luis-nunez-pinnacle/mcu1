/*
 * stm32f401xx.h
 *
 *  Created on: Oct 7, 2026
 *      Author: lenunez
 */

#ifndef INC_STM32F401XX_H_
#define INC_STM32F401XX_H_

#include <stddef.h>
#include <stdint.h>

#define __vo volatile
#define __weak __attribute__((weak))

/* ARM Cortex Mx PRocessor NVIC ISERx Register Addresses */
#define NVIC_ISER0                      ((__vo uint32_t*) 0xE000E100)
#define NVIC_ISER1                      ((__vo uint32_t*) 0xE000E104)
#define NVIC_ISER2                      ((__vo uint32_t*) 0xE000E108)
#define NVIC_ISER3                      ((__vo uint32_t*) 0xE000E10C)

/* ARM Cortex Mx PRocessor NVIC ICERx Register Addresses */
#define NVIC_ICER0                      ((__vo uint32_t*) 0xE000E180)
#define NVIC_ICER1                      ((__vo uint32_t*) 0xE000E184)
#define NVIC_ICER2                      ((__vo uint32_t*) 0xE000E188)
#define NVIC_ICER3                      ((__vo uint32_t*) 0xE000E18C)

/* ARM Cortex Mx Processor Priority register addresses */
#define NVIC_PR_BASE_ADDR             ((__vo uint32_t *) 0xE000E400)

#define NO_PR_BITS_IMPLEMENTED          4


/* Memories base address */
#define FLASH_BASEADDR                  0x08000000UL
#define SRAM1_BASEADDR                  0x20000000UL  
#define SRAM2_BASEADDR                  NULL            /* Not supported */
#define ROM_BASEADDR                    0x1FFF0000UL    /* System Memory */
#define SRAM                            SRAM1_BASEADDR

/* Buses base addresses */
#define APB1_BASEADDR                   0x40000000UL
#define APB2_BASEADDR                   0x40010000UL
#define AHB1_BASEADDR                   0x40020000UL
#define AHB2_BASEADDR                   0x50000000UL

/* APB1 Peripheral offsets */
#define I2C1_BASEADDR                   (APB1_BASEADDR + 0x5400)
#define I2C2_BASEADDR                   (APB1_BASEADDR + 0x5800)
#define I2C3_BASEADDR                   (APB1_BASEADDR + 0x5C00)
#define SPI2_BASEADDR                   (APB1_BASEADDR + 0x3800)
#define SPI3_BASEADDR                   (APB1_BASEADDR + 0x3C00)
#define USART2_BASEADDR                 (APB1_BASEADDR + 0x4400)

/* APB2 Peripheral offsets */
#define SPI1_BASEADDR                   (APB2_BASEADDR + 0x3000)
#define SPI4_BASEADDR                   (APB2_BASEADDR + 0x3400)
#define USART1_BASEADDR                 (APB2_BASEADDR + 0x1000)
#define USART6_BASEADDR                 (APB2_BASEADDR + 0x1400)
#define EXTI_BASEADDR                   (APB2_BASEADDR + 0x3C00)
#define SYSCFG_BASEADDR                 (APB2_BASEADDR + 0x3800)

/* AHB1 Peripheral bases address  */
#define GPIOA_BASEADDR                  (AHB1_BASEADDR + 0x0000)
#define GPIOB_BASEADDR                  (AHB1_BASEADDR + 0x0400)
#define GPIOC_BASEADDR                  (AHB1_BASEADDR + 0x0800)
#define GPIOD_BASEADDR                  (AHB1_BASEADDR + 0x0C00)
#define GPIOE_BASEADDR                  (AHB1_BASEADDR + 0x1000)
#define RCC_BASEADDR                    (AHB1_BASEADDR + 0x3800)



/* RCC peripheral register details */
typedef struct
{
    __vo uint32_t CR; 
    __vo uint32_t PLLCFGR;
    __vo uint32_t CFGR;
    __vo uint32_t CIR;
    __vo uint32_t AHB1RSTR;
    __vo uint32_t AHB2RSTR;
    __vo uint32_t AHB3RSTR;             /* Not in RM */
    uint32_t      RESERVED0;
    __vo uint32_t APB1RSTR;
    __vo uint32_t APB2RSTR;
    uint32_t      RESERVED1[2];
    __vo uint32_t AHB1ENR;
    __vo uint32_t AHB2ENR;
    __vo uint32_t AHB3ENR;              /* Not in RM */
    //uint32_t      RESERVED2[2];
    uint32_t      RESERVED2;
    __vo uint32_t APB1ENR;
    __vo uint32_t APB2ENR;
    uint32_t      RESERVED3[2];
    __vo uint32_t AHB1LPENR;
    __vo uint32_t AHB2LPENR;
    __vo uint32_t AHB3LPENR;            /* Not int RM */
    uint32_t      RESERVED4;
    __vo uint32_t APB1LPENR;
    __vo uint32_t APB2LPENR;
    int32_t       RESERVED5[2];
    __vo uint32_t BDCR;
    __vo uint32_t CSR;
    uint32_t      RESERVED6[2];
    __vo uint32_t SSCGR;               
    __vo uint32_t PLLI2SCFGR;           
    __vo uint32_t PLLSAICFGR;           /* Not in RM */
    __vo uint32_t DCKCFGR;              
    __vo uint32_t CKGATENR;             /* Not in RM */
    __vo uint32_t DCKCFGR2;             /* Not in RM */
} RCC_RegDef_t;

/* SPI peripheral register details */
typedef struct stm32f401xx
{
    __vo uint32_t CR1;
    __vo uint32_t CR2;
    __vo uint32_t SR;
    __vo uint32_t DR;
    __vo uint32_t CRCPR;
    __vo uint32_t RXCRC;
    __vo uint32_t TXCRC;
    __vo uint32_t I2SCFGR;
    __vo uint32_t I2SPR;
} SPI_RegDef_t;

/* I2C peripheral register details */
typedef struct 
{
    __vo uint32_t CR1;
    __vo uint32_t CR2;
    __vo uint32_t OAR1;
    __vo uint32_t OAR2;
    __vo uint32_t DR;
    __vo uint32_t SR1;
    __vo uint32_t SR2;
    __vo uint32_t CCR;
    __vo uint32_t TRISE;
    __vo uint32_t FLTR;
} I2C_RegDef_t;

/* UART peripheral register details */
typedef struct
{
    __vo uint32_t SR;
    __vo uint32_t DR;
    __vo uint32_t BBR;
    __vo uint32_t CR1;
    __vo uint32_t CR2;
    __vo uint32_t CR3;
    __vo uint32_t GTPR;
} UART_RegDef_t;

/* GPIO peripheral register details */
typedef struct
{
    __vo uint32_t MODER;
    __vo uint32_t OTYPER;
    __vo uint32_t OSPEEDR;
    __vo uint32_t PUPDR;
    __vo uint32_t IDR;
    __vo uint32_t ODR;
    __vo uint32_t BSRRL;
    __vo uint32_t BSRRH;
    __vo uint32_t LCKR;
    __vo uint32_t AFR[2];                    /* AFR[0] =  AFRL and AFR[1] = AFRH */
} GPIO_RegDef_t;

/* EXTI peripheral register details */
typedef struct
{
    __vo uint32_t IMR;
    __vo uint32_t EMR;
    __vo uint32_t RTSR;
    __vo uint32_t FTSR;
    __vo uint32_t SWIER;
    __vo uint32_t PR;
} EXTI_RegDef_t;

/* SYSCFG peripheral register details */
typedef struct 
{
    __vo uint32_t MEMRMP;
    __vo uint32_t PMC;
    __vo uint32_t EXTICR[4];
    uint32_t      RESERVED1[2];
    __vo uint32_t CMPCR;
    uint32_t      RESERVED2[2];
    __vo uint32_t CFGR;
} SYSCFG_RegDef_t;



/* RCC peripheral definition */
#define RCC                             ((RCC_RegDef_t *)RCC_BASEADDR)

/* GPIO peripheral definitions  */
#define GPIOA                           ((GPIO_RegDef_t *)GPIOA_BASEADDR)
#define GPIOB                           ((GPIO_RegDef_t *)GPIOB_BASEADDR)
#define GPIOC                           ((GPIO_RegDef_t *)GPIOC_BASEADDR)
#define GPIOD                           ((GPIO_RegDef_t *)GPIOD_BASEADDR)
#define GPIOE                           ((GPIO_RegDef_t *)GPIOE_BASEADDR)

/* SPI peripheral definitions */
#define SPI1                            ((SPI_RegDef_t *)SPI1_BASEADDR)
#define SPI2                            ((SPI_RegDef_t *)SPI2_BASEADDR)
#define SPI3                            ((SPI_RegDef_t *)SPI3_BASEADDR)
#define SPI4                            ((SPI_RegDef_t *)SPI4_BASEADDR)

/* I2C peripheral definitions */
#define I2C1                            ((I2C_RegDef_t *)I2C1_BASEADDR)
#define I2C2                            ((I2C_RegDef_t *)I2C2_BASEADDR)
#define I2C3                            ((I2C_RegDef_t *)I2C3_BASEADDR)

/* UART peripheral definitions*/

/* EXTI peripheral definition */
#define EXTI                            ((EXTI_RegDef_t *)EXTI_BASEADDR)

/* SYSCFG peripheral definition */
#define SYSCFG                          ((SYSCFG_RegDef_t *)SYSCFG_BASEADDR)



/* Clock Enable Macros for GPIOx peripherals */
#define GPIOA_PCLK_EN()                 (RCC->AHB1ENR |= (1 << 0))
#define GPIOB_PCLK_EN()                 (RCC->AHB1ENR |= (1 << 1))
#define GPIOC_PCLK_EN()                 (RCC->AHB1ENR |= (1 << 2))
#define GPIOD_PCLK_EN()                 (RCC->AHB1ENR |= (1 << 3))
#define GPIOE_PCLK_EN()                 (RCC->AHB1ENR |= (1 << 4))

/* Clock Enable Macros for I2Cx peripherals */
#define I2C1_PCLK_EN()                  (RCC->APB1ENR |= (1 << 21))
#define I2C2_PCLK_EN()                  (RCC->APB1ENR |= (1 << 22))
#define I2C3_PCLK_EN()                  (RCC->APB1ENR |= (1 << 23))

/* Clock Enable Macros for UARTx peripherals */
#define UART1_PCLK_EN()                 (RCC->APB2ENR |= (1 << 4))
#define UART2_PCLK_EN()                 (RCC->APB1ENR |= (1 << 17))
#define UART6_PCLK_EN()                 (RCC->APB2ENR |= (1 << 5))

/* Clock Enable Macros for SPIx peripherals */
#define SPI1_PCLK_EN()                  (RCC->APB2ENR |= (1 << 12))
#define SPI2_PCLK_EN()                  (RCC->APB1ENR |= (1 << 14))
#define SPI3_PCLK_EN()                  (RCC->APB1ENR |= (1 << 15))
#define SPI4_PCLK_EN()                  (RCC->APB2ENR |= (1 << 13))

/* Clock Enable macros for SYSCFG peripheral */
#define SYSCFG_PCLK_EN()                (RCC->APB2ENR |= (1 << 14))

/* Clock Disable Macros for GPIOx peripherals */
#define GPIOA_PCLK_DI()                 (RCC->AHB1ENR &= ~(1 << 0))
#define GPIOB_PCLK_DI()                 (RCC->AHB1ENR &= ~(1 << 1))
#define GPIOC_PCLK_DI()                 (RCC->AHB1ENR &= ~(1 << 2))
#define GPIOD_PCLK_DI()                 (RCC->AHB1ENR &= ~(1 << 3))
#define GPIOE_PCLK_DI()                 (RCC->AHB1ENR &= ~(1 << 4))

/* Clock Disable Macros for I2Cx peripherals */
#define I2C1_PCLK_DI()                  (RCC->APB1ENR &= ~(1 << 21))
#define I2C2_PCLK_DI()                  (RCC->APB1ENR &= ~(1 << 22))
#define I2C3_PCLK_DI()                  (RCC->APB1ENR &= ~(1 << 23))

/* Clock Disable Macros for UARTx peripherals */
#define UART1_PCLK_DI()                 (RCC->APB2ENR &= ~(1 << 4))
#define UART2_PCLK_DI()                 (RCC->APB1ENR &= ~(1 << 17))
#define UART6_PCLK_DI()                 (RCC->APB2ENR &= ~(1 << 5))

/* Clock Disable Macros for SPIx peripherals */
#define SPI1_PCLK_DI()                  (RCC->APB2ENR &= ~(1 << 12))
#define SPI2_PCLK_DI()                  (RCC->APB1ENR &= ~(1 << 14))
#define SPI3_PCLK_DI()                  (RCC->APB1ENR &= ~(1 << 15))
#define SPI4_PCLK_DI()                  (RCC->APB2ENR &= ~(1 << 13)) 



/* Macros to reset GPIO peripherals */
#define GPIOA_REG_RESET()               do{ (RCC->APB2RSTR |= (1 << 0)); (RCC->AHB1RSTR &= ~(1 << 0)); } while(0)
#define GPIOB_REG_RESET()               do{ (RCC->APB2RSTR |= (1 << 1)); (RCC->AHB1RSTR &= ~(1 << 1)); } while(0)
#define GPIOC_REG_RESET()               do{ (RCC->APB2RSTR |= (1 << 2)); (RCC->AHB1RSTR &= ~(1 << 2)); } while(0)
#define GPIOD_REG_RESET()               do{ (RCC->APB2RSTR |= (1 << 3)); (RCC->AHB1RSTR &= ~(1 << 3)); } while(0)
#define GPIOE_REG_RESET()               do{ (RCC->APB2RSTR |= (1 << 4)); (RCC->AHB1RSTR &= ~(1 << 4)); } while(0)

/* Macros to reset SPI peripherals */
#define SPI1_REG_RESET()                do{ (RCC->APB2RSTR |= (1 << 12)); (RCC->APB2RSTR &= ~(1 << 12)); } while(0)
#define SPI2_REG_RESET()                do{ (RCC->APB1RSTR |= (1 << 14)); (RCC->APB1RSTR &= ~(1 << 14)); } while(0)
#define SPI3_REG_RESET()                do{ (RCC->APB1RSTR |= (1 << 15)); (RCC->APB1RSTR &= ~(1 << 15)); } while(0)
#define SPI4_REG_RESET()                do{ (RCC->APB2RSTR |= (1 << 13)); (RCC->APB2RSTR &= ~(1 << 13)); } while(0)

/* Macros to reset I2C peripherals */
#define I2C1_REG_RESET()                do{ (RCC->APB1RSTR |= (1 << 21)); (RCC->APB1RSTR &= ~(1 << 21)); } while(0)
#define I2C2_REG_RESET()                do{ (RCC->APB1RSTR |= (1 << 22)); (RCC->APB1RSTR &= ~(1 << 22)); } while(0)
#define I2C3_REG_RESET()                do{ (RCC->APB1RSTR |= (1 << 23)); (RCC->APB1RSTR &= ~(1 << 23)); } while(0)

/* Macros for IRQ numbers */
#define IRQ_NUM_EXTI0                   6
#define IRQ_NUM_EXTI1                   7
#define IRQ_NUM_EXTI2                   8
#define IRQ_NUM_EXTI3                   9
#define IRQ_NUM_EXTI4                   10
#define IRQ_NUM_EXTI9_5                 23
#define IRQ_NUM_I2C1_EV                 31
#define IRQ_NUM_I2C1_ER                 32
#define IRQ_NUM_I2C2_EV                 33
#define IRQ_NUM_I2C2_ER                 34
#define IRQ_NUM_SPI1                    35
#define IRQ_NUM_SPI2                    36
#define IRQ_NUM_EXTI15_11               40
#define IRQ_NUM_SPI3                    51
#define IRQ_NUM_I2C3_EV                 72
#define IRQ_NUM_I2C3_ER                 73
#define IRQ_NUM_SPI4                    84

/* Macros for IRQ priorities */
#define NVIC_IRQ_PRIORITY_0             0
#define NVIC_IRQ_PRIORITY_1             1
#define NVIC_IRQ_PRIORITY_2             2
#define NVIC_IRQ_PRIORITY_3             3
#define NVIC_IRQ_PRIORITY_4             4
#define NVIC_IRQ_PRIORITY_5             5
#define NVIC_IRQ_PRIORITY_6             6
#define NVIC_IRQ_PRIORITY_7             7
#define NVIC_IRQ_PRIORITY_8             8
#define NVIC_IRQ_PRIORITY_9             9
#define NVIC_IRQ_PRIORITY_10            10             
#define NVIC_IRQ_PRIORITY_11            11
#define NVIC_IRQ_PRIORITY_12            12
#define NVIC_IRQ_PRIORITY_13            13
#define NVIC_IRQ_PRIORITY_14            14
#define NVIC_IRQ_PRIORITY_15            15



/* Return port code for given GPIOx base address*/
#define GPIO_BASSEADDR_TO_PORT_CODE(x)  (x == GPIOA) ? 0 :\
                                        (x == GPIOB) ? 1 :\
                                        (x == GPIOC) ? 2 :\
                                        (x == GPIOD) ? 3 :\
                                        (x == GPIOE) ? 4 :0



/* Bit Position defintions of SPI peripheral */
#define SPI_CR1_CPHA            0
#define SPI_CR1_CPOL            1
#define SPI_CR1_MSTR            2
#define SPI_CR1_BR              3
#define SPI_CR1_SPE             6
#define SPI_CR1_LSB_FIRST       7
#define SPI_CR1_SSI             8
#define SPI_CR1_SSM             9
#define SPI_CR1_RX_ONLY         10
#define SPI_CR1_DFF             11
#define SPI_CR1_CRC_NEXT        12
#define SPI_CR1_CRC_EN          13
#define SPI_CR1_BIDI_OE         14
#define SPI_CR1_BIDI_MODE       15

#define SPI_CR2_RXDMAEN         0
#define SPI_CR2_TXDMAEN         1
#define SPI_CR2_SSOE            2
#define SPI_CR2_RESERVED        3
#define SPI_CR2_FRF             4
#define SPI_CR2_ERRIE           5
#define SPI_CR2_RXNEIE          6
#define SPI_CR2_TXEIE           7

#define SPI_SR_RXNE             0
#define SPI_SR_TXE              1
#define SPI_SR_CHSIDE           2
#define SPI_SR_UDR              3
#define SPI_SR_CRCERR           4
#define SPI_SR_MODF             5
#define SPI_SR_OVR              6
#define SPI_SR_BSY              7
#define SPI_SR_FRE              8


/* Bit position definitions of I2C peripheral  */

#define I2C_CR1_PE              0
//#define I2C_CR1_SMBUS           1
//#define I2C_CR1_SMBTYPE         3
//#define I2C_CR1_ENARP           4
//#define I2C_CR1_ENPEC           5
//#define I2C_CR1_ENGC            6
#define I2C_CR1_NOSTRETCH       7
#define I2C_CR1_START           8
#define I2C_CR1_STOP            9
#define I2C_CR1_ACK             10
//#define I2C_CR1_POS             11
//#define I2C_CR1_PEC             12
//#define I2C_CR1_ALERT           13
#define I2C_CR1_SWRST           15

#define I2C_CR2_FREQ            0
#define I2C_CR2_ITERREN         8
#define I2C_CR2_ITEVFEN         9
#define I2C_CR2_ITBUFEN         10

#define I2C_OAR1_ADD0           0
#define I2C_OAR1_ADD7_1         1
#define I2C_OAR1_ADD9_8         8
#define I2C_OAR1_ADDMODE        15

#define I2C_OAR2_ENDUAL         0
#define I2C_OAR2_ADD2           1

#define I2C_SR1_SB              0
#define I2C_SR1_ADDR            1
#define I2C_SR1_BTF             2
#define I2C_SR1_ADD10           3
#define I2C_SR1_STOPF           4
#define I2C_SR1_RXNE            6
#define I2C_SR1_TXE             7
#define I2C_SR1_BERR            8
#define I2C_SR1_ARLO            9
#define I2C_SR1_AF              10
#define I2C_SR1_OVR             11
#define I2C_SR1_PECERR          12
#define I2C_SR1_TIMEOUT         14

#define I2C_SR2_MSL             0
#define I2C_SR2_BUSY            1
#define I2C_SR2_TRA             2
//#define I2C_SR2_GENCALL         4
//#define I2C_SR2_SMBDEFAULT      5
//#define I2C_SR2_SMBHOST         6
#define I2C_SR2_DUALF           7
//#define I2C_SR2_PEC             8

#define I2C_CCR_CCR             0
#define I2C_CCR_DUTY            14
#define I2C_CCR_FS              15  



/* Generic macros*/
#define ENABLE          1
#define DISABLE         0
#define SET             ENABLE
#define RESET           DISABLE
#define GPIO_PIN_SET    SET
#define GPIO_PIN_RESET  RESET
#define FLAG_SET        SET
#define FLAG_RESET      RESET





#include "stm32f401xx_gpio_driver.h"
#include "stm32f401xx_spi_driver.h"
#include "stm32f401xx_i2c_driver.h"


#endif /* INC_STM32F401XX_H_ */
