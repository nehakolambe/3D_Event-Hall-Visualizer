#include "CSCIx229.h"

// Object collision + rotation helpers
// Check AABB overlap
bool collidesWithAnyObject(SceneObject *movingObj, float newX, float newZ)
{
    // Loop through all objects
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *other = &objects[i];

        // Skip self & non-solid objects
        if (other == movingObj || !other->solid)
            continue;

        // For every sub-box of the moving object
        for (int a = 0; a < movingObj->subBoxCount; a++)
        {
            float a_xmin = newX + movingObj->subBox[a][0];
            float a_xmax = newX + movingObj->subBox[a][1];
            float a_ymin = movingObj->y + movingObj->subBox[a][2];
            float a_ymax = movingObj->y + movingObj->subBox[a][3];
            float a_zmin = newZ + movingObj->subBox[a][4];
            float a_zmax = newZ + movingObj->subBox[a][5];

            // Compare with each sub-box of the other object
            for (int b = 0; b < other->subBoxCount; b++)
            {
                float b_xmin = other->x + other->subBox[b][0];
                float b_xmax = other->x + other->subBox[b][1];
                float b_ymin = other->y + other->subBox[b][2];
                float b_ymax = other->y + other->subBox[b][3];
                float b_zmin = other->z + other->subBox[b][4];
                float b_zmax = other->z + other->subBox[b][5];

                // 3D AABB overlap test
                if (a_xmax > b_xmin && a_xmin < b_xmax &&
                    a_ymax > b_ymin && a_ymin < b_ymax &&
                    a_zmax > b_zmin && a_zmin < b_zmax)
                {
                    return true; // collision detected!
                }
            }
        }
    }

    return false; // no collision
}

// Rotate object around its Y-axis
void rotateObject(SceneObject *obj, float angle)
{
    if (!obj)
        return;

    obj->rotation += angle;

    // Keep in 0–360 range
    if (obj->rotation >= 360.0f)
        obj->rotation -= 360.0f;
    if (obj->rotation < 0.0f)
        obj->rotation += 360.0f;
}