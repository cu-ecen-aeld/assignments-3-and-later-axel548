CC = gcc
CFLAGS = -Wall -Werror

writer: writer.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f writer

ifdef CROSS_COMPILE
CC = $(CROSS_COMPILE)gcc
CFLAGS += --sysroot=$(shell $(CC) -print-sysroot)
endif

.PHONY: all clean
