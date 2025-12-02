#include "CSCIx229.h"

typedef struct
{
    float centerX;
    float centerZ;
    float halfX;
    float halfZ;
    float axis[2][2]; // [0]=local X axis, [1]=local Z axis
    float minY;
    float maxY;
} BoxOBB;

// Rotate AABB around Y-axis and compute world-space AABB bounds
static void computeRotatedBounds(const SceneObject *obj, int boxIndex,
                                 float worldX, float worldZ,
                                 float *minX, float *maxX,
                                 float *minY, float *maxY,
                                 float *minZ, float *maxZ)
{
    float xmin = obj->subBox[boxIndex][0];
    float xmax = obj->subBox[boxIndex][1];
    float ymin = obj->subBox[boxIndex][2];
    float ymax = obj->subBox[boxIndex][3];
    float zmin = obj->subBox[boxIndex][4];
    float zmax = obj->subBox[boxIndex][5];

    float angle = -obj->rotation * (M_PI / 180.0f);
    float cosYaw = cosf(angle);
    float sinYaw = sinf(angle);

    *minX = *minZ = +1e9f;
    *maxX = *maxZ = -1e9f;

    *minY = ymin + obj->y;
    *maxY = ymax + obj->y;

    for (int cornerXIndex = 0; cornerXIndex < 2; cornerXIndex++)
    {
        float x = (cornerXIndex == 0) ? xmin : xmax;
        for (int cornerZIndex = 0; cornerZIndex < 2; cornerZIndex++)
        {
            float z = (cornerZIndex == 0) ? zmin : zmax;

            float xr = cosYaw * x - sinYaw * z + worldX;
            float zr = sinYaw * x + cosYaw * z + worldZ;

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
}

static void buildBoxOBB(const SceneObject *obj, int boxIndex,
                        float worldX, float worldZ, BoxOBB *box)
{
    float xmin = obj->subBox[boxIndex][0];
    float xmax = obj->subBox[boxIndex][1];
    float ymin = obj->subBox[boxIndex][2];
    float ymax = obj->subBox[boxIndex][3];
    float zmin = obj->subBox[boxIndex][4];
    float zmax = obj->subBox[boxIndex][5];

    float localCenterX = 0.5f * (xmin + xmax);
    float localCenterZ = 0.5f * (zmin + zmax);
    float halfX = 0.5f * (xmax - xmin);
    float halfZ = 0.5f * (zmax - zmin);

    float angle = -obj->rotation * (M_PI / 180.0f);
    float cosYaw = cosf(angle);
    float sinYaw = sinf(angle);

    box->centerX = cosYaw * localCenterX - sinYaw * localCenterZ + worldX;
    box->centerZ = sinYaw * localCenterX + cosYaw * localCenterZ + worldZ;
    box->halfX = halfX;
    box->halfZ = halfZ;
    box->axis[0][0] = cosYaw;
    box->axis[0][1] = sinYaw;
    box->axis[1][0] = -sinYaw;
    box->axis[1][1] = cosYaw;
    box->minY = ymin + obj->y;
    box->maxY = ymax + obj->y;
}

static float projectRadius(const BoxOBB *box, float axisX, float axisZ)
{
    float axisLen = sqrtf(axisX * axisX + axisZ * axisZ);
    if (axisLen < 1e-6f)
        return 0.0f;

    float nx = axisX / axisLen;
    float nz = axisZ / axisLen;

    float dotX = nx * box->axis[0][0] + nz * box->axis[0][1];
    float dotZ = nx * box->axis[1][0] + nz * box->axis[1][1];
    return box->halfX * fabsf(dotX) + box->halfZ * fabsf(dotZ);
}

static int overlapOnAxis(const BoxOBB *a, const BoxOBB *b, float axisX, float axisZ)
{
    float ra = projectRadius(a, axisX, axisZ);
    float rb = projectRadius(b, axisX, axisZ);
    float dx = b->centerX - a->centerX;
    float dz = b->centerZ - a->centerZ;
    float axisLen = sqrtf(axisX * axisX + axisZ * axisZ);
    if (axisLen < 1e-6f)
        return 1;
    float distance = fabsf((dx * axisX + dz * axisZ) / axisLen);
    return distance <= (ra + rb);
}

static int obbOverlapXZ(const BoxOBB *a, const BoxOBB *b)
{
    if (!overlapOnAxis(a, b, a->axis[0][0], a->axis[0][1]))
        return 0;
    if (!overlapOnAxis(a, b, a->axis[1][0], a->axis[1][1]))
        return 0;
    if (!overlapOnAxis(a, b, b->axis[0][0], b->axis[0][1]))
        return 0;
    if (!overlapOnAxis(a, b, b->axis[1][0], b->axis[1][1]))
        return 0;
    return 1;
}

// Object collision + rotation helpers
// Check AABB overlap
bool collidesWithAnyObject(SceneObject *movingObj, float newX, float newZ,
                           bool adjustPlayerHeight, bool allowStageSnap)
{
    float bestPlatformTop = 0.0f; // highest platform below player
    float playerHeight = 0.0f;
    if (adjustPlayerHeight)
    {
        playerHeight = movingObj->subBox[0][3] - movingObj->subBox[0][2];
    }

    // Loop through all objects
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *other = &objects[i];

        // Skip self and non-solid objects
        if (other == movingObj || !other->solid)
            continue;

        // For every sub-box of the moving object
        for (int movingSubBoxIndex = 0; movingSubBoxIndex < movingObj->subBoxCount; movingSubBoxIndex++)
        {
            float movingMinX, movingMaxX, movingMinY, movingMaxY, movingMinZ, movingMaxZ;
            BoxOBB boxA;

            computeRotatedBounds(movingObj, movingSubBoxIndex, newX, newZ,
                                 &movingMinX, &movingMaxX,
                                 &movingMinY, &movingMaxY,
                                 &movingMinZ, &movingMaxZ);
            buildBoxOBB(movingObj, movingSubBoxIndex, newX, newZ, &boxA);

            // Compare with each sub-box of the other object
            for (int otherSubBoxIndex = 0; otherSubBoxIndex < other->subBoxCount; otherSubBoxIndex++)
            {
                float otherMinX, otherMaxX, otherMinY, otherMaxY, otherMinZ, otherMaxZ;
                BoxOBB boxB;
                computeRotatedBounds(other, otherSubBoxIndex, other->x, other->z,
                                     &otherMinX, &otherMaxX,
                                     &otherMinY, &otherMaxY,
                                     &otherMinZ, &otherMaxZ);
                buildBoxOBB(other, otherSubBoxIndex, other->x, other->z, &boxB);

                // AABB horizontal overlap check
                bool overlapXZ =
                    (movingMaxX > otherMinX && movingMinX < otherMaxX &&
                     movingMaxZ > otherMinZ && movingMinZ < otherMaxZ);

                if (!overlapXZ)
                    continue;

                if (!obbOverlapXZ(&boxA, &boxB))
                    continue;

                // stage climbing logic
                bool isPlatform = allowStageSnap &&
                                   strstr(other->name, "Stage") != NULL;

                if (isPlatform)
                {
                    float stageTop = boxB.maxY; // actual stage height

                    // Check if player is above platform
                    if (boxA.minY >= stageTop - 2.0f)
                    {
                        if (stageTop > bestPlatformTop)
                            bestPlatformTop = stageTop;

                        // Allow movement onto top
                        continue;
                    }

                    // Otherwise hit the side of the stage
                }

                // normal vertical overlap check
                bool overlapY =
                    (boxA.maxY > boxB.minY && boxA.minY < boxB.maxY);

                if (overlapY)
                {
                    // Side collision, then block movement
                    return true;
                }
            }
        }
    }

    if (adjustPlayerHeight && movingObj == &playerObj)
    {
        if (bestPlatformTop > 0.0f)
        {
            float desiredFeetY = bestPlatformTop;
            float desiredFpvY = desiredFeetY + playerHeight;

            // Smooth snap to platform
            fpvY = desiredFpvY;

            movingObj->y = fpvY - playerHeight;
        }
        else
        {
            // Reset to floor level
            movingObj->y = 0.0f;
            fpvY = playerHeight; // camera height above floor
        }
    }

    return false; // no collision
}

bool checkRotationCollision(SceneObject *obj, float newRotation)
{
    // Save current rotation
    float oldRotation = obj->rotation;

    // Apply temporary rotation
    obj->rotation = newRotation;
    
    // Check if this new state hits anything
    // We pass obj->x and obj->z because the position hasn't changed, only angle
    bool collides = collidesWithAnyObject(obj, obj->x, obj->z, false, false);

    // Restore original rotation immediately
    obj->rotation = oldRotation;

    return collides;
}

// Rotate object around its Y-axis
void rotateObject(SceneObject *obj, float angle)
{
    if (!obj)
        return;

    float newRotation = obj->rotation + angle;

    // Keep in 0–360 range
    if (newRotation >= 360.0f)
        newRotation -= 360.0f;
    if (newRotation < 0.0f)
        newRotation += 360.0f;

    // apply if the new angle doesn't result in a collision
    if (!checkRotationCollision(obj, newRotation))
    {
        obj->rotation = newRotation;
    }
}

// Player bounding box
void initPlayerCollision(void)
{
    playerObj.subBoxCount = 1;

    playerObj.subBox[0][0] = -0.6f;
    playerObj.subBox[0][1] = 0.6f;

    playerObj.subBox[0][2] = 0.0f;
    playerObj.subBox[0][3] = fpvY;

    playerObj.subBox[0][4] = -0.6f;
    playerObj.subBox[0][5] = 0.6f;

    playerObj.solid = 1;
}
