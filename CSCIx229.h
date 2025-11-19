#ifndef CSCIx229
#define CSCIx229

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

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

// Formatted printing and fatal error helpers
#ifdef __GNUC__
void Print(const char* format , ...) __attribute__ ((format(printf,1,2)));
void Fatal(const char* format , ...) __attribute__ ((format(printf,1,2))) __attribute__ ((noreturn));
#else
void Print(const char* format, ...);
void Fatal(const char* format, ...);
#endif

// Texture loading and GL error check utilities
unsigned int LoadTexBMP(const char* file);
void ErrCheck(const char* where);

// Scene and rendering
void scene_display(void);
void scene_init(void);
void reshape(int width, int height);
void Project(void);

// Object draw functions
void drawTable(float x, float z);
void drawCocktailTable(float x, float z);
void drawCocktailTable2(float x, float z);
void drawCocktailTable3(float x, float z);
void drawMeetingTable(float x, float z);
void drawBanquetChair(float x, float z);
void drawLamp(float x, float z);
void drawDoor(float x, float z, float width, float height);
void drawCurvedScreen(float wallX, float wallZ, float width, float height,
                      float yBase, float radiusH, float radiusV, float zOffset);
void drawBarChairObj(float x, float z);
void drawCeilingLights(void);

// Controls
void controls_key(unsigned char ch, int x, int y);
void controls_special(int key, int x, int y);

// Lighting
void lighting_init(void);
void lighting_update(void);
void lighting_draw_debug_marker(void);
void drawLampBulb(void);

extern int lightState;
extern float lampX, lampY, lampZ;

// Debug globals
extern int debugMode;
extern int debugObjectIndex;
extern int totalObjects;

// Scene object definition
typedef struct {
    int id;
    char name[32];
    float x, y, z;
    float scale;
    float rotation;
    float bbox[6];                // xmin, xmax, ymin, ymax, zmin, zmax
    void (*drawFunc)(float, float);
    int movable;
    bool solid;
} SceneObject;

// Scene object management
#define MAX_OBJECTS 100

extern SceneObject objects[MAX_OBJECTS];
extern int objectCount;
extern bool collidesWithAnyObject(SceneObject* movingObj, float newX, float newZ);

extern SceneObject* selectedObject;
extern int dragging;

bool collidesWithAnyObject(SceneObject* movingObj, float newX, float newZ);

// Mouse interaction
void mouse_button(int button, int state, int x, int y);
void mouse_motion(int x, int y);
void screenToWorld(int x, int y, float* worldX, float* worldZ);
SceneObject* pickObject(float worldX, float worldZ);

// Texture handles
extern unsigned int screenTex;
extern unsigned int cocktailTableTex;
extern unsigned int tableTex;
extern unsigned int stageTex;
extern unsigned int lampRodTex;
extern unsigned int lampShadeTex;
extern unsigned int chairCushionTex;
extern unsigned int chairLegTex;
extern unsigned int doorFrameTex;
extern unsigned int doorKnobTex;
extern unsigned int cloudTex;
extern unsigned int moonTex;
extern unsigned int starTex;
extern unsigned int threadTex;
extern unsigned int meetingTableTex;
extern unsigned int meetingTableLegTex;
extern unsigned int cocktail2Tex;
extern unsigned int cocktail2LegTex;
extern unsigned int cocktail3Tex;
extern unsigned int cocktail3LegTex;
extern unsigned int barChairCushionTex;
extern unsigned int barChairWoodTex;
extern unsigned int barChairBackTex;

// Interaction utilities
void rotateObject(SceneObject* obj, float angle);
void deselectObject(void);

#ifdef __cplusplus
}
#endif

#endif
