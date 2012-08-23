#!/bin/bash
#烧写程序到stm32f4xx

#修改stlink使用权限
sudo chmod -R a+xwr /dev/bus/usb/
#全片擦除
st-flash erase
#将main.bin烧写到stm32f4xx到0x8000000位置
st-flash write main.bin 0x8000000