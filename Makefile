all:
	gcc -Wall main.c -o main
	./main
	mv image2.bmp /sdcard/null
