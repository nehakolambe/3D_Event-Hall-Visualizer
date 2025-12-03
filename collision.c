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
    // Local-space bounds pulled from the sub-box definition
    float localMinX = sceneObject->subBox[boxIndex][0];
    float localMaxX = sceneObject->subBox[boxIndex][1];
    float localMinY = sceneObject->subBox[boxIndex][2];
    float localMaxY = sceneObject->subBox[boxIndex][3];
    float localMinZ = sceneObject->subBox[boxIndex][4];
    float localMaxZ = sceneObject->subBox[boxIndex][5];

    // Precompute yaw rotation used for both axes
    float yawDegrees = -sceneObject->rotation;
    float cosYaw = Cos(yawDegrees);
    float sinYaw = Sin(yawDegrees);

    *minX = *minZ = +1e9f;
    *maxX = *maxZ = -1e9f;

    *minY = localMinY + sceneObject->y;
    *maxY = localMaxY + sceneObject->y;

    // Iterate over both min/max X corners
    for (int cornerXIndex = 0; cornerXIndex < 2; cornerXIndex++)
    {
        float localCornerX = (cornerXIndex == 0) ? localMinX : localMaxX;
        // For each X corner, iterate across the Z limits
        for (int cornerZIndex = 0; cornerZIndex < 2; cornerZIndex++)
        {
            float localCornerZ = (cornerZIndex == 0) ? localMinZ : localMaxZ;

            float rotatedX = cosYaw * localCornerX - sinYaw * localCornerZ + worldX;
            float rotatedZ = sinYaw * localCornerX + cosYaw * localCornerZ + worldZ;

            // Track global min/max projections for the rotated bounds
            if (rotatedX < *minX)
                *minX = rotatedX;
            if (rotatedX > *maxX)
                *maxX = rotatedX;
            if (rotatedZ < *minZ)
                *minZ = rotatedZ;
            if (rotatedZ > *maxZ)
                *maxZ = rotatedZ;
        }
    }
}

// Build an oriented bounding box for a sub-box using object transform
static void buildBoxOBB(const SceneObject *sceneObject, int boxIndex,
                        float worldX, float worldZ, BoxOBB *box)
{
    // Local-space corners for this sub-box prior to rotation
    float localMinX = sceneObject->subBox[boxIndex][0];
    float localMaxX = sceneObject->subBox[boxIndex][1];
    float localMinY = sceneObject->subBox[boxIndex][2];
    float localMaxY = sceneObject->subBox[boxIndex][3];
    float localMinZ = sceneObject->subBox[boxIndex][4];
    float localMaxZ = sceneObject->subBox[boxIndex][5];

    // Derived center/half extents that define the OBB
    float localCenterX = 0.5f * (localMinX + localMaxX);
    float localCenterZ = 0.5f * (localMinZ + localMaxZ);
    float halfExtentX = 0.5f * (localMaxX - localMinX);
    float halfExtentZ = 0.5f * (localMaxZ - localMinZ);

    // Rotation basis applied to this sub-box
    float yawDegrees = -sceneObject->rotation;
    float cosYaw = Cos(yawDegrees);
    float sinYaw = Sin(yawDegrees);

    box->centerX = cosYaw * localCenterX - sinYaw * localCenterZ + worldX;
    box->centerZ = sinYaw * localCenterX + cosYaw * localCenterZ + worldZ;
    box->halfX = halfExtentX;
    box->halfZ = halfExtentZ;
    box->axis[0][0] = cosYaw;
    box->axis[0][1] = sinYaw;
    box->axis[1][0] = -sinYaw;
    box->axis[1][1] = cosYaw;
    box->minY = localMinY + sceneObject->y;
    box->maxY = localMaxY + sceneObject->y;
}

// Calculate the projection radius of an OBB onto the given axis
static float projectRadius(const BoxOBB *box, float axisX, float axisZ)
{
    float axisLength = sqrtf(axisX * axisX + axisZ * axisZ);
    // Treat near-zero axis lengths as zero projection
    if (axisLength < 1e-6f)
        return 0.0f;

    float normalizedAxisX = axisX / axisLength;
    float normalizedAxisZ = axisZ / axisLength;

    float localXAxisAlignment = normalizedAxisX * box->axis[0][0] + normalizedAxisZ * box->axis[0][1];
    float localZAxisAlignment = normalizedAxisX * box->axis[1][0] + normalizedAxisZ * box->axis[1][1];
    return box->halfX * fabsf(localXAxisAlignment) + box->halfZ * fabsf(localZAxisAlignment);
}

// Check whether two OBBs overlap along a particular separating axis
static int overlapOnAxis(const BoxOBB *a, const BoxOBB *b, float axisX, float axisZ)
{
    float projectionRadiusA = projectRadius(a, axisX, axisZ);
    float projectionRadiusB = projectRadius(b, axisX, axisZ);
    float centerDeltaX = b->centerX - a->centerX;
    float centerDeltaZ = b->centerZ - a->centerZ;
    float axisLength = sqrtf(axisX * axisX + axisZ * axisZ);
    // Zero-length axis can't separate boxes, so treat as overlap
    if (axisLength < 1e-6f)
        return 1;
    float centerProjectionDistance = fabsf((centerDeltaX * axisX + centerDeltaZ * axisZ) / axisLength);
    return centerProjectionDistance <= (projectionRadiusA + projectionRadiusB);
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
    for (int objectIndex = 0; objectIndex < objectCount; objectIndex++)
    {
        SceneObject *otherObject = &objects[objectIndex];

        // Skip self and non-solid objects
        if (otherObject == movingObject || !otherObject->solid)
            continue;

        // Test each sub-box that composes the moving object
        for (int movingSubBoxIndex = 0; movingSubBoxIndex < movingObject->subBoxCount; movingSubBoxIndex++)
        {
            // World-space AABB bounds used for the cheap rejection test
            float movingAabbMinX, movingAabbMaxX, movingAabbMinY, movingAabbMaxY, movingAabbMinZ, movingAabbMaxZ;
            // Fully rotated oriented bounding box for SAT overlap checks
            BoxOBB movingObb;

            computeRotatedBounds(movingObject, movingSubBoxIndex, newX, newZ,
                                 &movingAabbMinX, &movingAabbMaxX,
                                 &movingAabbMinY, &movingAabbMaxY,
                                 &movingAabbMinZ, &movingAabbMaxZ);
            buildBoxOBB(movingObject, movingSubBoxIndex, newX, newZ, &movingObb);

            // Compare with each sub-box of the other object
            for (int otherSubBoxIndex = 0; otherSubBoxIndex < otherObject->subBoxCount; otherSubBoxIndex++)
            {
                // Equivalent bounds for the candidate collider
                float otherAabbMinX, otherAabbMaxX, otherAabbMinY, otherAabbMaxY, otherAabbMinZ, otherAabbMaxZ;
                BoxOBB otherObb;
                computeRotatedBounds(otherObject, otherSubBoxIndex, otherObject->x, otherObject->z,
                                     &otherAabbMinX, &otherAabbMaxX,
                                     &otherAabbMinY, &otherAabbMaxY,
                                     &otherAabbMinZ, &otherAabbMaxZ);
                buildBoxOBB(otherObject, otherSubBoxIndex, otherObject->x, otherObject->z, &otherObb);

                // AABB horizontal overlap check
                bool aabbOverlapXZ =
                    (movingAabbMaxX > otherAabbMinX && movingAabbMinX < otherAabbMaxX &&
                     movingAabbMaxZ > otherAabbMinZ && movingAabbMinZ < otherAabbMaxZ);

                // Skip OBB test if the axis-aligned boxes already separate
                if (!aabbOverlapXZ)
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
                bool yOverlap =
                    (movingObb.maxY > otherObb.minY && movingObb.minY < otherObb.maxY);

                if (yOverlap)
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
    bool collisionDetected = collidesWithAnyObject(sceneObject, sceneObject->x, sceneObject->z, false, false);

    // Restore original rotation immediately
    sceneObject->rotation = oldRotation;

    return collisionDetected;
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
