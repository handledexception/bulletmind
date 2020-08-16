SRC_DIR = ./src
DEPS_DIR = ./deps

OUT_DIR = ./out/linux/clang-x64-debug
APP = $(OUT_DIR)/bulletmind

LIBS=
LIBDIRS=-L$(DEPS_DIR)/

CC ?= clang
LD = $(CC)
CFLAGS = --std=c11 -g -O0 -Wall -m64 -D BM_LINUX -D BM_DEBUG

LDFLAGS = -lc -lm -lSDL2

INCLUDES = -I$(SRC_DIR) \
	-I$(DEPS_DIR)/tomlc99 \
	-I/usr/include/SDL2 

SRCS = $(SRC_DIR)/binary.c \
	$(SRC_DIR)/buffer.c \
	$(SRC_DIR)/command.c \
	$(SRC_DIR)/entity.c \
	$(SRC_DIR)/font.c \
	$(SRC_DIR)/sprite.c \
	$(SRC_DIR)/input.c \
	$(SRC_DIR)/render.c \
	$(SRC_DIR)/resource.c \
	$(SRC_DIR)/engine.c \
	$(SRC_DIR)/performance.c \
	$(SRC_DIR)/toml_config.c \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/memalign.c \
	$(SRC_DIR)/memarena.c \
	$(DEPS_DIR)/tomlc99/toml.c

OBJS = $(patsubst %.c,%.o,$(SRCS))

%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

clean:
	rm -rf $(SRC_DIR)/*.o
	rm -rf $(OUT_DIR)/bulletmind
	rm -rf $(OUT_DIR)/assets
	rm -rf $(OUT_DIR)/config

all: $(APP)

$(APP): $(OBJS)
	$(LD) $(LDFLAGS) $(LIBDIRS) $(OBJS) -o $(APP) $(LIBS)
	cp -R ./assets $(OUT_DIR)/assets
	cp -R ./config $(OUT_DIR)/config
