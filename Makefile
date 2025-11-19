# 3D Campus Event Hall Visualizer
EXE=final

# Main target
all: $(EXE)

#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  macOS
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  macOS/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
main.o: main.c CSCIx229.h
scene.o: scene.c CSCIx229.h
object.o: object.c CSCIx229.h
geometry.o: geometry.c CSCIx229.h
collision.o: collision.c CSCIx229.h
controls.o: controls.c CSCIx229.h
mouse.o: mouse.c CSCIx229.h
errcheck.o: errcheck.c CSCIx229.h
lighting.o: lighting.c CSCIx229.h
loadtexbmp.o: loadtexbmp.c CSCIx229.h
fatal.o: fatal.c CSCIx229.h
print.o: print.c CSCIx229.h

#  Create archive (professor’s helper lib)
CSCIx229.a: fatal.o  errcheck.o print.o loadtexbmp.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

#  Link final executable
$(EXE): main.o scene.o object.o controls.o mouse.o lighting.o geometry.o collision.o CSCIx229.a
	gcc $(CFLG) -o $@ $^ $(LIBS)

#  Clean
clean:
	$(CLEAN)
