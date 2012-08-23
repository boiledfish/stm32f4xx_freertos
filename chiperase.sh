#!/bin/bash

#修改stlink使用权限
sudo chmod -R a+xwr /dev/bus/usb/

#全片擦除
st-flash erase