NAME = wifimeter
CC = xtensa-lx106-elf-gcc
CFLAGS = -Wall -Wno-implicit-function-declaration -Os -I. -mlongcalls -DICACHE_FLASH
LDLIBS = -nostdlib -Wl,--start-group -lmain -lupgrade -lnet80211 -lwpa -llwip -lpp -lphy -Wl,--end-group -lcirom -lgcc
LDFLAGS = -Teagle.app.v6.ld

$(NAME)-0x00000.bin: $(NAME)
	esptool.py elf2image $^

$(NAME): \
	main.o \
	httpd.o \
	handler.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

flash: $(NAME)-0x00000.bin
	esptool.py write_flash 0x00000 $(NAME)-0x00000.bin 0x40000 $(NAME)-0x40000.bin

clean:
	rm -rf $(NAME) $(NAME)-0x00000.bin $(NAME)-0x40000.bin *.o

