CC = gcc
CFLAGS = -I.
LDFLAGS = -luser32 -lgdi32 -lkernel32
BUILD_DIR = build

sgl_default_example: $(BUILD_DIR)/default_example.o $(BUILD_DIR)/SGL.o
	$(CC) -o $(BUILD_DIR)/sgl_default_example $(BUILD_DIR)/default_example.o $(BUILD_DIR)/SGL.o $(LDFLAGS)

sgl_bezier_example: $(BUILD_DIR)/bezier_example.o $(BUILD_DIR)/SGL.o
	$(CC) -o $(BUILD_DIR)/sgl_bezier_example $(BUILD_DIR)/bezier_example.o $(BUILD_DIR)/SGL.o $(LDFLAGS)

$(BUILD_DIR)/default_example.o: default_example.c SGL.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c default_example.c -o $(BUILD_DIR)/default_example.o

$(BUILD_DIR)/bezier_example.o: bezier_example.c SGL.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c bezier_example.c -o $(BUILD_DIR)/bezier_example.o

$(BUILD_DIR)/SGL.o: SGL.c SGL.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c SGL.c -o $(BUILD_DIR)/SGL.o

all: clean sgl_default_example sgl_bezier_example

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)