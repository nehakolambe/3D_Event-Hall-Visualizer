#ifndef CSCIx229
#define CSCIx229

// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

// GLEW header
#ifdef USEGLEW
#include <GL/glew.h>
#endif

// OpenGL extension prototypes
#define GL_GLEXT_PROTOTYPES

// Select SDL, SDL2, GLFW or GLUT
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

// GLU and OpenGL headers
#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

// Utility macros
#define Cos(th) cos(3.14159265 / 180 * (th))
#define Sin(th) sin(3.14159265 / 180 * (th))
#define PI 3.14159265

// Room bounds for placement/dragging logic
#define ROOM_MIN_X -18.0f
#define ROOM_MAX_X 18.0f
#define ROOM_MIN_Z -28.0f
#define ROOM_MAX_Z 28.0f

#ifdef __cplusplus
extern "C"
{
#endif

// Formatted printing and fatal error helpers
#ifdef __GNUC__
    void Print(const char *format, ...) __attribute__((format(printf, 1, 2)));
    void Fatal(const char *format, ...) __attribute__((format(printf, 1, 2))) __attribute__((noreturn));
#else
void Print(const char *format, ...);
void Fatal(const char *format, ...);
#endif

    // Texture loading and GL error check utilities
    unsigned int LoadTexBMP(const char *file);
    void ErrCheck(const char *where);

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
    void drawCeilingShapes(void);
    void drawFireplace(float x, float z);
    void drawFirePlane(void);

    // Controls
    void controls_key(unsigned char ch, int x, int y);
    void controls_special(int key, int x, int y);

    // Lighting
    void lighting_init(void);
    void lighting_update(void);
    void lighting_draw_debug_marker(void);

    int CreateShaderProg(char *VertFile, char *FragFile);

    extern int lightState;
    extern float zh;
    extern float lightSpeed;
    extern float radius;
    extern int moveLight;
    extern float lightY;

    extern int mode; // 0 = perspective, 1 = FPV, 2 = orthogonal
    extern double fpvX, fpvY, fpvZ;
    extern double yaw, pitch;

    // External camera state
    extern double fov;
    extern double th, ph;
    extern double camZ;
    extern double dim;
    extern int screenWidth;
    extern int screenHeight;

// Scene object definition
#define MAX_SUBBOXES 8
    typedef struct
    {
        int id;
        char name[32];
        float x, y, z;
        float scale;
        float rotation;
        void (*drawFunc)(float, float);
        int movable;
        bool solid;
        int subBoxCount;
        float subBox[MAX_SUBBOXES][6]; // each is {xmin, xmax, ymin, ymax, zmin, zmax}
    } SceneObject;

    typedef enum
    {
        SPAWN_LAMP = 0,
        SPAWN_EVENT_TABLE,
        SPAWN_MEETING_TABLE,
        SPAWN_BAR_CHAIR,
        SPAWN_BANQUET_CHAIR,
        SPAWN_COCKTAIL_1,
        SPAWN_COCKTAIL_2,
        SPAWN_COCKTAIL_3,
        SPAWN_TYPE_COUNT
    } SceneSpawnType;

// Scene object management
#define MAX_OBJECTS 100

    extern SceneObject objects[MAX_OBJECTS];
    extern int objectCount;
    extern bool collidesWithAnyObject(SceneObject *movingObj, float newX, float newZ, bool adjustPlayerHeight, bool allowStageSnap);

    extern SceneObject *selectedObject;
    extern SceneObject playerObj;
    extern int dragging;
    extern bool snapToGridEnabled;
    extern bool bboxHighlightEnabled;
    int scene_object_supports_snap(const SceneObject *obj);
    void scene_snap_position(float *x, float *z);
    void scene_snap_all_objects(void);
    void scene_apply_stage_height(SceneObject *obj);

    // Mouse interaction
    void mouse_button(int button, int state, int mouseX, int mouseY);
    void mouse_motion(int mouseX, int mouseY);

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
    extern unsigned int fireplaceTex;
    extern unsigned int fireNoiseTex;

    // geometry utilities
    void drawCuboid(float width, float height, float depth);
    void drawDisk(float radius, float y, float thickness);
    void drawSphere(float radius, int slices, int stacks);
    void drawCylinder(float radius, float height, int slices);
    void drawFrustum(float bottomRadius, float topRadius, float height, int slices);

    // Interaction utilities
    void rotateObject(SceneObject *obj, float angle);
    SceneObject *scene_spawn_object(SceneSpawnType type);
    void scene_remove_selected_object(void);

    // Player collision
    void initPlayerCollision(void);

    // Whiteboard overlay
    extern int whiteboardMode;
    void whiteboard_activate(void);
    void whiteboard_deactivate(void);
    void whiteboard_clear(void);
    void whiteboard_add_stroke(float u1, float v1, float u2, float v2, int erase);
    void whiteboard_render(float x, float y, float width, float height);
    void whiteboard_get_canvas_rect(float *x, float *y, float *width, float *height);
    int whiteboard_point_in_canvas(float sx, float sy);
    void whiteboard_screen_to_canvas(float sx, float sy, float *u, float *v);
    void whiteboard_background_invalidate(void);
    int whiteboard_background_ready(void);
    void whiteboard_capture_background(int width, int height);
    void whiteboard_draw_background(int width, int height);
    void whiteboard_render_on_board(float boardBottom, float boardTop, float boardLeft, float boardRight);
    void drawWhiteboardOverlay(void);
    void drawWhiteboardTrigger(float x, float z);

    // Whiteboard stroke data structure
    typedef struct
    {
        float u1, v1;
        float u2, v2;
        int erase;
    } Stroke;

    // Save/Load functions
    void save_scene(const char *filename);
    void load_scene(const char *filename);

    // Accessors to read private whiteboard data
    int whiteboard_get_stroke_count(void);
    Stroke whiteboard_get_stroke(int index);

#ifdef __cplusplus
}
#endif

#endif
