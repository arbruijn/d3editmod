cmake_minimum_required(VERSION 3.12)

project(d3)

set(CMAKE_LIBRARY_PATH "/usr/lib/i386-linux-gnu" CACHE PATH "<desc>")
find_package(SDL2 REQUIRED)
#find_package(OpenGL REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})

#set(OPTS "-m32")
#set(CMAKE_C_FLAGS "$(OPTS)")
#set(CMAKE_CXX_FLAGS "$(OPTS)")
#set(CMAKE_EXE_LINKER_FLAGS "$(OPTS)")
add_compile_options("-m32 -g")
add_link_options("-m32")
add_compile_definitions(LINUX __LINUX__ MEM_USE_RTL STATIC_OPENGL NEWEDITORSRC MONO)

include_directories("${PROJECT_SOURCE_DIR}/lib")
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/dllload")
include_directories("${PROJECT_SOURCE_DIR}/dllload/emu/include")

#target_include_directories(d3 ${PROJET_SOURCE_DIR}/lib)

set(SRC main/d3.cpp main/ambient.cpp main/bnode.cpp main/boa.cpp main/gamepath.cpp main/lightmap_info.cpp
	main/loadlevel.cpp main/room.cpp main/ship.cpp main/special_face.cpp
	main/terrain.cpp main/vclip.cpp main/door.cpp main/levelgoal.cpp main/object.cpp main/terrainsearch.cpp
	model/polymodel.cpp main/object_lighting.cpp main/matcen.cpp main/trigger.cpp main/renderobject.cpp
	main/render.cpp main/postrender.cpp main/terrainrender.cpp
	sound/hlsoundlib.cpp
	main/weapon.cpp main/demofile.cpp main/difficulty.cpp main/player.cpp main/pagein.cpp main/scorch.cpp
	main/ai.cpp main/fireball.cpp main/viseffect.cpp main/doorway.cpp
	main/msafe.cpp main/msafeget.cpp main/multi.cpp main/powerup.cpp
	main/hud.cpp main/osiris_predefs.cpp main/game.cpp main/localization.cpp main/gamefont.cpp
	grtext/grfont.cpp grtext/grtext.cpp main/inventory.cpp main/osiris_timer.cpp main/guidebot.cpp main/damage.cpp
	main/gamedll.cpp main/draw.cpp main/attach.cpp main/spew.cpp main/gameevent.cpp main/cockpit.cpp)

if(LINUX)
list(APPEND SRC
	dllload/emu/x86emu.c dllload/emu/x86run.c dllload/emu/x86run66.c dllload/emu/x86run660f.c
	dllload/emu/x86run_private.c dllload/emu/x86primop.c dllload/emu/x87emu_private.c
	dllload/emu/x87run.c dllload/emu/tools/box86stack.c dllload/dll.cpp)
else()
list(APPEND SRC dllload/nodll.cpp)
endif()

list(APPEND SRC ui/uidraw.cpp)

list(APPEND SRC 3d/CLIPPER.cpp 3d/DRAW.cpp 3d/GLOBVARS.cpp 3d/INSTANCE.cpp 3d/POINTS.cpp 3d/SETUP.cpp)

list(APPEND SRC renderer/renderer.cpp renderer/rend_opengl.cpp renderer/notexture.cpp)

list(APPEND SRC bitmap/bitmain.cpp bitmap/lightmap.cpp bitmap/bumpmap.cpp bitmap/iff.cpp bitmap/pcx.cpp bitmap/tga.cpp)

list(APPEND SRC vecmat/vector.cpp fix/fix.cpp)

list(APPEND SRC physics/FindIntersection.cpp physics/physics_common.cpp physics/physicspoly.cpp physics/physics.cpp
	physics/collide.cpp)

list(APPEND SRC misc/endian.cpp misc/error.cpp misc/psglob.cpp misc/psrand.cpp misc/pstring.cpp)

list(APPEND SRC mem/Mem.cpp)

list(APPEND SRC cfile/CFILE.cpp cfile/hog.cpp)

list(APPEND SRC linux/lnxdebug.cpp linux/lnxlib.cpp)

list(APPEND SRC globals.cpp util.cpp)

list(APPEND SRC manage/manage.cpp)

add_executable(d3 "${SRC}")
#target_link_libraries(d3 ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
target_link_libraries(d3 SDL2 GL)

