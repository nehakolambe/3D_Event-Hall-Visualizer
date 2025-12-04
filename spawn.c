#include "CSCIx229.h"

// Struct for a spawnable object
typedef struct
{
    const char *baseName;
    void (*drawFunc)(float, float);
    int movable;
    float defaultRotation;
    float defaultScale;
} ObjectTemplate;

// List of all objects the user can create
static const ObjectTemplate spawnTemplates[SPAWN_TYPE_COUNT] = {
    [SPAWN_LAMP] = {"Lamp", drawLamp, 1, 0.0f, 1.0f},
    [SPAWN_EVENT_TABLE] = {"EventTable", drawTable, 1, 0.0f, 1.0f},
    [SPAWN_MEETING_TABLE] = {"MeetingTable", drawMeetingTable, 1, 0.0f, 1.0f},
    [SPAWN_BAR_CHAIR] = {"BarChair", drawBarChairObj, 1, 0.0f, 1.0f},
    [SPAWN_BANQUET_CHAIR] = {"BanquetChair", drawBanquetChair, 1, 0.0f, 1.0f},
    [SPAWN_COCKTAIL_1] = {"Cocktail_1", drawCocktailTable, 1, 0.0f, 1.0f},
    [SPAWN_COCKTAIL_2] = {"Cocktail_2", drawCocktailTable2, 1, 0.0f, 1.0f},
    [SPAWN_COCKTAIL_3] = {"Cocktail_3", drawCocktailTable3, 1, 0.0f, 1.0f}};

// Keeps track of how many of each item we have made
static int spawnCounters[SPAWN_TYPE_COUNT] = {0};

// Resets the counter
void scene_spawn_reset(void)
{
    memset(spawnCounters, 0, sizeof(spawnCounters));
}

// Scans the room to find a place to put the new object where it won't hit anything
static int findFreeGroundSpot(SceneObject *prototype, float *outX, float *outZ)
{
    // Need a valid object with boundingbox
    if (!prototype || prototype->subBoxCount == 0)
        return 0;

    const float margin = 0.5f;

    // If Grid Snap is on, we only check grid points, else we check every 1 unit
    const int snapCandidate = snapToGridEnabled && scene_object_supports_snap(prototype);
    const float step = snapCandidate ? GRID_SNAP_SIZE : 1.0f;

    float minX = ROOM_MIN_X + margin;
    float maxX = ROOM_MAX_X - margin;
    float minZ = ROOM_MIN_Z + margin;
    float maxZ = ROOM_MAX_Z - margin;

    float bestDist = 1e9f; // Used to find the spot closest to center (0,0)
    int found = 0;

    // Scan Z rows
    for (float z = minZ; z <= maxZ; z += step)
    {
        // Scan X columns
        for (float x = minX; x <= maxX; x += step)
        {
            float testX = x;
            float testZ = z;

            // Align to grid if needed
            if (snapCandidate)
                scene_snap_position(&testX, &testZ);

            // Double check bounds after snapping
            if (testX < minX)
                testX = minX;
            if (testX > maxX)
                testX = maxX;
            if (testZ < minZ)
                testZ = minZ;
            if (testZ > maxZ)
                testZ = maxZ;

            // Check if this spot is empty without any collisions
            if (!collidesWithAnyObject(prototype, testX, testZ, false, true))
            {
                // Valid spot, calculate distance to center of room
                float dist = testX * testX + testZ * testZ;

                // If this spot is closer to the center than our previous best, take it
                if (!found || dist < bestDist)
                {
                    bestDist = dist;
                    *outX = testX;
                    *outZ = testZ;
                    found = 1;
                }
            }
        }
    }

    return found; // Return 1 if we found a spot, 0 if room is full
}

// Helper funciton to check if a text string starts with a specific word
static int nameHasPrefix(const char *name, const char *prefix)
{
    // If either is missing, we can't compare them
    if (!name || !prefix)
        return 0;

    // Measure how long the prefix word is
    size_t len = strlen(prefix);

    // Compare just the beginning part of the name to the prefix
    return strncmp(name, prefix, len) == 0;
}

// Defining the bounding boxes
void configureObjectBounds(SceneObject *sceneObject)
{
    if (!sceneObject)
        return;

    // Curved Projector Screen
    if (strcmp(sceneObject->name, "CurvedScreen") == 0)
    {
        // Big box that encompasses the curve
        const float width = 35.0f;
        const float height = 10.0f;
        const float yBase = 3.5f;
        const float radiusH = 25.0f;
        const float zOffset = 0.5f;

        // How deep the curve is
        float halfWidth = width * 0.5f;
        float maxHeight = yBase + height;
        float minDepth = zOffset;
        float halfAngleDeg = (width * 0.5f / radiusH) * (180.0f / PI);
        float maxDepth = radiusH - radiusH * Cos(halfAngleDeg) + zOffset;

        sceneObject->subBoxCount = 1;
        // X bounds
        sceneObject->subBox[0][0] = -halfWidth;
        sceneObject->subBox[0][1] = halfWidth;
        // Y bounds (Height)
        sceneObject->subBox[0][2] = 0.0f;
        sceneObject->subBox[0][3] = maxHeight;
        // Z bounds (Depth)
        sceneObject->subBox[0][4] = minDepth;
        sceneObject->subBox[0][5] = maxDepth;
        return;
    }

    // Standard Event Table
    if (nameHasPrefix(sceneObject->name, "EventTable"))
    {
        // Dimensions
        const float topHalfX = 2.0f;
        const float topHalfZ = 1.0f;
        const float topBottomY = 2.0f;
        const float topTopY = 2.15f;
        const float legOffsetX = 1.7f;
        const float legOffsetZ = 0.7f;
        const float legHalf = 0.125f;

        // 1 Tabletop + 4 Legs = 5 boxes
        sceneObject->subBoxCount = 5;

        // Box 0: Tabletop
        sceneObject->subBox[0][0] = -topHalfX;
        sceneObject->subBox[0][1] = topHalfX;
        sceneObject->subBox[0][2] = topBottomY;
        sceneObject->subBox[0][3] = topTopY;
        sceneObject->subBox[0][4] = -topHalfZ;
        sceneObject->subBox[0][5] = topHalfZ;

        // Boxes 1-4: Legs
        int box = 1;
        // Loop through Left/Right and Front/Back combinations to place legs
        for (int legSignX = -1; legSignX <= 1; legSignX += 2)
        {
            for (int legSignZ = -1; legSignZ <= 1; legSignZ += 2)
            {
                float legCenterX = legSignX * legOffsetX;
                float legCenterZ = legSignZ * legOffsetZ;

                // Create a thin vertical box for the leg
                sceneObject->subBox[box][0] = legCenterX - legHalf;
                sceneObject->subBox[box][1] = legCenterX + legHalf;
                sceneObject->subBox[box][2] = 0.0f;
                sceneObject->subBox[box][3] = topBottomY;
                sceneObject->subBox[box][4] = legCenterZ - legHalf;
                sceneObject->subBox[box][5] = legCenterZ + legHalf;
                box++;
            }
        }
        return;
    }

    // Cocktail Table Type 1
    if (nameHasPrefix(sceneObject->name, "Cocktail_1"))
    {
        // Dimensions
        const float topRadius = 1.5f;
        const float topBottomY = 4.0f;
        const float topTopY = topBottomY + 0.1f;
        const float legRadius = 0.15f;
        const float legHeight = 4.0f;
        const float baseRadius = 0.6f;
        const float baseTopY = 0.05f;

        // Top + Center Pole + Base = 3 boxes
        sceneObject->subBoxCount = 3;

        // Box 0: Top
        sceneObject->subBox[0][0] = -topRadius;
        sceneObject->subBox[0][1] = topRadius;
        sceneObject->subBox[0][2] = topBottomY;
        sceneObject->subBox[0][3] = topTopY;
        sceneObject->subBox[0][4] = -topRadius;
        sceneObject->subBox[0][5] = topRadius;

        // Box 1: Center Pole
        sceneObject->subBox[1][0] = -legRadius;
        sceneObject->subBox[1][1] = legRadius;
        sceneObject->subBox[1][2] = 0.0f;
        sceneObject->subBox[1][3] = legHeight;
        sceneObject->subBox[1][4] = -legRadius;
        sceneObject->subBox[1][5] = legRadius;

        // Box 2: Base
        sceneObject->subBox[2][0] = -baseRadius;
        sceneObject->subBox[2][1] = baseRadius;
        sceneObject->subBox[2][2] = 0.0f;
        sceneObject->subBox[2][3] = baseTopY;
        sceneObject->subBox[2][4] = -baseRadius;
        sceneObject->subBox[2][5] = baseRadius;
        return;
    }

    // Cocktail Table Type 2
    if (nameHasPrefix(sceneObject->name, "Cocktail_2"))
    {
        // Dimensions
        const float topRadius = 1.4f;
        const float topBottomY = 4.0f;
        const float topTopY = topBottomY + 0.12f;
        const float legSpread = 0.8f;
        const float legLength = 4.0f;
        const float legTiltDeg = 10.0f;
        const float legHalf = 0.12f;
        const float horizShift = legLength * Sin(legTiltDeg);

        // 1 Tabletop + 3 Angled Legs = 4 boxes
        sceneObject->subBoxCount = 4;

        // Box 0: Tabletop
        sceneObject->subBox[0][0] = -topRadius;
        sceneObject->subBox[0][1] = topRadius;
        sceneObject->subBox[0][2] = topBottomY;
        sceneObject->subBox[0][3] = topTopY;
        sceneObject->subBox[0][4] = -topRadius;
        sceneObject->subBox[0][5] = topRadius;

        // Boxes 1-3: Angled Legs
        for (int i = 0; i < 3; i++)
        {
            float angDeg = (360.0f / 3.0f) * i;
            float dirX = Cos(angDeg);
            float dirZ = Sin(angDeg);

            // Calculate top and bottom positions of the angled leg
            float topX = legSpread * dirX;
            float topZ = legSpread * dirZ;
            float bottomX = topX + horizShift * dirX;
            float bottomZ = topZ + horizShift * dirZ;

            // Make a box that contains the entire slanted leg
            float minX = fminf(topX, bottomX) - legHalf;
            float maxX = fmaxf(topX, bottomX) + legHalf;
            float minZ = fminf(topZ, bottomZ) - legHalf;
            float maxZ = fmaxf(topZ, bottomZ) + legHalf;

            sceneObject->subBox[i + 1][0] = minX;
            sceneObject->subBox[i + 1][1] = maxX;
            sceneObject->subBox[i + 1][2] = 0.0f;
            sceneObject->subBox[i + 1][3] = topBottomY;
            sceneObject->subBox[i + 1][4] = minZ;
            sceneObject->subBox[i + 1][5] = maxZ;
        }
        return;
    }

    // Cocktail Table Type 3
    if (nameHasPrefix(sceneObject->name, "Cocktail_3"))
    {
        // Dimensions
        const float bottomRadius = 0.85f;
        const float bottomHeight = 3.5f;
        const float topRadius = 1.35f;
        const float topHeight = 0.4f;
        const float topDiskThickness = 0.05f;

        // Bottom + Connector + Top = 3 boxes
        sceneObject->subBoxCount = 3;

        // Box 0: Bottom
        sceneObject->subBox[0][0] = -bottomRadius;
        sceneObject->subBox[0][1] = bottomRadius;
        sceneObject->subBox[0][2] = 0.0f;
        sceneObject->subBox[0][3] = bottomHeight;
        sceneObject->subBox[0][4] = -bottomRadius;
        sceneObject->subBox[0][5] = bottomRadius;

        // Box 1: Connector
        sceneObject->subBox[1][0] = -topRadius;
        sceneObject->subBox[1][1] = topRadius;
        sceneObject->subBox[1][2] = bottomHeight;
        sceneObject->subBox[1][3] = bottomHeight + topHeight;
        sceneObject->subBox[1][4] = -topRadius;
        sceneObject->subBox[1][5] = topRadius;

        // Box 2: Top
        sceneObject->subBox[2][0] = -topRadius;
        sceneObject->subBox[2][1] = topRadius;
        sceneObject->subBox[2][2] = bottomHeight + topHeight;
        sceneObject->subBox[2][3] = bottomHeight + topHeight + topDiskThickness;
        sceneObject->subBox[2][4] = -topRadius;
        sceneObject->subBox[2][5] = topRadius;
        return;
    }

    // Chairs (Banquet, Event, Meet)
    if (nameHasPrefix(sceneObject->name, "EventChair") ||
        nameHasPrefix(sceneObject->name, "MeetChair") ||
        nameHasPrefix(sceneObject->name, "BanquetChair"))
    {
        // Dimensions
        const float seatHalfW = 0.5f;
        const float seatHalfD = 0.5f;
        const float seatThickness = 0.25f;
        const float seatCenterY = 1.0f;
        const float seatHalfT = seatThickness * 0.5f;
        const float seatBottomY = seatCenterY - seatHalfT;
        const float seatTopY = seatCenterY + seatHalfT;
        const float legInset = 0.1f;
        const float legHalfWidth = 0.07f * 0.5f;
        const float legReachX = seatHalfW - legInset + legHalfWidth;
        const float legReachZ = seatHalfD - legInset + legHalfWidth;
        const float backHeight = 1.3f;
        const float backCurve = 0.25f;
        const float backTopY = seatBottomY + backHeight;
        const float backMinZ = -seatHalfD - backCurve - seatHalfT;
        const float backMaxZ = -seatHalfD + seatHalfT;

        // Seat Cushion + Backrest + Legs = 3 boxes
        sceneObject->subBoxCount = 3;

        // Box 0: Seat Cushion
        sceneObject->subBox[0][0] = -seatHalfW;
        sceneObject->subBox[0][1] = seatHalfW;
        sceneObject->subBox[0][2] = seatBottomY;
        sceneObject->subBox[0][3] = seatTopY;
        sceneObject->subBox[0][4] = -seatHalfD;
        sceneObject->subBox[0][5] = seatHalfD;

        // Box 1: Backrest
        sceneObject->subBox[1][0] = -seatHalfW;
        sceneObject->subBox[1][1] = seatHalfW;
        sceneObject->subBox[1][2] = seatBottomY;
        sceneObject->subBox[1][3] = backTopY;
        sceneObject->subBox[1][4] = backMinZ;
        sceneObject->subBox[1][5] = backMaxZ;

        // Box 2: Legs
        sceneObject->subBox[2][0] = -legReachX;
        sceneObject->subBox[2][1] = legReachX;
        sceneObject->subBox[2][2] = 0.0f;
        sceneObject->subBox[2][3] = seatBottomY;
        sceneObject->subBox[2][4] = -legReachZ;
        sceneObject->subBox[2][5] = legReachZ;
        return;
    }

    // Bar Chair
    if (nameHasPrefix(sceneObject->name, "BarChair"))
    {
        // Dimensions
        const float legHalfX = 0.65f;
        const float legHalfZ = 0.55f;
        const float legTop = 2.2f;
        const float seatHalf = 0.65f;
        const float seatBottomY = 2.30f;
        const float seatTopY = 2.95f;
        const float backHalfX = 0.70f;
        const float backBottomY = 2.40f;
        const float backTopY = 4.35f;
        const float backMinZ = -0.65f;
        const float backMaxZ = -0.05f;

        // Legs + Seat + Backrest = 3 boxes
        sceneObject->subBoxCount = 3;

        // Box 0: Legs
        sceneObject->subBox[0][0] = -legHalfX;
        sceneObject->subBox[0][1] = legHalfX;
        sceneObject->subBox[0][2] = 0.0f;
        sceneObject->subBox[0][3] = legTop;
        sceneObject->subBox[0][4] = -legHalfZ;
        sceneObject->subBox[0][5] = legHalfZ;

        // Box 1: Seat
        sceneObject->subBox[1][0] = -seatHalf;
        sceneObject->subBox[1][1] = seatHalf;
        sceneObject->subBox[1][2] = seatBottomY;
        sceneObject->subBox[1][3] = seatTopY;
        sceneObject->subBox[1][4] = -seatHalf;
        sceneObject->subBox[1][5] = seatHalf;

        // Box 2: Backrest
        sceneObject->subBox[2][0] = -backHalfX;
        sceneObject->subBox[2][1] = backHalfX;
        sceneObject->subBox[2][2] = backBottomY;
        sceneObject->subBox[2][3] = backTopY;
        sceneObject->subBox[2][4] = backMinZ;
        sceneObject->subBox[2][5] = backMaxZ;
        return;
    }

    // Meeting Table
    if (nameHasPrefix(sceneObject->name, "MeetingTable"))
    {
        // Dimensions
        const float topHalfX = 3.0f;
        const float topHalfZ = 1.35f;
        const float topBottomY = 1.55f;
        const float topTopY = 1.7f;
        const float legOffsetX = 2.2f;
        const float legOffsetZ = 0.585f;
        const float legHalf = 0.125f;
        const float legTop = 1.6f;

        // 1 Tabletop + 4 Legs = 5 boxes
        sceneObject->subBoxCount = 5;

        // Box 0: Tabletop
        sceneObject->subBox[0][0] = -topHalfX;
        sceneObject->subBox[0][1] = topHalfX;
        sceneObject->subBox[0][2] = topBottomY;
        sceneObject->subBox[0][3] = topTopY;
        sceneObject->subBox[0][4] = -topHalfZ;
        sceneObject->subBox[0][5] = topHalfZ;

        // Boxes 1-4: Legs
        int box = 1;
        for (int legSignX = -1; legSignX <= 1; legSignX += 2)
        {
            for (int legSignZ = -1; legSignZ <= 1; legSignZ += 2)
            {
                float legCenterX = legSignX * legOffsetX;
                float legCenterZ = legSignZ * legOffsetZ;

                sceneObject->subBox[box][0] = legCenterX - legHalf;
                sceneObject->subBox[box][1] = legCenterX + legHalf;
                sceneObject->subBox[box][2] = 0.0f;
                sceneObject->subBox[box][3] = legTop;
                sceneObject->subBox[box][4] = legCenterZ - legHalf;
                sceneObject->subBox[box][5] = legCenterZ + legHalf;
                box++;
            }
        }
        return;
    }

    // Lamp
    if (nameHasPrefix(sceneObject->name, "Lamp"))
    {
        // Dimensions
        const float shadeHalf = 0.75f;
        const float shadeBottomY = 4.9f;
        const float shadeTopY = 5.7f;
        const float poleHalf = 0.25f;
        const float poleTopY = 5.15f;
        const float baseHalf = 0.45f;
        const float baseTopY = 0.2f;

        // Lampshade + Pole + Base = 3 boxes
        sceneObject->subBoxCount = 3;

        // Box 0: Lampshade
        sceneObject->subBox[0][0] = -shadeHalf;
        sceneObject->subBox[0][1] = shadeHalf;
        sceneObject->subBox[0][2] = shadeBottomY;
        sceneObject->subBox[0][3] = shadeTopY;
        sceneObject->subBox[0][4] = -shadeHalf;
        sceneObject->subBox[0][5] = shadeHalf;

        // Box 1: Pole
        sceneObject->subBox[1][0] = -poleHalf;
        sceneObject->subBox[1][1] = poleHalf;
        sceneObject->subBox[1][2] = 0.0f;
        sceneObject->subBox[1][3] = poleTopY;
        sceneObject->subBox[1][4] = -poleHalf;
        sceneObject->subBox[1][5] = poleHalf;

        // Box 2: Base
        sceneObject->subBox[2][0] = -baseHalf;
        sceneObject->subBox[2][1] = baseHalf;
        sceneObject->subBox[2][2] = 0.0f;
        sceneObject->subBox[2][3] = baseTopY;
        sceneObject->subBox[2][4] = -baseHalf;
        sceneObject->subBox[2][5] = baseHalf;
        return;
    }

    // Skip static walls and stage
    if (strcmp(sceneObject->name, "Wall_Back") == 0 ||
        strcmp(sceneObject->name, "Wall_Front") == 0 ||
        strcmp(sceneObject->name, "Wall_Left") == 0 ||
        strcmp(sceneObject->name, "Wall_Right") == 0 ||
        strcmp(sceneObject->name, "Stage") == 0)
    {
        return;
    }

    // Fallback: Default box for anything else
    sceneObject->subBoxCount = 1;
    sceneObject->subBox[0][0] = -0.8f;
    sceneObject->subBox[0][1] = 0.8f;
    sceneObject->subBox[0][2] = 0.0f;
    sceneObject->subBox[0][3] = 3.0f;
    sceneObject->subBox[0][4] = -0.8f;
    sceneObject->subBox[0][5] = 0.8f;
}

// Creates a new object from the list
SceneObject *scene_spawn_object(SceneSpawnType type)
{
    // Validate the requested type
    if (type < 0 || type >= SPAWN_TYPE_COUNT)
        return NULL;

    // Get the template for this object type
    const ObjectTemplate *tmpl = &spawnTemplates[type];

    // Create a temporary object to test collisions
    SceneObject prototype;
    memset(&prototype, 0, sizeof(SceneObject));
    strncpy(prototype.name, tmpl->baseName, sizeof(prototype.name) - 1);
    prototype.name[sizeof(prototype.name) - 1] = '\0';

    // Set default properties
    prototype.x = 0.0f;
    prototype.y = 0.0f;
    prototype.z = 0.0f;
    prototype.drawFunc = tmpl->drawFunc;
    prototype.movable = tmpl->movable;
    prototype.scale = tmpl->defaultScale;
    prototype.rotation = tmpl->defaultRotation;
    prototype.solid = 1;

    // Generate the bounding boxes
    configureObjectBounds(&prototype);

    float spawnX = 0.0f;
    float spawnZ = 0.0f;

    // Find a free spot on the floor
    if (!findFreeGroundSpot(&prototype, &spawnX, &spawnZ))
    {
        printf("No available space for %s.\n", tmpl->baseName);
        return NULL;
    }

    // Create a unique name
    char uniqueName[32];
    int count = ++spawnCounters[type];
    snprintf(uniqueName, sizeof(uniqueName), "%s_New%d", tmpl->baseName, count);

    // Add the object to the scene
    SceneObject *spawnedObject = addObject(uniqueName, spawnX, spawnZ, tmpl->drawFunc, tmpl->movable);

    if (!spawnedObject)
    {
        printf("Maximum object count reached.\n");
        return NULL;
    }

    // Apply the settings to the real object
    spawnedObject->rotation = tmpl->defaultRotation;
    spawnedObject->scale = tmpl->defaultScale;

    // Add bounding boxes to the real object
    configureObjectBounds(spawnedObject);

    // If it spawned on a stage, lift it up
    scene_apply_stage_height(spawnedObject);

    // Automatically select the new object for the user
    selectedObject = spawnedObject;
    dragging = 0;

    printf("Spawned %s at (%.1f, %.1f).\n", spawnedObject->name, spawnX, spawnZ);
    return spawnedObject;
}
