# This files is only for PocketBook 301/360

SDKPREFIX=/usr/local/pocketbook
OUT = dominos
SOURCES = dominos.cpp dmain.cpp help.cpp messages.c

CXX = wineg++
CC = winegcc
LIBS = -linkview -ljpeg `freetype-config --libs`
INCLUDE = -I$(SDKPREFIX)/include `freetype-config --cflags`
LIBDIR=-L$(SDKPREFIX)/lib
WINEOUT = $(OUT).exe.so
CFLAGS = -Wall -fomit-frame-pointer -O2 -I../include
WINEFLAGS = -mwindows -m32
ARMCC = $(SDKPREFIX)/bin/arm-linux-gcc
ARMCXX = $(SDKPREFIX)/bin/arm-linux-g++
ARMLIBDIR = -L$(SDKPREFIX)/arm-linux/lib $(LIBDIR)
ARMINCLUDE = -I$(SDKPREFIX)/arm-linux/include $(INCLUDE)
ARMOUT = $(OUT).app
IMAGES = $(wildcard ../data/images/*.bmp)

OBJECTS = $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCES)))
ARM_OBJECTS = $(patsubst %.c, %.o, $(patsubst %.cpp, arm_%.o, $(SOURCES)))

all: $(OUT)

arm: $(ARMOUT)

$(OUT): $(OBJECTS)
	$(CXX) -o $@ $^ $(INCLUDE) $(LIBDIR) $(LIBS) $(CFLAGS) $(WINEFLAGS)

$(ARMOUT): $(ARM_OBJECTS)
	$(ARMCXX) $(CFLAGS) $(ARMINCLUDE) $(ARMLIBDIR) $(LIBS) -o $@ $^

arm_%.o: %.c
	$(ARMCC) -c $(INCLUDE) $(CFLAGS) -o $@ $<

arm_%.o: %.cpp
	$(ARMCXX) -c $(INCLUDE) $(CFLAGS) -o $@ $<

.cpp.o:
	$(CXX) -c $(INCLUDE) $(CFLAGS) -o $@ $<

.c.o:
	$(CC) -c $(INCLUDE) $(CFLAGS) -o $@ $<

clean:
	-rm -f $(ARMOUT) $(OUT) $(WINEOUT) $(IMGSRC) $(OBJECTS) $(ARM_OBJECTS)

# tiles.c: $(IMAGES)
# 	$(SDKPREFIX)/bin/pbres -c $@ $^

help.cpp: help.txt
	awk -f gen_help.awk help.txt > help.cpp
