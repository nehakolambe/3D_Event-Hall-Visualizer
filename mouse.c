#include "CSCIx229.h"

// Check if the invisible line from the mouse hits a specific box part of an object
static int rayIntersectsSubBoxWorld(
    float rayOriginX, float rayOriginY, float rayOriginZ,
    float rayDirX, float rayDirY, float rayDirZ,
    const SceneObject *sceneObject,
    int boxIndex,
    float *intersectionDistanceOut)
{
    float entryDistance = -1e9f; // When the ray enters the box
    float exitDistance = +1e9f;  // When the ray leaves the box

    // Check all 3 dimensions: 0=Width(X), 1=Height(Y), 2=Depth(Z)
    for (int axisIndex = 0; axisIndex < 3; axisIndex++)
    {
        // Calculate the world position of the box edges
        float boxMin = sceneObject->subBox[boxIndex][axisIndex * 2] +
                       (axisIndex == 0 ? sceneObject->x : axisIndex == 1 ? sceneObject->y
                                                                         : sceneObject->z);

        float boxMax = sceneObject->subBox[boxIndex][axisIndex * 2 + 1] +
                       (axisIndex == 0 ? sceneObject->x : axisIndex == 1 ? sceneObject->y
                                                                         : sceneObject->z);

        // Get the ray's start point and direction
        float rayOriginAxis = (axisIndex == 0 ? rayOriginX : axisIndex == 1 ? rayOriginY
                                                                            : rayOriginZ);
        float rayDirAxis = (axisIndex == 0 ? rayDirX : axisIndex == 1 ? rayDirY
                                                                      : rayDirZ);

        // Case 1: The ray is flat/parallel to this side of the box
        if (fabsf(rayDirAxis) < 1e-6f)
        {
            // If the ray starts outside the box boundaries, it can never hit it
            if (rayOriginAxis < boxMin || rayOriginAxis > boxMax)
                return 0; // Missed
        }
        else
        {
            // Case 2: The ray is angled
            float entryCandidate = (boxMin - rayOriginAxis) / rayDirAxis;
            float exitCandidate = (boxMax - rayOriginAxis) / rayDirAxis;

            // Ensure entry is smaller than exit
            if (entryCandidate > exitCandidate)
            {
                float swapTemp = entryCandidate;
                entryCandidate = exitCandidate;
                exitCandidate = swapTemp;
            }

            // Tighten the window
            if (entryCandidate > entryDistance)
                entryDistance = entryCandidate;
            if (exitCandidate < exitDistance)
                exitDistance = exitCandidate;

            // If the entry is after the exit, the ray missed the box entirely
            // Or if the exit is behind us (negative), the box is behind the camera
            if (entryDistance > exitDistance || exitDistance < 0)
                return 0; // Missed
        }
    }

    // We hit the box
    *intersectionDistanceOut = entryDistance;
    return 1;
}

// Convert the 2D mouse click (x,y) into a 3D line (Ray)
static void getRayFromMouse(int mouseX, int mouseY,
                            float *rayOriginX, float *rayOriginY, float *rayOriginZ,
                            float *rayDirX, float *rayDirY, float *rayDirZ)
{
    GLdouble modelMatrix[16], projectionMatrix[16];
    GLint viewport[4];

    // Get current camera setup and window size
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLdouble nearPointX, nearPointY, nearPointZ;
    GLdouble farPointX, farPointY, farPointZ;

    // Unproject the 2D mouse point into 3D space at Near and Far planes
    gluUnProject((GLdouble)mouseX, (GLdouble)(viewport[3] - mouseY), 0.0,
                 modelMatrix, projectionMatrix, viewport,
                 &nearPointX, &nearPointY, &nearPointZ);
    gluUnProject((GLdouble)mouseX, (GLdouble)(viewport[3] - mouseY), 1.0,
                 modelMatrix, projectionMatrix, viewport,
                 &farPointX, &farPointY, &farPointZ);

    // Ray starts at the near point (the screen surface)
    *rayOriginX = (float)nearPointX;
    *rayOriginY = (float)nearPointY;
    *rayOriginZ = (float)nearPointZ;

    // Ray direction is the vector pointing from Near to Far
    *rayDirX = (float)(farPointX - nearPointX);
    *rayDirY = (float)(farPointY - nearPointY);
    *rayDirZ = (float)(farPointZ - nearPointZ);
}

// Helper function to check which object the user clicked on
SceneObject *pickObject3D(int mouseX, int mouseY)
{
    // Turn mouse click into a 3D ray
    float rayOriginX, rayOriginY, rayOriginZ, rayDirX, rayDirY, rayDirZ;
    getRayFromMouse(mouseX, mouseY,
                    &rayOriginX, &rayOriginY, &rayOriginZ,
                    &rayDirX, &rayDirY, &rayDirZ);

    SceneObject *closestObject = NULL;
    float closestHitDistance = 1e9f;

    // Loop through every object in the scene
    for (int objectIndex = 0; objectIndex < objectCount; objectIndex++)
    {
        SceneObject *sceneObject = &objects[objectIndex];

        // Skip objects we aren't allowed to move
        if (!sceneObject->movable)
            continue;

        // Check every part of this object
        for (int subBoxIndex = 0; subBoxIndex < sceneObject->subBoxCount; subBoxIndex++)
        {
            float subBoxHitDistance;
            // Check if the ray hits the bounding box
            if (rayIntersectsSubBoxWorld(rayOriginX, rayOriginY, rayOriginZ,
                                         rayDirX, rayDirY, rayDirZ,
                                         sceneObject, subBoxIndex,
                                         &subBoxHitDistance))
            {
                // If hit, check if it's closer than anything
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

// Called by the system when the mouse button is clicked
void mouse_button(int button, int state, int mouseX, int mouseY)
{
    // If left button is clicked
    if (button == GLUT_LEFT_BUTTON)
    {
        // When the button is pressed down
        if (state == GLUT_DOWN)
        {
            // Run the picking logic
            SceneObject *pickedObject = pickObject3D(mouseX, mouseY);

            if (pickedObject)
            {
                if (selectedObject == pickedObject)
                {
                    // If we clicked the object we already have, just grab it
                    dragging = 1;
                }
                else
                {
                    // If we clicked a new object, switch selection to it
                    selectedObject = pickedObject;
                    dragging = 1;
                    printf("Selected object: %s\n", selectedObject->name);
                }
            }
            else
            {
                // If we clicked in air, drop the selection
                selectedObject = NULL;
                dragging = 0;
                printf("Deselected all objects.\n");
            }
        }
        else if (state == GLUT_UP)
        {
            // When button is released, stop dragging
            dragging = 0;
            printf("Object placed.\n");
        }
    }

    // Redraw the screen
    glutPostRedisplay();
}

// Helper function to find where the mouse ray hits the floor plane
static int rayPlaneIntersection(float rayOriginX, float rayOriginY, float rayOriginZ,
                                float rayDirX, float rayDirY, float rayDirZ,
                                float *planeHitX, float *planeHitZ)
{
    // If ray is horizontal, it will never hit the floor
    if (fabsf(rayDirY) < 1e-6f)
        return 0;

    float rayParameter = -rayOriginY / rayDirY;

    // If the hit point is behind the camera, ignore it
    if (rayParameter < 0)
        return 0;

    // Find the X and Z coordinates on the floor
    *planeHitX = rayOriginX + rayParameter * rayDirX;
    *planeHitZ = rayOriginZ + rayParameter * rayDirZ;
    return 1;
}

// Called by system when the mouse moves
void mouse_motion(int mouseX, int mouseY)
{
    // Only move things if we are currently dragging a valid object
    if (dragging && selectedObject)
    {
        // Get the new ray for the current mouse position
        float rayOriginX, rayOriginY, rayOriginZ, rayDirX, rayDirY, rayDirZ;
        getRayFromMouse(mouseX, mouseY,
                        &rayOriginX, &rayOriginY, &rayOriginZ,
                        &rayDirX, &rayDirY, &rayDirZ);

        float planeHitX, planeHitZ;
        // Find where the mouse is pointing on the floor
        if (rayPlaneIntersection(rayOriginX, rayOriginY, rayOriginZ,
                                 rayDirX, rayDirY, rayDirZ,
                                 &planeHitX, &planeHitZ))
        {
            float newX = planeHitX;
            float newZ = planeHitZ;

            // If Grid Snap is on, round the coordinates
            if (snapToGridEnabled && scene_object_supports_snap(selectedObject))
                scene_snap_position(&newX, &newZ);

            // Keep the object inside the room walls
            if (newX < ROOM_MIN_X)
                newX = ROOM_MIN_X;
            if (newX > ROOM_MAX_X)
                newX = ROOM_MAX_X;
            if (newZ < ROOM_MIN_Z)
                newZ = ROOM_MIN_Z;
            if (newZ > ROOM_MAX_Z)
                newZ = ROOM_MAX_Z;

            // Before applying the move, check if it hits any other furniture
            if (!collidesWithAnyObject(selectedObject, newX, newZ, false, true))
            {
                // Update the object's position
                selectedObject->x = newX;
                selectedObject->z = newZ;

                // Adjust height if we moved onto a stage
                scene_apply_stage_height(selectedObject);
            }
        }
    }

    // Redraw screen
    glutPostRedisplay();
}
