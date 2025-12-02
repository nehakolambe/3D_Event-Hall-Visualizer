#include "CSCIx229.h"

// Check if the mouse ray hits a sub-bounding-box of an object
static int rayIntersectsSubBoxWorld(
    float rayOriginX, float rayOriginY, float rayOriginZ,
    float rayDirX, float rayDirY, float rayDirZ,
    const SceneObject *sceneObject,
    int boxIndex,
    float *intersectionDistanceOut)
{
    float entryDistance = -1e9f;
    float exitDistance = +1e9f;

    for (int axis = 0; axis < 3; axis++)
    {
        // Compute min/max of the box along this axis in world coordinates
        float boxMin = sceneObject->subBox[boxIndex][axis * 2] +
                       (axis == 0 ? sceneObject->x : axis == 1 ? sceneObject->y
                                                               : sceneObject->z);

        float boxMax = sceneObject->subBox[boxIndex][axis * 2 + 1] +
                       (axis == 0 ? sceneObject->x : axis == 1 ? sceneObject->y
                                                               : sceneObject->z);

        // Ray origin and direction on this axis
        float rayOriginAxis = (axis == 0 ? rayOriginX : axis == 1 ? rayOriginY
                                                                  : rayOriginZ);
        float rayDirAxis = (axis == 0 ? rayDirX : axis == 1 ? rayDirY
                                                            : rayDirZ);

        // If ray is parallel to the slab
        if (fabsf(rayDirAxis) < 1e-6f)
        {
            if (rayOriginAxis < boxMin || rayOriginAxis > boxMax)
                return 0;
        }
        else
        {
            float entryCandidate = (boxMin - rayOriginAxis) / rayDirAxis;
            float exitCandidate = (boxMax - rayOriginAxis) / rayDirAxis;

            if (entryCandidate > exitCandidate)
            {
                float swappedValue = entryCandidate;
                entryCandidate = exitCandidate;
                exitCandidate = swappedValue;
            }

            if (entryCandidate > entryDistance)
                entryDistance = entryCandidate;
            if (exitCandidate < exitDistance)
                exitDistance = exitCandidate;

            // If range is invalid, no intersection
            if (entryDistance > exitDistance || exitDistance < 0)
                return 0;
        }
    }

    *intersectionDistanceOut = entryDistance;
    return 1;
}

// Convert the mouse (x,y) into a 3D ray in world space
static void getRayFromMouse(int mouseX, int mouseY,
                            float *rayOriginX, float *rayOriginY, float *rayOriginZ,
                            float *rayDirX, float *rayDirY, float *rayDirZ)
{
    GLdouble modelMatrix[16], projectionMatrix[16];
    GLint viewport[4];

    // Read the current camera matrices and viewport so we can unproject screen coords
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLdouble nearPointX, nearPointY, nearPointZ;
    GLdouble farPointX, farPointY, farPointZ;

    // Unproject the mouse position at the near and far clip planes to build a ray
    gluUnProject((GLdouble)mouseX, (GLdouble)(viewport[3] - mouseY), 0.0,
                 modelMatrix, projectionMatrix, viewport,
                 &nearPointX, &nearPointY, &nearPointZ);
    gluUnProject((GLdouble)mouseX, (GLdouble)(viewport[3] - mouseY), 1.0,
                 modelMatrix, projectionMatrix, viewport,
                 &farPointX, &farPointY, &farPointZ);

    *rayOriginX = (float)nearPointX;
    *rayOriginY = (float)nearPointY;
    *rayOriginZ = (float)nearPointZ;

    *rayDirX = (float)(farPointX - nearPointX);
    *rayDirY = (float)(farPointY - nearPointY);
    *rayDirZ = (float)(farPointZ - nearPointZ);
}

// Object picking function
SceneObject *pickObject3D(int mouseX, int mouseY)
{
    float rayOriginX, rayOriginY, rayOriginZ, rayDirX, rayDirY, rayDirZ;
    getRayFromMouse(mouseX, mouseY,
                    &rayOriginX, &rayOriginY, &rayOriginZ,
                    &rayDirX, &rayDirY, &rayDirZ);

    // Track the closest object hit by the ray so we can select it
    SceneObject *closestObject = NULL;
    float closestHitDistance = 1e9f;

    for (int objectIndex = 0; objectIndex < objectCount; objectIndex++)
    {
        SceneObject *sceneObject = &objects[objectIndex];
        // Ignore static objects
        if (!sceneObject->movable)
            continue;

        for (int subBoxIndex = 0; subBoxIndex < sceneObject->subBoxCount; subBoxIndex++)
        {
            float subBoxHitDistance;
            if (rayIntersectsSubBoxWorld(rayOriginX, rayOriginY, rayOriginZ,
                                         rayDirX, rayDirY, rayDirZ,
                                         sceneObject, subBoxIndex,
                                         &subBoxHitDistance))
            {
                if (subBoxHitDistance < closestHitDistance)
                {
                    closestHitDistance = subBoxHitDistance;
                    closestObject = sceneObject;
                }
            }
        }
    }

    return closestObject;
}

// Mouse button callback
void mouse_button(int button, int state, int mouseX, int mouseY)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            SceneObject *pickedObject = pickObject3D(mouseX, mouseY);

            if (pickedObject)
            {
                if (selectedObject == pickedObject)
                {
                    // Clicking the selected object starts dragging it
                    dragging = 1;
                }
                else
                {
                    // Pick a new object and start dragging immediately
                    selectedObject = pickedObject;
                    dragging = 1;
                    printf("Selected object: %s\n", selectedObject->name);
                }
            }
            else
            {
                // Clicked empty space, so deselect everything
                selectedObject = NULL;
                dragging = 0;
                printf("Deselected all objects.\n");
            }
        }
        else if (state == GLUT_UP)
        {
            // Mouse release stops dragging
            dragging = 0;
            printf("Object placed.\n");
        }
    }

    glutPostRedisplay();
}

// Ray-plane intersection helper
static int rayPlaneIntersection(float rayOriginX, float rayOriginY, float rayOriginZ,
                                float rayDirX, float rayDirY, float rayDirZ,
                                float *planeHitX, float *planeHitZ)
{
    // Ignore rays parallel to the ground plane
    if (fabsf(rayDirY) < 1e-6f)
        return 0;

    float t = -rayOriginY / rayDirY;
    if (t < 0)
        return 0;

    // Compute the point where the ray intersects the ground plane (y=0)
    *planeHitX = rayOriginX + t * rayDirX;
    *planeHitZ = rayOriginZ + t * rayDirZ;
    return 1;
}

// Mouse motion callback
void mouse_motion(int mouseX, int mouseY)
{
    if (dragging && selectedObject)
    {
        float rayOriginX, rayOriginY, rayOriginZ, rayDirX, rayDirY, rayDirZ;
        getRayFromMouse(mouseX, mouseY,
                        &rayOriginX, &rayOriginY, &rayOriginZ,
                        &rayDirX, &rayDirY, &rayDirZ);

        float planeHitX, planeHitZ;
        if (rayPlaneIntersection(rayOriginX, rayOriginY, rayOriginZ,
                                 rayDirX, rayDirY, rayDirZ,
                                 &planeHitX, &planeHitZ))
        {
            float newX = planeHitX;
            float newZ = planeHitZ;

            if (snapToGridEnabled && scene_object_supports_snap(selectedObject))
                // Snap furniture to the grid if supported
                scene_snap_position(&newX, &newZ);

            // room boundaries
            if (newX < ROOM_MIN_X)
                newX = ROOM_MIN_X;
            if (newX > ROOM_MAX_X)
                newX = ROOM_MAX_X;
            if (newZ < ROOM_MIN_Z)
                newZ = ROOM_MIN_Z;
            if (newZ > ROOM_MAX_Z)
                newZ = ROOM_MAX_Z;

            // object–object collision
            if (!collidesWithAnyObject(selectedObject, newX, newZ, false, true))
            {
                // Update the object's location once it is valid
                selectedObject->x = newX;
                selectedObject->z = newZ;
                scene_apply_stage_height(selectedObject);
            }
        }
    }

    glutPostRedisplay();
}
