#include "CSCIx229.h"

// Struct for rotating box
typedef struct
{
    float centerX;
    float centerZ;
    float halfX;
    float halfZ;
    float axis[2][2];
    float minY;
    float maxY;
} BoxOBB;

// Calculates an axis-aligned box that surrounds the rotated object
// Quick check if objects are even close to each other
static void computeRotatedBounds(const SceneObject *sceneObject, int boxIndex,
                                 float worldX, float worldZ,
                                 float *minX, float *maxX,
                                 float *minY, float *maxY,
                                 float *minZ, float *maxZ)
{
    // Get the original size of the box before rotation
    float localMinX = sceneObject->subBox[boxIndex][0];
    float localMaxX = sceneObject->subBox[boxIndex][1];
    float localMinY = sceneObject->subBox[boxIndex][2];
    float localMaxY = sceneObject->subBox[boxIndex][3];
    float localMinZ = sceneObject->subBox[boxIndex][4];
    float localMaxZ = sceneObject->subBox[boxIndex][5];

    // Calculate sine and cosine of the rotation angle
    float yawDegrees = -sceneObject->rotation;
    float cosYaw = Cos(yawDegrees);
    float sinYaw = Sin(yawDegrees);

    // Initialize min/max to extreme values
    *minX = *minZ = +1e9f;
    *maxX = *maxZ = -1e9f;

    // Add the object's position to the box height
    *minY = localMinY + sceneObject->y;
    *maxY = localMaxY + sceneObject->y;

    // Loop through the Left and Right sides (X corners)
    for (int cornerXIndex = 0; cornerXIndex < 2; cornerXIndex++)
    {
        // Pick either the min or max X
        float localCornerX = (cornerXIndex == 0) ? localMinX : localMaxX;
        
        // Loop through the Front and Back sides (Z corners)
        for (int cornerZIndex = 0; cornerZIndex < 2; cornerZIndex++)
        {
            // Pick either the min or max Z
            float localCornerZ = (cornerZIndex == 0) ? localMinZ : localMaxZ;

            // Standard math formula to rotate a 2D point (X, Z)
            float rotatedX = cosYaw * localCornerX - sinYaw * localCornerZ + worldX;
            float rotatedZ = sinYaw * localCornerX + cosYaw * localCornerZ + worldZ;

            // Check if this new corner is the smallest or biggest X we have found
            if (rotatedX < *minX)
                *minX = rotatedX;
            if (rotatedX > *maxX)
                *maxX = rotatedX;
            
            // Check if this new corner is the smallest or biggest Z we have found
            if (rotatedZ < *minZ)
                *minZ = rotatedZ;
            if (rotatedZ > *maxZ)
                *maxZ = rotatedZ;
        }
    }
}

// Build the BoxOBB structure for the rotated box
// Detailed check if the quick check says they might be hitting
static void buildBoxOBB(const SceneObject *sceneObject, int boxIndex,
                        float worldX, float worldZ, BoxOBB *box)
{
    // Get the raw dimensions of the box
    float localMinX = sceneObject->subBox[boxIndex][0];
    float localMaxX = sceneObject->subBox[boxIndex][1];
    float localMinY = sceneObject->subBox[boxIndex][2];
    float localMaxY = sceneObject->subBox[boxIndex][3];
    float localMinZ = sceneObject->subBox[boxIndex][4];
    float localMaxZ = sceneObject->subBox[boxIndex][5];

    // Find center point and radius of the box
    float localCenterX = 0.5f * (localMinX + localMaxX);
    float localCenterZ = 0.5f * (localMinZ + localMaxZ);
    float halfExtentX = 0.5f * (localMaxX - localMinX);
    float halfExtentZ = 0.5f * (localMaxZ - localMinZ);

    // Calculate sine and cosine of the rotation angle
    float yawDegrees = -sceneObject->rotation;
    float cosYaw = Cos(yawDegrees);
    float sinYaw = Sin(yawDegrees);

    // Rotate the center point and add the world position
    box->centerX = cosYaw * localCenterX - sinYaw * localCenterZ + worldX;
    box->centerZ = sinYaw * localCenterX + cosYaw * localCenterZ + worldZ;
    
    // Store the half-sizes
    box->halfX = halfExtentX;
    box->halfZ = halfExtentZ;
    
    // Store the directions the box is facing
    box->axis[0][0] = cosYaw;
    box->axis[0][1] = sinYaw;
    box->axis[1][0] = -sinYaw;
    box->axis[1][1] = cosYaw;
    
    // Set the height in the world
    box->minY = localMinY + sceneObject->y;
    box->maxY = localMaxY + sceneObject->y;
}

// Calculates how long the box looks when viewed from a specific angle
static float projectRadius(const BoxOBB *box, float axisX, float axisZ)
{
    // Calculate the length of the measuring line
    float axisLength = sqrtf(axisX * axisX + axisZ * axisZ);
    
    // If the line is practically zero length, the shadow is size 0
    if (axisLength < 1e-6f)
        return 0.0f;

    // Normalize the measuring line
    float normalizedAxisX = axisX / axisLength;
    float normalizedAxisZ = axisZ / axisLength;

    // Calculate how much the box's local axes align with the measuring line
    float localXAxisAlignment = normalizedAxisX * box->axis[0][0] + normalizedAxisZ * box->axis[0][1];
    float localZAxisAlignment = normalizedAxisX * box->axis[1][0] + normalizedAxisZ * box->axis[1][1];
    
    // Return the projected radius
    return box->halfX * fabsf(localXAxisAlignment) + box->halfZ * fabsf(localZAxisAlignment);
}

// Check if there is overlap between two boxes along a specific axis
static int overlapOnAxis(const BoxOBB *a, const BoxOBB *b, float axisX, float axisZ)
{
    // Get the shadow length of box A and B
    float projectionRadiusA = projectRadius(a, axisX, axisZ);
    float projectionRadiusB = projectRadius(b, axisX, axisZ);
    
    // Find distance between the two centers
    float centerDeltaX = b->centerX - a->centerX;
    float centerDeltaZ = b->centerZ - a->centerZ;
    
    float axisLength = sqrtf(axisX * axisX + axisZ * axisZ);
    
    if (axisLength < 1e-6f)
        return 1;
        
    // Calculate the distance between centers projected onto the line
    float centerProjectionDistance = fabsf((centerDeltaX * axisX + centerDeltaZ * axisZ) / axisLength);
    
    // If the distance between centers is smaller than the two shadow lengths combined, they overlap
    return centerProjectionDistance <= (projectionRadiusA + projectionRadiusB);
}

// Separating Axis Theorem (SAT) to check for overlap between two rotated boxes in XZ plane
static int obbOverlapXZ(const BoxOBB *a, const BoxOBB *b)
{
    // Check using Box A's Forward and Right directions
    if (!overlapOnAxis(a, b, a->axis[0][0], a->axis[0][1]))
        return 0; // Found a gap, no collision
    if (!overlapOnAxis(a, b, a->axis[1][0], a->axis[1][1]))
        return 0; // Found a gap, no collision
        
    // Check using Box B's Forward and Right directions
    if (!overlapOnAxis(a, b, b->axis[0][0], b->axis[0][1]))
        return 0; // Found a gap, no collision
    if (!overlapOnAxis(a, b, b->axis[1][0], b->axis[1][1]))
        return 0; // Found a gap, no collision
        
    return 1; // No gaps found on any side, so they must be hitting
}

// Check if moving an object to a new spot causes a crash
bool collidesWithAnyObject(SceneObject *movingObject, float newX, float newZ,
                           bool adjustPlayerHeight, bool allowStageSnap)
{
    float bestPlatformTop = 0.0f;
    float playerHeight = 0.0f;
    
    // If we need to adjust the player height, find out how tall the player is
    if (adjustPlayerHeight)
    {
        playerHeight = movingObject->subBox[0][3] - movingObject->subBox[0][2];
    }

    // Loop through every single object in the scene
    for (int objectIndex = 0; objectIndex < objectCount; objectIndex++)
    {
        SceneObject *otherObject = &objects[objectIndex];

        // Skip checking against itself or non-solid objects
        if (otherObject == movingObject || !otherObject->solid)
            continue;

        // Check every subbox of the moving object
        for (int movingSubBoxIndex = 0; movingSubBoxIndex < movingObject->subBoxCount; movingSubBoxIndex++)
        {
            // Variables for the object
            float movingAabbMinX, movingAabbMaxX, movingAabbMinY, movingAabbMaxY, movingAabbMinZ, movingAabbMaxZ;
            BoxOBB movingObb;

            // Calculate the bounds for the moving object at the new position
            computeRotatedBounds(movingObject, movingSubBoxIndex, newX, newZ,
                                 &movingAabbMinX, &movingAabbMaxX,
                                 &movingAabbMinY, &movingAabbMaxY,
                                 &movingAabbMinZ, &movingAabbMaxZ);
            buildBoxOBB(movingObject, movingSubBoxIndex, newX, newZ, &movingObb);

            // Check against every subbox of the other object
            for (int otherSubBoxIndex = 0; otherSubBoxIndex < otherObject->subBoxCount; otherSubBoxIndex++)
            {
                // Variables for the other object
                float otherAabbMinX, otherAabbMaxX, otherAabbMinY, otherAabbMaxY, otherAabbMinZ, otherAabbMaxZ;
                BoxOBB otherObb;
                
                // Calculate bounds for the other object at its current position
                computeRotatedBounds(otherObject, otherSubBoxIndex, otherObject->x, otherObject->z,
                                     &otherAabbMinX, &otherAabbMaxX,
                                     &otherAabbMinY, &otherAabbMaxY,
                                     &otherAabbMinZ, &otherAabbMaxZ);
                buildBoxOBB(otherObject, otherSubBoxIndex, otherObject->x, otherObject->z, &otherObb);

                // Quick Check (AABB)
                // If the simple outer boxes don't touch, we can stop right here
                bool aabbOverlapXZ =
                    (movingAabbMaxX > otherAabbMinX && movingAabbMinX < otherAabbMaxX &&
                     movingAabbMaxZ > otherAabbMinZ && movingAabbMinZ < otherAabbMaxZ);

                if (!aabbOverlapXZ)
                    continue; // Too far apart, check next object

                // Detailed Check (OBB)
                // The outer boxes touched, so check the rotated boxes
                if (!obbOverlapXZ(&movingObb, &otherObb))
                    continue;

                // Height Check
                // Check if the object is a Stage that we can walk on
                bool isPlatform = allowStageSnap &&
                                   strstr(otherObject->name, "Stage") != NULL;

                if (isPlatform)
                {
                    float stageTop = otherObb.maxY; 

                    // If the player's feet are above the platform top
                    if (movingObb.minY >= stageTop - 2.0f)
                    {
                        // Save the highest platform we are standing on
                        if (stageTop > bestPlatformTop)
                            bestPlatformTop = stageTop;

                        // It's a floor, not a wall, so don't block movement
                        continue;
                    }
                }

                // Check if they overlap vertically (Height)
                bool yOverlap =
                    (movingObb.maxY > otherObb.minY && movingObb.minY < otherObb.maxY);

                // Overlapping in X, Z, and Y
                if (yOverlap)
                {
                    // Collision detected, no movement allowed
                    return true;
                }
            }
        }
    }

    // Check if we need to snap the player onto a platform
    if (adjustPlayerHeight && movingObject == &playerObj)
    {
        // If we found a platform under our feet
        if (bestPlatformTop > 0.0f)
        {
            float desiredFeetY = bestPlatformTop;
            float desiredFpvY = desiredFeetY + playerHeight;

            // Set camera height
            fpvY = desiredFpvY;

            // Move player to top of platform
            movingObject->y = fpvY - playerHeight;
        }
        else
        {
            // No platform, walking on ground
            movingObject->y = 0.0f;
            fpvY = playerHeight; 
        }
    }

    return false; // Safe to move
}

// Helper funciton to dry run a rotation to see if it hits anything
bool checkRotationCollision(SceneObject *sceneObject, float newRotation)
{
    // Save the current rotation
    float oldRotation = sceneObject->rotation;

    // Apply the new rotation temporarily
    sceneObject->rotation = newRotation;
    
    // Run the collision check with the new angle
    bool collisionDetected = collidesWithAnyObject(sceneObject, sceneObject->x, sceneObject->z, false, false);

    // Put the rotation back the way it was
    sceneObject->rotation = oldRotation;

    return collisionDetected;
}

// Helper function to rotate an object if there are no collisions
void rotateObject(SceneObject *sceneObject, float angle)
{
    if (!sceneObject)
        return;

    // Calculate intended angle
    float newRotation = sceneObject->rotation + angle;

    // Keep the number between 0 and 360 degrees
    if (newRotation >= 360.0f)
        newRotation -= 360.0f;
    if (newRotation < 0.0f)
        newRotation += 360.0f;

    // Update if no collision
    if (!checkRotationCollision(sceneObject, newRotation))
    {
        sceneObject->rotation = newRotation;
    }
}

// Setup the player's physical body box
void initPlayerCollision(void)
{
    playerObj.subBoxCount = 1;

    // Width (X)
    playerObj.subBox[0][0] = -0.6f;
    playerObj.subBox[0][1] = 0.6f;

    // Height (Y)
    playerObj.subBox[0][2] = 0.0f;
    playerObj.subBox[0][3] = fpvY;

    // Depth (Z)
    playerObj.subBox[0][4] = -0.6f;
    playerObj.subBox[0][5] = 0.6f;

    // Turn collision on
    playerObj.solid = 1;
}
