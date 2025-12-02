#include "CSCIx229.h"

// Toggles that can be switched at runtime
bool snapToGridEnabled = false;

// Helper used to quick prefix compare that tolerates null input
static int nameHasPrefix(const char *name, const char *prefix)
{
    // Defensive: null names/prefixes never match
    if (!name || !prefix)
        return 0;

    size_t len = strlen(prefix);
    return strncmp(name, prefix, len) == 0;
}

// Only certain furniture should snap to the placement grid
static int nameSupportsSnap(const char *name)
{
    // Null names cannot match any whitelist entry
    if (!name)
        return 0;

    // Tables/chairs should align perfectly with the grid
    if (nameHasPrefix(name, "EventTable") ||
        nameHasPrefix(name, "MeetingTable") ||
        nameHasPrefix(name, "BanquetChair") ||
        nameHasPrefix(name, "EventChair") ||
        nameHasPrefix(name, "MeetChair") ||
        nameHasPrefix(name, "Cocktail_") ||
        nameHasPrefix(name, "BarChair"))
        return 1;

    return 0;
}

// Returns true when the object supports snapping to the grid
int scene_object_supports_snap(const SceneObject *sceneObject)
{
    // Null pointers obviously cannot snap
    if (!sceneObject)
        return 0;

    return nameSupportsSnap(sceneObject->name);
}

// Snaps a pair of coordinates to the shared grid size
void scene_snap_position(float *x, float *z)
{
    // Nothing to do without valid pointers or grid size
    if (!x || !z || GRID_SNAP_SIZE <= 0.0f)
        return;

    *x = roundf(*x / GRID_SNAP_SIZE) * GRID_SNAP_SIZE;
    *z = roundf(*z / GRID_SNAP_SIZE) * GRID_SNAP_SIZE;
}

// Snaps every eligible object without breaking collisions
void scene_snap_all_objects(void)
{
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *sceneObject = &objects[i];
        // Skip objects that deliberately opt out of snapping
        if (!scene_object_supports_snap(sceneObject))
            continue;

        float snappedX = sceneObject->x;
        float snappedZ = sceneObject->z;
        scene_snap_position(&snappedX, &snappedZ);

        // Clamp snapped coordinates so they stay inside the room
        if (snappedX < ROOM_MIN_X)
            snappedX = ROOM_MIN_X;
        if (snappedX > ROOM_MAX_X)
            snappedX = ROOM_MAX_X;
        if (snappedZ < ROOM_MIN_Z)
            snappedZ = ROOM_MIN_Z;
        if (snappedZ > ROOM_MAX_Z)
            snappedZ = ROOM_MAX_Z;

        // Only move the object if the snapped spot is collision free
        if (!collidesWithAnyObject(sceneObject, snappedX, snappedZ, false, true))
        {
            sceneObject->x = snappedX;
            sceneObject->z = snappedZ;
            scene_apply_stage_height(sceneObject);
        }
    }
}
