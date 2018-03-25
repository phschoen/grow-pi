all: main

main: main.c dh22.c gpio.c sleep.c giesomat.c sample.c
	gcc -o $@ $^ -O3

PI_IP=192.168.178.68
download-sensor-data:
	scp pi@$(PI_IP):/home/pi/usb/sensor* ./


image:
	./plot
	firefox ./test.png

