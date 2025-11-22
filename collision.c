#include "CSCIx229.h"

// Rotate AABB around Y-axis and compute its world OBB bounds
static void computeRotatedBounds(SceneObject *obj, int boxIndex,
                                 float worldX, float worldZ,
                                 float *minX, float *maxX,
                                 float *minY, float *maxY,
                                 float *minZ, float *maxZ)
{
    float rx = obj->rotation * (M_PI / 180.0f);
    float c = cosf(rx);
    float s = sinf(rx);

    float xmin = obj->subBox[boxIndex][0];
    float xmax = obj->subBox[boxIndex][1];
    float ymin = obj->subBox[boxIndex][2];
    float ymax = obj->subBox[boxIndex][3];
    float zmin = obj->subBox[boxIndex][4];
    float zmax = obj->subBox[boxIndex][5];

    // all 8 corners
    float cx[8] = {xmin, xmax, xmin, xmax, xmin, xmax, xmin, xmax};
    float cy[8] = {ymin, ymin, ymin, ymin, ymax, ymax, ymax, ymax};
    float cz[8] = {zmin, zmin, zmax, zmax, zmin, zmin, zmax, zmax};

    *minX = *minZ = +1e9f;
    *maxX = *maxZ = -1e9f;

    *minY = ymin + obj->y;
    *maxY = ymax + obj->y;

    for (int i = 0; i < 8; i++)
    {
        float x = cx[i];
        float z = cz[i];

        // rotate around Y
        float xr = c * x - s * z;
        float zr = s * x + c * z;

        xr += worldX;
        zr += worldZ;

        if (xr < *minX)
            *minX = xr;
        if (xr > *maxX)
            *maxX = xr;
        if (zr < *minZ)
            *minZ = zr;
        if (zr > *maxZ)
            *maxZ = zr;
    }
}

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
            float a_xmin, a_xmax, a_ymin, a_ymax, a_zmin, a_zmax;

            computeRotatedBounds(movingObj, a, newX, newZ,
                                 &a_xmin, &a_xmax,
                                 &a_ymin, &a_ymax,
                                 &a_zmin, &a_zmax);

            // Compare with each sub-box of the other object
            for (int b = 0; b < other->subBoxCount; b++)
            {
                float b_xmin, b_xmax, b_ymin, b_ymax, b_zmin, b_zmax;

                computeRotatedBounds(other, b, other->x, other->z,
                                     &b_xmin, &b_xmax,
                                     &b_ymin, &b_ymax,
                                     &b_zmin, &b_zmax);

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