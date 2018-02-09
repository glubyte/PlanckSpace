CC = gcc

# INCLUDE_PATHS
INCLUDE_PATHS = -IC:\SDL207\include\SDL2 -IC:\VulkanSDK\1.0.65.0\Include

# LIBRARY_PATHS
LIBRARY_PATHS = -LC:\SDL207\lib -LC:\VulkanSDK\1.0.65.0\Lib 
# -LC:\mingw_dev\windowslibs

# COMPILER FLAGS
COMPILER_FLAGS = -O3
# -w; suppresses all warnings
# -Wl, -subsystem,windows; gets rid of the console window

# LINKER_FLAGS
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lvulkan-1
# -lkernel32 -ladvapi32 -lcomdlg32 -lgdi32 -lodbc32 -lodbccp32 -lole32 -loleaut32 -lshell32 -luser32 -luuid -lwinspool


OBJ_NAME = planckspace

objects = main.o vk.o sdl.o maths.o gen.o perception.o fps.o physik.o chunks.o init.o menu.o

all : $(objects)
	gcc $(COMPILER_FLAGS) $(objects) $(OBJ_NAME).res $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o winbuild\$(OBJ_NAME)

main.o : vk.h sdl.h gen.h fps.h chunks.h perception.h init.h maths.h menu.h
	gcc -c $(COMPILER_FLAGS) main.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS) 

vk.o : vk.h maths.h perception.h
	gcc -c $(COMPILER_FLAGS) vk.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

sdl.o: sdl.h
	gcc -c $(COMPILER_FLAGS) sdl.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
	
maths.o : maths.h
	gcc -c $(COMPILER_FLAGS) maths.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

gen.o : gen.h maths.h chunks.h
	gcc -c $(COMPILER_FLAGS) gen.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

perception.o : perception.h maths.h sdl.h
	gcc -c $(COMPILER_FLAGS) perception.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

fps.o : fps.h
	gcc -c $(COMPILER_FLAGS) fps.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

physik.o : physik.h
	gcc -c $(COMPILER_FLAGS) physik.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)

chunks.o : chunks.h
	gcc -c $(COMPILER_FLAGS) chunks.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
	
init.o : init.h vk.h sdl.h perception.h fps.h
	gcc -c $(COMPILER_FLAGS) init.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
	
menu.o : menu.h vk.h sdl.h fps.h
	gcc -c $(COMPILER_FLAGS) menu.c $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(LINKER_FLAGS)
	
clean :
	rm $(objects)
