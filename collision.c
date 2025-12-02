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

// Rotate a sub-box around Y and compute its world-space AABB extents
static void computeRotatedBounds(const SceneObject *sceneObject, int boxIndex,
                                 float worldX, float worldZ,
                                 float *minX, float *maxX,
                                 float *minY, float *maxY,
                                 float *minZ, float *maxZ)
{
    float xmin = sceneObject->subBox[boxIndex][0];
    float xmax = sceneObject->subBox[boxIndex][1];
    float ymin = sceneObject->subBox[boxIndex][2];
    float ymax = sceneObject->subBox[boxIndex][3];
    float zmin = sceneObject->subBox[boxIndex][4];
    float zmax = sceneObject->subBox[boxIndex][5];

    float angle = -sceneObject->rotation * (M_PI / 180.0f);
    float cosYaw = cosf(angle);
    float sinYaw = sinf(angle);

    *minX = *minZ = +1e9f;
    *maxX = *maxZ = -1e9f;

    *minY = ymin + sceneObject->y;
    *maxY = ymax + sceneObject->y;

    // Iterate over both min/max X corners
    for (int cornerXIndex = 0; cornerXIndex < 2; cornerXIndex++)
    {
        float x = (cornerXIndex == 0) ? xmin : xmax;
        // For each X corner, iterate across the Z limits
        for (int cornerZIndex = 0; cornerZIndex < 2; cornerZIndex++)
        {
            float z = (cornerZIndex == 0) ? zmin : zmax;

            float xr = cosYaw * x - sinYaw * z + worldX;
            float zr = sinYaw * x + cosYaw * z + worldZ;

            // Track global min/max projections for the rotated bounds
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

// Build an oriented bounding box for a sub-box using object transform
static void buildBoxOBB(const SceneObject *sceneObject, int boxIndex,
                        float worldX, float worldZ, BoxOBB *box)
{
    float xmin = sceneObject->subBox[boxIndex][0];
    float xmax = sceneObject->subBox[boxIndex][1];
    float ymin = sceneObject->subBox[boxIndex][2];
    float ymax = sceneObject->subBox[boxIndex][3];
    float zmin = sceneObject->subBox[boxIndex][4];
    float zmax = sceneObject->subBox[boxIndex][5];

    float localCenterX = 0.5f * (xmin + xmax);
    float localCenterZ = 0.5f * (zmin + zmax);
    float halfX = 0.5f * (xmax - xmin);
    float halfZ = 0.5f * (zmax - zmin);

    float angle = -sceneObject->rotation * (M_PI / 180.0f);
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
    box->minY = ymin + sceneObject->y;
    box->maxY = ymax + sceneObject->y;
}

// Calculate the projection radius of an OBB onto the given axis
static float projectRadius(const BoxOBB *box, float axisX, float axisZ)
{
    float axisLen = sqrtf(axisX * axisX + axisZ * axisZ);
    // Treat near-zero axis lengths as zero projection
    if (axisLen < 1e-6f)
        return 0.0f;

    float nx = axisX / axisLen;
    float nz = axisZ / axisLen;

    float dotX = nx * box->axis[0][0] + nz * box->axis[0][1];
    float dotZ = nx * box->axis[1][0] + nz * box->axis[1][1];
    return box->halfX * fabsf(dotX) + box->halfZ * fabsf(dotZ);
}

// Check whether two OBBs overlap along a particular separating axis
static int overlapOnAxis(const BoxOBB *a, const BoxOBB *b, float axisX, float axisZ)
{
    float ra = projectRadius(a, axisX, axisZ);
    float rb = projectRadius(b, axisX, axisZ);
    float dx = b->centerX - a->centerX;
    float dz = b->centerZ - a->centerZ;
    float axisLen = sqrtf(axisX * axisX + axisZ * axisZ);
    // Zero-length axis can't separate boxes, so treat as overlap
    if (axisLen < 1e-6f)
        return 1;
    float distance = fabsf((dx * axisX + dz * axisZ) / axisLen);
    return distance <= (ra + rb);
}

// Run SAT tests using both box axes to confirm XZ overlap
static int obbOverlapXZ(const BoxOBB *a, const BoxOBB *b)
{
    // Test the two axes that belong to box A
    if (!overlapOnAxis(a, b, a->axis[0][0], a->axis[0][1]))
        return 0;
    if (!overlapOnAxis(a, b, a->axis[1][0], a->axis[1][1]))
        return 0;
    // Test the two axes that belong to box B
    if (!overlapOnAxis(a, b, b->axis[0][0], b->axis[0][1]))
        return 0;
    if (!overlapOnAxis(a, b, b->axis[1][0], b->axis[1][1]))
        return 0;
    return 1;
}

// Determine if moving an object to (newX,newZ) would collide with anything
bool collidesWithAnyObject(SceneObject *movingObject, float newX, float newZ,
                           bool adjustPlayerHeight, bool allowStageSnap)
{
    float bestPlatformTop = 0.0f; // highest platform below player
    float playerHeight = 0.0f;
    // When adjusting player height, precompute capsule height
    if (adjustPlayerHeight)
    {
        playerHeight = movingObject->subBox[0][3] - movingObject->subBox[0][2];
    }

    // Iterate over every object in the scene to check collisions
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *otherObject = &objects[i];

        // Skip self and non-solid objects
        if (otherObject == movingObject || !otherObject->solid)
            continue;

        // Test each sub-box that composes the moving object
        for (int movingSubBoxIndex = 0; movingSubBoxIndex < movingObject->subBoxCount; movingSubBoxIndex++)
        {
            float movingMinX, movingMaxX, movingMinY, movingMaxY, movingMinZ, movingMaxZ;
            BoxOBB movingObb;

            computeRotatedBounds(movingObject, movingSubBoxIndex, newX, newZ,
                                 &movingMinX, &movingMaxX,
                                 &movingMinY, &movingMaxY,
                                 &movingMinZ, &movingMaxZ);
            buildBoxOBB(movingObject, movingSubBoxIndex, newX, newZ, &movingObb);

            // Compare with each sub-box of the other object
            for (int otherSubBoxIndex = 0; otherSubBoxIndex < otherObject->subBoxCount; otherSubBoxIndex++)
            {
                float otherMinX, otherMaxX, otherMinY, otherMaxY, otherMinZ, otherMaxZ;
                BoxOBB otherObb;
                computeRotatedBounds(otherObject, otherSubBoxIndex, otherObject->x, otherObject->z,
                                     &otherMinX, &otherMaxX,
                                     &otherMinY, &otherMaxY,
                                     &otherMinZ, &otherMaxZ);
                buildBoxOBB(otherObject, otherSubBoxIndex, otherObject->x, otherObject->z, &otherObb);

                // AABB horizontal overlap check
                bool overlapXZ =
                    (movingMaxX > otherMinX && movingMinX < otherMaxX &&
                     movingMaxZ > otherMinZ && movingMinZ < otherMaxZ);

                // Skip OBB test if the axis-aligned boxes already separate
                if (!overlapXZ)
                    continue;

                // Perform the more expensive OBB check next
                if (!obbOverlapXZ(&movingObb, &otherObb))
                    continue;

                // Detect stage platform volumes when snapping is allowed
                bool isPlatform = allowStageSnap &&
                                   strstr(otherObject->name, "Stage") != NULL;

                if (isPlatform)
                {
                    float stageTop = otherObb.maxY; // actual stage height

                    // Check if player is above platform
                    if (movingObb.minY >= stageTop - 2.0f)
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
                    (movingObb.maxY > otherObb.minY && movingObb.minY < otherObb.maxY);

                if (overlapY)
                {
                    // Side collision, then block movement
                    return true;
                }
            }
        }
    }

    // Adjust FPV height when the player is moving and snap-to-platform is active
    if (adjustPlayerHeight && movingObject == &playerObj)
    {
        // If we found a platform below the player, place them on it
        if (bestPlatformTop > 0.0f)
        {
            float desiredFeetY = bestPlatformTop;
            float desiredFpvY = desiredFeetY + playerHeight;

            // Smooth snap to platform
            fpvY = desiredFpvY;

            movingObject->y = fpvY - playerHeight;
        }
        else
        {
            // Reset to floor level
            movingObject->y = 0.0f;
            fpvY = playerHeight; // camera height above floor
        }
    }

    return false; // no collision
}

// Helper that tests whether a temporary rotation would hit anything
bool checkRotationCollision(SceneObject *sceneObject, float newRotation)
{
    // Save current rotation
    float oldRotation = sceneObject->rotation;

    // Apply temporary rotation
    sceneObject->rotation = newRotation;
    
    // Check if this new state hits anything
    bool collides = collidesWithAnyObject(sceneObject, sceneObject->x, sceneObject->z, false, false);

    // Restore original rotation immediately
    sceneObject->rotation = oldRotation;

    return collides;
}

// Rotate an object by the given angle when no collision occurs
void rotateObject(SceneObject *sceneObject, float angle)
{
    if (!sceneObject)
        return;

    float newRotation = sceneObject->rotation + angle;

    // Wrap rotation into [0, 360)
    if (newRotation >= 360.0f)
        newRotation -= 360.0f;
    if (newRotation < 0.0f)
        newRotation += 360.0f;

    // apply if the new angle doesn't result in a collision
    if (!checkRotationCollision(sceneObject, newRotation))
    {
        sceneObject->rotation = newRotation;
    }
}

// Initialize the player's collision box dimensions
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
