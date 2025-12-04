#include "CSCIx229.h"

// Toggle to enable/disable grid snapping
bool snapToGridEnabled = false;

// Helper function to check if a text string starts with a specific word
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

// Helper function to check if a specific object name is on the "Allowed to Snap" list
static int nameSupportsSnap(const char *name)
{
    // If the object has no name, it can't snap
    if (!name)
        return 0;

    // Check against the list of furniture types
    if (nameHasPrefix(name, "EventTable") ||
        nameHasPrefix(name, "MeetingTable") ||
        nameHasPrefix(name, "BanquetChair") ||
        nameHasPrefix(name, "EventChair") ||
        nameHasPrefix(name, "MeetChair") ||
        nameHasPrefix(name, "Cocktail_") ||
        nameHasPrefix(name, "BarChair"))
        return 1; // Yes, this is snapable furniture

    return 0; // No, ignore this object
}

// Helper function to check if an object supports snapping
int scene_object_supports_snap(const SceneObject *sceneObject)
{
    if (!sceneObject)
        return 0;

    return nameSupportsSnap(sceneObject->name);
}

// Forces a position to align with the grid lines
// It modifies the X and Z numbers directly
void scene_snap_position(float *x, float *z)
{
    // Need valid pointers and a grid size bigger than zero
    if (!x || !z || GRID_SNAP_SIZE <= 0.0f)
        return;

    // Round X and Z to nearest grid line
    *x = roundf(*x / GRID_SNAP_SIZE) * GRID_SNAP_SIZE;
    *z = roundf(*z / GRID_SNAP_SIZE) * GRID_SNAP_SIZE;
}

// Loops through every object in the scene and tries to snap them to the grid
void scene_snap_all_objects(void)
{
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *sceneObject = &objects[i];

        // Skip objects that shouldn't snap
        if (!scene_object_supports_snap(sceneObject))
            continue;

        // Calculate the object's snapped position
        float snappedX = sceneObject->x;
        float snappedZ = sceneObject->z;
        scene_snap_position(&snappedX, &snappedZ);

        // New position should be inside the room walls
        if (snappedX < ROOM_MIN_X)
            snappedX = ROOM_MIN_X;
        if (snappedX > ROOM_MAX_X)
            snappedX = ROOM_MAX_X;
        if (snappedZ < ROOM_MIN_Z)
            snappedZ = ROOM_MIN_Z;
        if (snappedZ > ROOM_MAX_Z)
            snappedZ = ROOM_MAX_Z;

        // Only move the object if the new spot is empty
        if (!collidesWithAnyObject(sceneObject, snappedX, snappedZ, false, true))
        {
            // Apply the new coordinates
            sceneObject->x = snappedX;
            sceneObject->z = snappedZ;

            // Adjust height in case it snapped onto a stage
            scene_apply_stage_height(sceneObject);
        }
    }
}
