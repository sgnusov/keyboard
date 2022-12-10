arduino-cli compile -u -p /dev/ttyUSB0 --fqbn STMicroelectronics:stm32:GenF1:pnum=BLUEPILL_F103C8,xserial=generic,usb=HID,upload_method=serialMethod \
	--build-property="compiler.cpp.extra_flags=\"-O0\"" --output-dir ./build --clean keyboard
