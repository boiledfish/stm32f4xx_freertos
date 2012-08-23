#!/bin/bash

#修改stlink使用权限
sudo chmod -R a+xwr /dev/bus/usb/

#将main.bin烧写到stm32f4xx到0x8000000位置
st-flash write main.bin 0x8000000