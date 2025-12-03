#include "CSCIx229.h"

// Struct for spawnable object types
typedef struct
{
    const char *baseName;
    void (*drawFunc)(float, float);
    int movable;
    float defaultRotation;
    float defaultScale;
} ObjectTemplate;

// Lookup table for the spawn menu
static const ObjectTemplate spawnTemplates[SPAWN_TYPE_COUNT] = {
    [SPAWN_LAMP] = {"Lamp", drawLamp, 1, 0.0f, 1.0f},
    [SPAWN_EVENT_TABLE] = {"EventTable", drawTable, 1, 0.0f, 1.0f},
    [SPAWN_MEETING_TABLE] = {"MeetingTable", drawMeetingTable, 1, 0.0f, 1.0f},
    [SPAWN_BAR_CHAIR] = {"BarChair", drawBarChairObj, 1, 0.0f, 1.0f},
    [SPAWN_BANQUET_CHAIR] = {"BanquetChair", drawBanquetChair, 1, 0.0f, 1.0f},
    [SPAWN_COCKTAIL_1] = {"Cocktail_1", drawCocktailTable, 1, 0.0f, 1.0f},
    [SPAWN_COCKTAIL_2] = {"Cocktail_2", drawCocktailTable2, 1, 0.0f, 1.0f},
    [SPAWN_COCKTAIL_3] = {"Cocktail_3", drawCocktailTable3, 1, 0.0f, 1.0f}};

// Counter keeps unique names for repeated spawns
static int spawnCounters[SPAWN_TYPE_COUNT] = {0};

void scene_spawn_reset(void)
{
    memset(spawnCounters, 0, sizeof(spawnCounters));
}

static int findFreeGroundSpot(SceneObject *prototype, float *outX, float *outZ);

// Helper used to quick prefix compare that tolerates null input
static int nameHasPrefix(const char *name, const char *prefix)
{
    // Defensive: null names/prefixes never match
    if (!name || !prefix)
        return 0;

    size_t len = strlen(prefix);
    return strncmp(name, prefix, len) == 0;
}

// Build collision boxes for each object type by name
void configureObjectBounds(SceneObject *sceneObject)
{
    // Ignore null pointers entirely
    if (!sceneObject)
        return;

    // Collision box for the curved screen
    if (strcmp(sceneObject->name, "CurvedScreen") == 0)
    {
        const float width = 35.0f;
        const float height = 10.0f;
        const float yBase = 3.5f;
        const float radiusH = 25.0f;
        const float zOffset = 0.5f;

        float halfWidth = width * 0.5f;
        float maxHeight = yBase + height;
        float minDepth = zOffset;
        float halfAngleDeg = (width * 0.5f / radiusH) * (180.0f / PI);
        float maxDepth = radiusH - radiusH * Cos(halfAngleDeg) + zOffset;

        sceneObject->subBoxCount = 1;
        sceneObject->subBox[0][0] = -halfWidth;
        sceneObject->subBox[0][1] = halfWidth;
        sceneObject->subBox[0][2] = 0.0f;
        sceneObject->subBox[0][3] = maxHeight;
        sceneObject->subBox[0][4] = minDepth;
        sceneObject->subBox[0][5] = maxDepth;
        return;
    }

    // Collision boxes for Event tables
    if (nameHasPrefix(sceneObject->name, "EventTable"))
    {
        const float topHalfX = 2.0f;
        const float topHalfZ = 1.0f;
        const float topBottomY = 2.0f;
        const float topTopY = 2.15f;
        const float legOffsetX = 1.7f;
        const float legOffsetZ = 0.7f;
        const float legHalf = 0.125f;

        sceneObject->subBoxCount = 5;

        // Tabletop
        sceneObject->subBox[0][0] = -topHalfX;
        sceneObject->subBox[0][1] = topHalfX;
        sceneObject->subBox[0][2] = topBottomY;
        sceneObject->subBox[0][3] = topTopY;
        sceneObject->subBox[0][4] = -topHalfZ;
        sceneObject->subBox[0][5] = topHalfZ;

        int box = 1;
        // Loop over +/- X offsets and +/- Z offsets for the four legs
        for (int legSignX = -1; legSignX <= 1; legSignX += 2)
        {
            // Pair each X choice with both possible Z offsets
            for (int legSignZ = -1; legSignZ <= 1; legSignZ += 2)
            {
                float legCenterX = legSignX * legOffsetX;
                float legCenterZ = legSignZ * legOffsetZ;

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

    // Collision boxes for first cocktail table variant
    if (nameHasPrefix(sceneObject->name, "Cocktail_1"))
    {
        const float topRadius = 1.5f;
        const float topBottomY = 4.0f;
        const float topTopY = topBottomY + 0.1f;
        const float legRadius = 0.15f;
        const float legHeight = 4.0f;
        const float baseRadius = 0.6f;
        const float baseTopY = 0.05f;

        sceneObject->subBoxCount = 3;

        sceneObject->subBox[0][0] = -topRadius;
        sceneObject->subBox[0][1] = topRadius;
        sceneObject->subBox[0][2] = topBottomY;
        sceneObject->subBox[0][3] = topTopY;
        sceneObject->subBox[0][4] = -topRadius;
        sceneObject->subBox[0][5] = topRadius;

        sceneObject->subBox[1][0] = -legRadius;
        sceneObject->subBox[1][1] = legRadius;
        sceneObject->subBox[1][2] = 0.0f;
        sceneObject->subBox[1][3] = legHeight;
        sceneObject->subBox[1][4] = -legRadius;
        sceneObject->subBox[1][5] = legRadius;

        sceneObject->subBox[2][0] = -baseRadius;
        sceneObject->subBox[2][1] = baseRadius;
        sceneObject->subBox[2][2] = 0.0f;
        sceneObject->subBox[2][3] = baseTopY;
        sceneObject->subBox[2][4] = -baseRadius;
        sceneObject->subBox[2][5] = baseRadius;
        return;
    }

    // Collision boxes for second cocktail table variant
    if (nameHasPrefix(sceneObject->name, "Cocktail_2"))
    {
        const float topRadius = 1.4f;
        const float topBottomY = 4.0f;
        const float topTopY = topBottomY + 0.12f;
        const float legSpread = 0.8f;
        const float legLength = 4.0f;
        const float legTiltDeg = 10.0f;
        const float legHalf = 0.12f;
        const float horizShift = legLength * Sin(legTiltDeg);

        sceneObject->subBoxCount = 4;

        sceneObject->subBox[0][0] = -topRadius;
        sceneObject->subBox[0][1] = topRadius;
        sceneObject->subBox[0][2] = topBottomY;
        sceneObject->subBox[0][3] = topTopY;
        sceneObject->subBox[0][4] = -topRadius;
        sceneObject->subBox[0][5] = topRadius;

        // Three legs placed evenly around the table
        for (int i = 0; i < 3; i++)
        {
            float angDeg = (360.0f / 3.0f) * i;
            float dirX = Cos(angDeg);
            float dirZ = Sin(angDeg);
            float topX = legSpread * dirX;
            float topZ = legSpread * dirZ;
            float bottomX = topX + horizShift * dirX;
            float bottomZ = topZ + horizShift * dirZ;

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

    // Collision boxes for third cocktail table variant
    if (nameHasPrefix(sceneObject->name, "Cocktail_3"))
    {
        const float bottomRadius = 0.85f;
        const float bottomHeight = 3.5f;
        const float topRadius = 1.35f;
        const float topHeight = 0.4f;
        const float topDiskThickness = 0.05f;

        sceneObject->subBoxCount = 3;

        sceneObject->subBox[0][0] = -bottomRadius;
        sceneObject->subBox[0][1] = bottomRadius;
        sceneObject->subBox[0][2] = 0.0f;
        sceneObject->subBox[0][3] = bottomHeight;
        sceneObject->subBox[0][4] = -bottomRadius;
        sceneObject->subBox[0][5] = bottomRadius;

        sceneObject->subBox[1][0] = -topRadius;
        sceneObject->subBox[1][1] = topRadius;
        sceneObject->subBox[1][2] = bottomHeight;
        sceneObject->subBox[1][3] = bottomHeight + topHeight;
        sceneObject->subBox[1][4] = -topRadius;
        sceneObject->subBox[1][5] = topRadius;

        sceneObject->subBox[2][0] = -topRadius;
        sceneObject->subBox[2][1] = topRadius;
        sceneObject->subBox[2][2] = bottomHeight + topHeight;
        sceneObject->subBox[2][3] = bottomHeight + topHeight + topDiskThickness;
        sceneObject->subBox[2][4] = -topRadius;
        sceneObject->subBox[2][5] = topRadius;
        return;
    }

    // Shared collision setup for all chair types
    if (nameHasPrefix(sceneObject->name, "EventChair") ||
        nameHasPrefix(sceneObject->name, "MeetChair") ||
        nameHasPrefix(sceneObject->name, "BanquetChair"))
    {
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

        sceneObject->subBoxCount = 3;

        // seat cushion
        sceneObject->subBox[0][0] = -seatHalfW;
        sceneObject->subBox[0][1] = seatHalfW;
        sceneObject->subBox[0][2] = seatBottomY;
        sceneObject->subBox[0][3] = seatTopY;
        sceneObject->subBox[0][4] = -seatHalfD;
        sceneObject->subBox[0][5] = seatHalfD;

        // backrest
        sceneObject->subBox[1][0] = -seatHalfW;
        sceneObject->subBox[1][1] = seatHalfW;
        sceneObject->subBox[1][2] = seatBottomY;
        sceneObject->subBox[1][3] = backTopY;
        sceneObject->subBox[1][4] = backMinZ;
        sceneObject->subBox[1][5] = backMaxZ;

        // leg cluster
        sceneObject->subBox[2][0] = -legReachX;
        sceneObject->subBox[2][1] = legReachX;
        sceneObject->subBox[2][2] = 0.0f;
        sceneObject->subBox[2][3] = seatBottomY;
        sceneObject->subBox[2][4] = -legReachZ;
        sceneObject->subBox[2][5] = legReachZ;
        return;
    }
    // Collision boxes for bar chairs
    if (nameHasPrefix(sceneObject->name, "BarChair"))
    {
        sceneObject->subBoxCount = 3;

        sceneObject->subBox[0][0] = -0.65f;
        sceneObject->subBox[0][1] = 0.65f;
        sceneObject->subBox[0][2] = 0.0f;
        sceneObject->subBox[0][3] = 2.2f;
        sceneObject->subBox[0][4] = -0.55f;
        sceneObject->subBox[0][5] = 0.55f;

        sceneObject->subBox[1][0] = -0.65f;
        sceneObject->subBox[1][1] = 0.65f;
        sceneObject->subBox[1][2] = 2.30f;
        sceneObject->subBox[1][3] = 2.95f;
        sceneObject->subBox[1][4] = -0.65f;
        sceneObject->subBox[1][5] = 0.65f;

        sceneObject->subBox[2][0] = -0.70f;
        sceneObject->subBox[2][1] = 0.70f;
        sceneObject->subBox[2][2] = 2.40f;
        sceneObject->subBox[2][3] = 4.35f;
        sceneObject->subBox[2][4] = -0.65f;
        sceneObject->subBox[2][5] = -0.05f;
        return;
    }

    // Collision boxes for the large meeting table
    if (nameHasPrefix(sceneObject->name, "MeetingTable"))
    {
        const float topHalfX = 3.0f;
        const float topHalfZ = 1.35f;
        const float topBottomY = 1.55f;
        const float topTopY = 1.7f;
        const float legOffsetX = 2.2f;
        const float legOffsetZ = 0.585f;
        const float legHalf = 0.125f;
        const float legTop = 1.6f;

        sceneObject->subBoxCount = 5;

        sceneObject->subBox[0][0] = -topHalfX;
        sceneObject->subBox[0][1] = topHalfX;
        sceneObject->subBox[0][2] = topBottomY;
        sceneObject->subBox[0][3] = topTopY;
        sceneObject->subBox[0][4] = -topHalfZ;
        sceneObject->subBox[0][5] = topHalfZ;

        int box = 1;
        // Four legs formed by iterating dir combinations
        for (int legSignX = -1; legSignX <= 1; legSignX += 2)
        {
            // Combine with +/- Z to cover each leg position
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

    // Collision boxes for the standing lamp
    if (nameHasPrefix(sceneObject->name, "Lamp"))
    {
        sceneObject->subBoxCount = 3;

        sceneObject->subBox[0][0] = -0.75f;
        sceneObject->subBox[0][1] = 0.75f;
        sceneObject->subBox[0][2] = 4.9f;
        sceneObject->subBox[0][3] = 5.7f;
        sceneObject->subBox[0][4] = -0.75f;
        sceneObject->subBox[0][5] = 0.75f;

        sceneObject->subBox[1][0] = -0.25f;
        sceneObject->subBox[1][1] = 0.25f;
        sceneObject->subBox[1][2] = 0.0f;
        sceneObject->subBox[1][3] = 5.15f;
        sceneObject->subBox[1][4] = -0.25f;
        sceneObject->subBox[1][5] = 0.25f;

        sceneObject->subBox[2][0] = -0.45f;
        sceneObject->subBox[2][1] = 0.45f;
        sceneObject->subBox[2][2] = 0.0f;
        sceneObject->subBox[2][3] = 0.2f;
        sceneObject->subBox[2][4] = -0.45f;
        sceneObject->subBox[2][5] = 0.45f;
        return;
    }

    // Walls and the stage already provided their own boxes
    if (strcmp(sceneObject->name, "Wall_Back") == 0 ||
        strcmp(sceneObject->name, "Wall_Front") == 0 ||
        strcmp(sceneObject->name, "Wall_Left") == 0 ||
        strcmp(sceneObject->name, "Wall_Right") == 0 ||
        strcmp(sceneObject->name, "Stage") == 0)
    {
        return;
    }

    sceneObject->subBoxCount = 1;
    sceneObject->subBox[0][0] = -0.8f;
    sceneObject->subBox[0][1] = 0.8f;
    sceneObject->subBox[0][2] = 0.0f;
    sceneObject->subBox[0][3] = 3.0f;
    sceneObject->subBox[0][4] = -0.8f;
    sceneObject->subBox[0][5] = 0.8f;
}

// Search the floor grid for a position that does not collide with anything
static int findFreeGroundSpot(SceneObject *prototype, float *outX, float *outZ)
{
    // Can't search without a valid bounding box to test against
    if (!prototype || prototype->subBoxCount == 0)
        return 0;

    const float margin = 0.5f;
    const int snapCandidate = snapToGridEnabled && scene_object_supports_snap(prototype);
    const float step = snapCandidate ? GRID_SNAP_SIZE : 1.0f;
    float minX = ROOM_MIN_X + margin;
    float maxX = ROOM_MAX_X - margin;
    float minZ = ROOM_MIN_Z + margin;
    float maxZ = ROOM_MAX_Z - margin;
    float bestDist = 1e9f;
    int found = 0;

    // Sweep through the allowed Z rows
    for (float z = minZ; z <= maxZ; z += step)
    {
        // Sweep through each X column on that row
        for (float x = minX; x <= maxX; x += step)
        {
            float testX = x;
            float testZ = z;

            // Snap to the placement grid if this item supports it
            if (snapCandidate)
                scene_snap_position(&testX, &testZ);

            // Clamp test position to stay inside the walkable area
            if (testX < minX)
                testX = minX;
            if (testX > maxX)
                testX = maxX;
            if (testZ < minZ)
                testZ = minZ;
            if (testZ > maxZ)
                testZ = maxZ;

            // Remember this location if it is collision-free
            if (!collidesWithAnyObject(prototype, testX, testZ, false, true))
            {
                float dist = testX * testX + testZ * testZ;
                // Track whichever option is closest to origin
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

    return found;
}

// Creates an object from the spawn menu and places it in the room
SceneObject *scene_spawn_object(SceneSpawnType type)
{
    // Reject invalid spawn enums
    if (type < 0 || type >= SPAWN_TYPE_COUNT)
        return NULL;

    const ObjectTemplate *tmpl = &spawnTemplates[type];

    SceneObject prototype;
    memset(&prototype, 0, sizeof(SceneObject));
    strncpy(prototype.name, tmpl->baseName, sizeof(prototype.name) - 1);
    prototype.name[sizeof(prototype.name) - 1] = '\0';
    prototype.x = 0.0f;
    prototype.y = 0.0f;
    prototype.z = 0.0f;
    prototype.drawFunc = tmpl->drawFunc;
    prototype.movable = tmpl->movable;
    prototype.scale = tmpl->defaultScale;
    prototype.rotation = tmpl->defaultRotation;
    prototype.solid = 1;

    configureObjectBounds(&prototype);

    float spawnX = 0.0f;
    float spawnZ = 0.0f;
    // Abort when the room has no available floor space
    if (!findFreeGroundSpot(&prototype, &spawnX, &spawnZ))
    {
        printf("No available space for %s.\n", tmpl->baseName);
        return NULL;
    }

    char uniqueName[32];
    int count = ++spawnCounters[type];
    snprintf(uniqueName, sizeof(uniqueName), "%s_New%d", tmpl->baseName, count);

    SceneObject *spawnedObject = addObject(uniqueName, spawnX, spawnZ, tmpl->drawFunc, tmpl->movable);
    // Bail if the object list is already full
    if (!spawnedObject)
    {
        printf("Maximum object count reached.\n");
        return NULL;
    }

    spawnedObject->rotation = tmpl->defaultRotation;
    spawnedObject->scale = tmpl->defaultScale;
    configureObjectBounds(spawnedObject);
    scene_apply_stage_height(spawnedObject);

    selectedObject = spawnedObject;
    dragging = 0; // reset drag state so user can move it freshly

    printf("Spawned %s at (%.1f, %.1f).\n", spawnedObject->name, spawnX, spawnZ);
    return spawnedObject;
}
