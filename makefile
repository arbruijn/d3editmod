#CXX = clang++
#CC = clang
#CXXFLAGS += -fsanitize=undefined -fno-sanitize=alignment
#LDFLAGS += -fsanitize=undefined -fno-sanitize=alignment
DEF = -Ilib -DLINUX -D__LINUX__ -I. -DMEM_USE_RTL -DSTATIC_OPENGL -DNED_PHYSICS -DNEWEDITORSRC -DMONO
DEF += -I../gl4es/include
CXXFLAGS += -m32 -fno-pic -fcf-protection=none
LDFLAGS += -m32
CXXFLAGS += -I/usr/include/SDL2 
CXXFLAGS += -Wall -Wno-unknown-pragmas -Wno-unused-variable -g  -Wno-multichar $(DEF)
LDLIBS += -lm -lGL -lSDL2 

all: d3

OBJS = d3.o ambient.o bnode.o boa.o gamepath.o lightmap_info.o \
	LoadLevel.o room.o ship.o special_face.o \
	terrain.o vclip.o door.o levelgoal.o object.o terrainsearch.o \
	polymodel.o object_lighting.o matcen.o trigger.o renderobject.o render.o postrender.o terrainrender.o \
	sound.o weapon.o demo.o diff.o player.o pagein.o
# matcen.o postrender.o
# terrainrender.o terrainsearch.o

OBJS += 3d/CLIPPER.o 3d/DRAW.o 3d/GLOBVARS.o 3d/INSTANCE.o 3d/POINTS.o 3d/SETUP.o
# 3d/MATRIX.o


OBJS += renderer/renderer.o renderer/rend_opengl.o renderer/notexture.o

OBJS += bitmap/bitmain.o bitmap/lightmap.o bitmap/bumpmap.o bitmap/iff.o bitmap/pcx.o bitmap/tga.o

OBJS += vecmat/vector.o fix/fix.o

OBJS += physics/FindIntersection.o physics/physics_common.o physics/physicspoly.o physics/physics.o

OBJS += misc/endian.o misc/error.o misc/psglob.o misc/psrand.o misc/pstring.o

OBJS += mem/Mem.o

OBJS += cfile/CFILE.o cfile/hog.o

OBJS += linux/lnxdebug.o linux/lnxlib.o

OBJS += globals.o util.o

OBJS += manage.o

d3: $(OBJS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS)

%.em.o: %.cpp
	emcc -g -c $(DEF) -o $@ $^
#-sGL_UNSAFE_OPTS=0
# 
d3.html: $(patsubst %.o,%.em.o,$(OBJS))
	emcc -g -sFULL_ES2 -L../gl4es/lib -lGL  -sINITIAL_MEMORY=64mb --preload-file d3demo.hog -sUSE_SDL=2  -o $@ $^

clean:
	rm -f $(OBJS)
