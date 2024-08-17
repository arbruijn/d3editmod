EMUOBJ = emu/x86emu.o emu/x86run.o emu/x86run66.o emu/x86run660f.o emu/x86run_private.o \
	emu/x86primop.o emu/x87emu_private.o emu/x87run.o emu/tools/box86stack.o

#CFLAGS += -m32
CFLAGS += -Wall -Wno-unused -g -I. -Iemu/include -I../../pkg/d3sdk/osiris -I../../pkg/d3sdk/include
#CFLAGS += -DTEST
CFLAGS += -DLINUX -D__LINUX__
CFLAGS += -I../lib -I.. -Wno-unknown-pragmas
CXXFLAGS = $(CFLAGS)
LDLIBS += -lm

all: dllload

dllload: dll.o nod3.o $(EMUOBJ)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f testld.exe testld2.dll $(EMUOBJ) dll.o nod3.o

EMINC = -I. -I../libx86emu -I../libx86emu/include
EMCCOPT = -O $(EMINC)
EMLDOPT = --preload-file level1.dll

myld.html: $(patsubst %.o,%.em.o,myld.o $(patsubst %,../libx86emu/%,$(EMUOBJ)))
	emcc $(EMLDOPT) -o $@ $^
%.em.o: %.c
	emcc -Wno-pragma-pack $(EMCCOPT) -c -o $@ $^
