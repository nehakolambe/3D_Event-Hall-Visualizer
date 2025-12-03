#include "CSCIx229.h"

// Keyboard Input Handler
void controls_key(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;
    const double speed = 0.8;
    const double yawDegrees = yaw;

    switch (key)
    {
    // Toggle dynamic light motion
    case 'l':
    case 'L':
        movingLightEnabled = !movingLightEnabled;
        break;

    // Toggle snap-to-grid
    case 'g':
    case 'G':
        snapToGridEnabled = !snapToGridEnabled;
        printf("Snap-to-grid %s.\n", snapToGridEnabled ? "enabled" : "disabled");
        if (snapToGridEnabled)
            scene_snap_all_objects();
        break;

    // Cycle through light modes
    case 'b':
    case 'B':
        lightState = (lightState + 1) % 3;
        break;

    // Toggle bounding box highlight
    case '`':
        bboxHighlightEnabled = !bboxHighlightEnabled;
        printf("Bounding box highlight %s.\n", bboxHighlightEnabled ? "enabled" : "disabled");
        break;

    // Rotate light horizontally (left)
    case '[':
        movingLightAngle -= 5;
        break;

    // Rotate light horizontally (right)
    case ']':
        movingLightAngle += 5;
        break;

    // Shrink light radius
    case '{':
        movingLightRadius -= 1;
        break;

    // Expand light radius
    case '}':
        movingLightRadius += 1;
        if (movingLightRadius > 40)
            movingLightRadius = 40;
        break;

    // Move light down
    case 'y':
        movingLightHeight -= 0.5f;
        break;

    // Move light up
    case 'Y':
        movingLightHeight += 0.5f;
        if (movingLightHeight > 30)
            movingLightHeight = 30;
        break;

    // Save scene layout
    case '/':
        save_scene("layout.csv");
        break;

    // Load scene layout
    case '?':
        load_scene("layout.csv");
        break;

    // Increase FOV (zoom out)
    case '-':
    case '_':
        fov += 2.0;
        if (fov > 80.0)
            fov = 80.0;
        break;

    // Decrease FOV (zoom in)
    case '+':
    case '=':
        fov -= 2.0;
        if (fov < 25.0)
            fov = 25.0;
        break;

    // Move camera backward (prespective and orthogonal modes)
    case '(':
        if (mode != 1)
        {
            dim -= 1.0;
            if (dim < 5.0)
                dim = 5.0;
        }
        break;

    // Move camera forward (prespective and orthogonal modes)
    case ')':
        if (mode != 1)
        {
            dim += 1.0;
            if (dim > 60.0)
                dim = 60.0;
        }
        break;

    // FPV forward
    case 'w':
    case 'W':
        if (mode == 1)
        {
            float newX = fpvX + speed * Sin(yawDegrees);
            float newZ = fpvZ - speed * Cos(yawDegrees);

            playerObj.y = fpvY - 1.6f;
            playerObj.x = fpvX;
            playerObj.z = fpvZ;

            if (!collidesWithAnyObject(&playerObj, newX, newZ, true, true))
            {
                fpvX = newX;
                fpvZ = newZ;
            }
        }
        break;

    // FPV backward
    case 's':
    case 'S':
        if (mode == 1)
        {
            float newX = fpvX - speed * Sin(yawDegrees);
            float newZ = fpvZ + speed * Cos(yawDegrees);

            playerObj.y = fpvY - 1.6f;
            playerObj.x = fpvX;
            playerObj.z = fpvZ;

            if (!collidesWithAnyObject(&playerObj, newX, newZ, true, true))
            {
                fpvX = newX;
                fpvZ = newZ;
            }
        }
        break;

    // FPV left
    case 'a':
    case 'A':
        if (mode == 1)
        {
            float newX = fpvX - speed * Cos(yawDegrees);
            float newZ = fpvZ - speed * Sin(yawDegrees);

            playerObj.y = fpvY - 1.6f;
            playerObj.x = fpvX;
            playerObj.z = fpvZ;

            if (!collidesWithAnyObject(&playerObj, newX, newZ, true, true))
            {
                fpvX = newX;
                fpvZ = newZ;
            }
        }
        break;

    // FPV right
    case 'd':
    case 'D':
        if (mode == 1)
        {
            float newX = fpvX + speed * Cos(yawDegrees);
            float newZ = fpvZ + speed * Sin(yawDegrees);

            playerObj.y = fpvY - 1.6f;
            playerObj.x = fpvX;
            playerObj.z = fpvZ;

            if (!collidesWithAnyObject(&playerObj, newX, newZ, true, true))
            {
                fpvX = newX;
                fpvZ = newZ;
            }
        }
        break;

    // Toggle FPV / Prespective / Orthogonal mode
    case 'm':
    case 'M':
        mode = (mode + 1) % 3;
        break;

    // Rotate selected object clockwise
    case 'r':
        if (selectedObject)
            rotateObject(selectedObject, 15.0f);
        break;

    // Rotate selected object counter-clockwise
    case 'R':
        if (selectedObject)
            rotateObject(selectedObject, -15.0f);
        break;

    // Remove selected object
    case 'q':
    case 'Q':
        scene_remove_selected_object();
        break;

    // Spawn objects on ground
    case '1':
        scene_spawn_object(SPAWN_LAMP);
        break;
    case '2':
        scene_spawn_object(SPAWN_EVENT_TABLE);
        break;
    case '3':
        scene_spawn_object(SPAWN_MEETING_TABLE);
        break;
    case '4':
        scene_spawn_object(SPAWN_BAR_CHAIR);
        break;
    case '5':
        scene_spawn_object(SPAWN_BANQUET_CHAIR);
        break;
    case '6':
        scene_spawn_object(SPAWN_COCKTAIL_1);
        break;
    case '7':
        scene_spawn_object(SPAWN_COCKTAIL_2);
        break;
    case '8':
        scene_spawn_object(SPAWN_COCKTAIL_3);
        break;

    // Reset scene
    case '0':
        th = ph = yaw = pitch = 0;
        camZ = 24;
        fpvX = 0;
        fpvZ = 24;
        fov = 55;
        dim = 20;
        movingLightEnabled = 1;
        movingLightRadius = 10.0f;
        movingLightAngle = 0;
        movingLightHeight = 5.0f;
        mode = 0;
        selectedObject = NULL;
        break;

    // Exit program (ESC)
    case 27:
        exit(0);
    }

    glutPostRedisplay();
}

// Special Keys (Arrow Keys)
void controls_special(int key, int x, int y)
{
    (void)x;
    (void)y;

    if (mode == 1)
    {
        // FPV look controls
        switch (key)
        {
        case GLUT_KEY_LEFT:
            yaw -= 5;
            break;
        case GLUT_KEY_RIGHT:
            yaw += 5;
            break;

        case GLUT_KEY_UP:
            pitch += 5;
            if (pitch > 60)
                pitch = 60;
            break;

        case GLUT_KEY_DOWN:
            pitch -= 5;
            if (pitch < -60)
                pitch = -60;
            break;
        }
    }
    else
    {
        // prespective camera rotation
        switch (key)
        {
        case GLUT_KEY_RIGHT:
            th += 5;
            break;
        case GLUT_KEY_LEFT:
            th -= 5;
            break;

        case GLUT_KEY_UP:
            ph += 5;
            if (ph > 90)
                ph = 90;
            break;

        case GLUT_KEY_DOWN:
            ph -= 5;
            if (ph < -90)
                ph = -90;
            break;
        }
    }

    th = fmod(th, 360.0);
    glutPostRedisplay();
}
