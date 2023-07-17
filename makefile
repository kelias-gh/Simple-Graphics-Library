CC = gcc
CFLAGS = -c -Wall -Werror -Wno-unused-function -Wno-incompatible-pointer-types
LDFLAGS = -luser32 -lgdi32 -lkernel32
ROOT_DIR := $(shell git rev-parse --show-toplevel 2>/dev/null || echo $(CURDIR))
ODIR = $(ROOT_DIR)/build/objs
DLLDIR = $(ROOT_DIR)/build
INC = -I$(ROOT_DIR)

.PHONY: SGL_DLL

SGL_DLL: $(ODIR)/SGL.o
	$(CC) -shared -o $(DLLDIR)/SGL_DLL.dll $(ODIR)/SGL.o $(LDFLAGS)

$(ODIR)/SGL.o: $(ROOT_DIR)/SGL.c $(ROOT_DIR)/SGL.h | $(ODIR)	
	$(CC) $(CFLAGS) $(ROOT_DIR)/SGL.c -o $(ODIR)/SGL.o

$(ODIR):
	mkdir -p $(ODIR)
