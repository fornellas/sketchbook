PORT=/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A50285BI-if00-port0
UPLOAD_BAUD=3000000
CONSOLE_BAUD=9600

.PHONY: compile
compile:
	arduino-cli compile --fqbn esp8266:esp8266:nodemcuv2 .

.PHONY: upload
upload: compile
	 arduino-cli upload -p $(PORT) --fqbn esp8266:esp8266:nodemcuv2:baud=$(UPLOAD_BAUD) .

.PHONY: console
console:
	screen $(PORT) $(CONSOLE_BAUD)