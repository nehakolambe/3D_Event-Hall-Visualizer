#include "CSCIx229.h"

// Object collision + rotation helpers
// Check AABB overlap
bool collidesWithAnyObject(SceneObject *movingObj, float newX, float newZ)
{
    float minX = newX + movingObj->bbox[0];
    float maxX = newX + movingObj->bbox[1];
    float minZ = newZ + movingObj->bbox[4];
    float maxZ = newZ + movingObj->bbox[5];

    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *other = &objects[i];
        if (other == movingObj || !other->solid)
            continue;

        float oMinX = other->x + other->bbox[0];
        float oMaxX = other->x + other->bbox[1];
        float oMinZ = other->z + other->bbox[4];
        float oMaxZ = other->z + other->bbox[5];

        // Overlap test
        if (maxX > oMinX && minX < oMaxX &&
            maxZ > oMinZ && minZ < oMaxZ)
        {
            return true; // Collision found
        }
    }

    return false; // No collision
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