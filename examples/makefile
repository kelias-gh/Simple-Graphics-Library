CC = gcc
CFLAGS = -c -Wall -Werror -Wno-unused-function
ROOT_DIR := $(shell git rev-parse --show-toplevel 2>/dev/null || echo $(CURDIR))
ODIR = $(CURDIR)/build/objs
EXEDIR = $(CURDIR)/build
DLLFLAG = -L$(CURDIR)/build -lSGL_DLL
INC = -I$(ROOT_DIR)
SGL_DLL_DIR = $(ROOT_DIR)/build

.PHONY: all clean

all: clean directories copy_dll bezier_example default_example

bezier_example: $(ODIR)/bezier_example.o | $(EXEDIR)
	$(CC) -o $(EXEDIR)/bezier_example $(ODIR)/bezier_example.o $(DLLFLAG)

default_example: $(ODIR)/default_example.o | $(EXEDIR)
	$(CC) -o $(EXEDIR)/default_example $(ODIR)/default_example.o $(DLLFLAG)

$(ODIR)/default_example.o: default_example.c | $(ODIR)
	$(CC) $(CFLAGS) $(INC) -c default_example.c -o $(ODIR)/default_example.o

$(ODIR)/bezier_example.o: bezier_example.c | $(ODIR)
	$(CC) $(CFLAGS) $(INC) -c bezier_example.c -o $(ODIR)/bezier_example.o

copy_dll:
	cp $(SGL_DLL_DIR)/SGL_DLL.dll $(CURDIR)/build

directories:
	mkdir -p $(EXEDIR)
	mkdir -p $(ODIR)

clean:
	rm -rf $(ODIR) $(EXEDIR)