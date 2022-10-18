TARGET = PSPickr

SOURCES  := data source
CFILES   := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.cpp))
GFXFILES := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.png))
WAVFILES := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.wav))

OBJS := $(addsuffix .o,$(BINFILES)) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(GFXFILES:.png=.o) $(WAVFILES:.wav=.o)

VERSION_MAJOR :=  1
VERSION_MINOR :=  0
VERSION_MICRO :=  0

INCDIR   = libs/ libs/include include
CFLAGS   = -Os -Wall -ffast-math -Wno-narrowing -Wno-unused-variable \
           -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) -DVERSION_MICRO=$(VERSION_MICRO)
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -std=gnu++17
ASFLAGS  := $(CFLAGS)

BUILD_PRX = 1
PSP_LARGE_MEMORY = 1

LIBDIR  = libs/lib
LDFLAGS =
LIBS    = -lintrafont -lglib2d -ljpeg -lpng16 -lstdc++ -lz -lpspgu -lpspvram -lpspaudiolib -lpspaudio -lpspreg

EXTRA_TARGETS   = EBOOT.PBP
PSP_EBOOT_TITLE = PSPickr v$(VERSION_MAJOR).$(VERSION_MINOR)$(VERSION_MICRO)
PSP_EBOOT_ICON  = ICON0.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

%.o: %.png
	bin2o -i $< $@ $(addsuffix _png, $(basename $(notdir $<) ))

%.o: %.wav
	bin2o -i $< $@ $(addsuffix _wav, $(basename $(notdir $<) ))
