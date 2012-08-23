PROJ_NAME=main

CC=arm-none-eabi-gcc

OBJCOPY=arm-none-eabi-objcopy

vpath %.c user/src stm32lib/src freertos/src

CFLAGS  = -g -O2 -Wall -Tstm32_flash.ld 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -Iuser/inc -Istm32lib/inc -Ifreertos/inc

SRCS += user/src/startup_stm32f4xx.s user/src/main.c \
user/src/stm32f4xx_gpio_config.c user/src/stm32f4xx_rcc_config.c \
user/src/stm32f4xx_usart_config.c \
user/src/stm32f4xx_nvic_config.c user/src/simple_task.c

SRCS += stm32lib/src/misc.c stm32lib/src/stm32f4xx_it.c \
stm32lib/src/stm32f4xx_usart.c \
stm32lib/src/stm32f4xx_gpio.c stm32lib/src/stm32f4xx_rcc.c \
stm32lib/src/system_stm32f4xx.c 

SRCS += freertos/src/heap_2.c freertos/src/list.c freertos/src/port.c \
freertos/src/queue.c freertos/src/task.c freertos/src/timers.c


OBJS = $(SRCS:.c=.o)

proj:	$(PROJ_NAME).elf


$(PROJ_NAME).elf:$(SRCS) $(OBJS)
	$(CC) $(CFLAGS) $(SRCS)
	$(OBJCOPY) -O binary *.out $(PROJ_NAME).bin
	rm -f *.out

clean:
	rm -f *.o
	rm -f user/src/*.o
	rm -f stm32lib/src/*.o
	rm -f freertos/src/*.o