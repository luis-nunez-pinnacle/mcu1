# MCU1 — Mastering Microcontrollers and Embedded Driver Development

Code examples, STM32CubeIDE projects, and study notes for the Udemy course
**Mastering Microcontrollers and Embedded Driver Development**.

These projects are for learning and experimentation with the STM32F401RE
microcontroller and its peripherals.

## Repository structure

```text
.
├── 001helloworld/
├── 002sampleapp/
├── 003clock/
├── 004PeripheralClockEnable/
├── 005HSI_measurement/
├── 006GPIOInterrupt/
├── stm32f4xx_drivers/
│   ├── drivers/
│   │   ├── Inc/
│   │   │   ├── stm32f401xx.h
│   │   │   ├── stm32f401xx_gpio_driver.h
│   │   │   └── stm32f401xx_spi_driver.h
│   │   └── Src/
│   │       ├── stm32f401xx_gpio_driver.c
│   │       └── stm32f401xx_spi_driver.c
│   ├── Inc/
│   └── Src/
│       ├── Example applications
│       ├── main.c
│       ├── syscalls.c
│       └── sysmem.c
├── Notes.md
└── .gitignore
```
## Getting started

Open or import the projects in STM32CubeIDE. Each numbered folder is a separate example project. The `stm32f4xx_drivers` folder contains the driver-development project and example applications.

Build and run the project for your STM32F401RE board using its STM32CubeIDE configuration.

## Notes

See [Notes.md](Notes.md) for theory and study notes on drivers, interfaces, and related microcontroller topics.

## Build artifacts

IDE-generated build output is excluded from version control. Build the projects locally in STM32CubeIDE.