#ifndef CSCIx229
#define CSCIx229

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

// --- GLEW _MUST_ be included first ---
#ifdef USEGLEW
#include <GL/glew.h>
#endif

// --- Get all GL prototypes ---
#define GL_GLEXT_PROTOTYPES

// --- Select SDL, SDL2, GLFW or GLUT ---
#if defined(SDL2)
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#elif defined(SDL)
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#elif defined(GLFW)
#include <GLFW/glfw3.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// --- Make sure GLU and GL are included ---
#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

// --- Utility macros ---
#define Cos(th) cos(3.14159265/180*(th))
#define Sin(th) sin(3.14159265/180*(th))
#define PI 3.14159265

#ifdef __cplusplus
extern "C" {
#endif

// =======================================================
//                  Utility functions
// =======================================================
#ifdef __GNUC__
void Print(const char* format , ...) __attribute__ ((format(printf,1,2)));
void Fatal(const char* format , ...) __attribute__ ((format(printf,1,2))) __attribute__ ((noreturn));
#else
void Print(const char* format , ...);
void Fatal(const char* format , ...);
#endif

unsigned int LoadTexBMP(const char* file);
void ErrCheck(const char* where);

#include <stdbool.h>   // for bool, true, false

// =======================================================
//                  PROJECT INTERFACES
// =======================================================

// --- Scene & rendering ---
void scene_display(void);
void scene_init(void);
void reshape(int width, int height);
void Project(); 

// --- Object draw functions ---
void drawTable(float x, float z);
void drawCocktailTable(float x, float z);
void drawBanquetChair(float x, float z);
void drawLamp(float x, float z);
void drawDoor(float x, float z, float width, float height);
void drawCurvedScreen(float wallX, float wallZ, float width, float height,
                      float yBase, float radiusH, float radiusV, float zOffset);

// --- Controls ---
void controls_key(unsigned char ch, int x, int y);
void controls_special(int key, int x, int y);


// =======================================================
//                  DEBUG / CAMERA GLOBALS
// =======================================================
extern int debugMode;
extern int debugObjectIndex;
extern int totalObjects;

// =======================================================
//                  SCENE OBJECT SYSTEM
// =======================================================

typedef struct {
    int id;
    char name[32];
    float x, y, z;
    float scale;
    float rotation;
    float bbox[6];                // (xmin, xmax, ymin, ymax, zmin, zmax)
    void (*drawFunc)(float, float);
    int movable;                  // 1 = draggable
    bool solid;
} SceneObject;

bool collidesWithAnyObject(SceneObject* movingObj, float newX, float newZ);

#define MAX_OBJECTS 100
extern SceneObject objects[MAX_OBJECTS];
extern int objectCount;

extern SceneObject* selectedObject;
extern int dragging;

// =======================================================
//                  MOUSE INTERACTION
// =======================================================

// --- Mouse interaction handlers ---
void mouse_button(int button, int state, int x, int y);
void mouse_motion(int x, int y);

// --- Utility for mapping screen → world (to be used by mouse.c) ---
void screenToWorld(int x, int y, float* worldX, float* worldZ);

// --- Object picking helper ---
SceneObject* pickObject(float worldX, float worldZ);

#ifdef __cplusplus
}
#endif

#endif
