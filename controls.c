#include "CSCIx229.h"

static SceneObject playerObj;

// Keyboard Input Handler
void controls_key(unsigned char key, int x, int y)
{
    const double speed = 0.8;
    const double radYaw = yaw * (M_PI / 180.0);

    if (whiteboardMode)
    {
        if (key == 27)
        {
            whiteboard_deactivate();
            glutPostRedisplay();
        }
        return;
    }

    switch (key)
    {
        // Toggle dynamic light motion
        case 'l':
        case 'L':
            moveLight = !moveLight;
            break;

        // Cycle through light modes
        case 'b':
        case 'B':
            lightState = (lightState + 1) % 3;
            break;

        // Rotate light horizontally (left)
        case '[':
            zh -= 5;
            break;

        // Rotate light horizontally (right)
        case ']':
            zh += 5;
            break;

        // Shrink light radius
        case '{':
            radius -= 1;
            break;

        // Expand light radius
        case '}':
            radius += 1;
            if (radius > 40) radius = 40;
            break;

        // Move light down
        case 'y':
            lightY -= 0.5f;
            break;

        // Move light up
        case 'Y':
            lightY += 0.5f;
            if (lightY > 30) lightY = 30;
            break;

        // Increase FOV (zoom out)
        case '-':
        case '_':
            fov += 2.0;
            if (fov > 80.0) fov = 80.0;
            break;

        // Decrease FOV (zoom in)
        case '+':
        case '=':
            fov -= 2.0;
            if (fov < 25.0) fov = 25.0;
            break;

        // Move camera backward (projection mode only)
        case '(':
            if (mode == 0)
            {
                dim -= 1.0;
                if (dim < 5.0) dim = 5.0;
            }
            break;

        // Move camera forward (projection mode only)
        case ')':
            if (mode == 0)
            {
                dim += 1.0;
                if (dim > 60.0) dim = 60.0;
            }
            break;

        // FPV forward
        case 'w':
        case 'W':
            if (mode == 1)
            {
                float newX = fpvX + speed * sin(radYaw);
                float newZ = fpvZ - speed * cos(radYaw);

                playerObj.y = fpvY - 1.6f;
                playerObj.x = fpvX;
                playerObj.z = fpvZ;

                if (!collidesWithAnyObject(&playerObj, newX, newZ))
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
                float newX = fpvX - speed * sin(radYaw);
                float newZ = fpvZ + speed * cos(radYaw);

                playerObj.y = fpvY - 1.6f;
                playerObj.x = fpvX;
                playerObj.z = fpvZ;

                if (!collidesWithAnyObject(&playerObj, newX, newZ))
                {
                    fpvX = newX;
                    fpvZ = newZ;
                }
            }
            break;

        // FPV strafe left
        case 'a':
        case 'A':
            if (mode == 1)
            {
                float newX = fpvX - speed * cos(radYaw);
                float newZ = fpvZ - speed * sin(radYaw);

                playerObj.y = fpvY - 1.6f;
                playerObj.x = fpvX;
                playerObj.z = fpvZ;

                if (!collidesWithAnyObject(&playerObj, newX, newZ))
                {
                    fpvX = newX;
                    fpvZ = newZ;
                }
            }
            break;

        // FPV strafe right
        case 'd':
        case 'D':
            if (mode == 1)
            {
                float newX = fpvX + speed * cos(radYaw);
                float newZ = fpvZ + speed * sin(radYaw);

                playerObj.y = fpvY - 1.6f;
                playerObj.x = fpvX;
                playerObj.z = fpvZ;

                if (!collidesWithAnyObject(&playerObj, newX, newZ))
                {
                    fpvX = newX;
                    fpvZ = newZ;
                }
            }
            break;

        // Toggle FPV / Projection / Orthogonal mode
        case 'm':
        case 'M':
            mode = (mode + 1) % 3;
            break;

        // Rotate selected object clockwise
        case 'r':
            if (selectedObject) rotateObject(selectedObject, 15.0f);
            break;

        // Rotate selected object counter-clockwise
        case 'R':
            if (selectedObject) rotateObject(selectedObject, -15.0f);
            break;

        // Reset scene
        case '0':
            th = ph = yaw = pitch = 0;
            camZ = 24;
            fpvX = 0;
            fpvZ = 24;
            fov = 55;
            dim = 20;
            moveLight = 1;
            radius = 10.0f;
            zh = 0;
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
    if (whiteboardMode)
        return;

    if (mode == 1)
    {
        // FPV look controls
        switch (key)
        {
            case GLUT_KEY_LEFT:  yaw -= 5; break;
            case GLUT_KEY_RIGHT: yaw += 5; break;

            case GLUT_KEY_UP:
                pitch += 5;
                if (pitch > 60) pitch = 60;
                break;

            case GLUT_KEY_DOWN:
                pitch -= 5;
                if (pitch < -60) pitch = -60;
                break;
        }
    }
    else
    {
        // Projection camera rotation
        switch (key)
        {
            case GLUT_KEY_RIGHT: th += 5; break;
            case GLUT_KEY_LEFT:  th -= 5; break;

            case GLUT_KEY_UP:
                ph += 5;
                if (ph > 90) ph = 90;
                break;

            case GLUT_KEY_DOWN:
                ph -= 5;
                if (ph < -90) ph = -90;
                break;
        }
    }

    th = fmod(th, 360.0);
    glutPostRedisplay();
}

// Player bounding box
void initPlayerCollision()
{
    playerObj.subBoxCount = 1;

    playerObj.subBox[0][0] = -0.6f;
    playerObj.subBox[0][1] =  0.6f;

    playerObj.subBox[0][2] = 0.0f;
    playerObj.subBox[0][3] = fpvY;

    playerObj.subBox[0][4] = -0.6f;
    playerObj.subBox[0][5] =  0.6f;

    playerObj.solid = 1;
}