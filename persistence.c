#include "CSCIx229.h"

// map object name substrings to spawn types
static int get_type_from_name(const char *name)
{
    // Map substrings to spawn menu entries
    if (strstr(name, "Lamp"))
        return SPAWN_LAMP;
    if (strstr(name, "EventTable"))
        return SPAWN_EVENT_TABLE;
    if (strstr(name, "MeetingTable"))
        return SPAWN_MEETING_TABLE;
    if (strstr(name, "BarChair"))
        return SPAWN_BAR_CHAIR;
    if (strstr(name, "BanquetChair") || strstr(name, "EventChair") || strstr(name, "MeetChair"))
        return SPAWN_BANQUET_CHAIR;
    if (strstr(name, "Cocktail_1"))
        return SPAWN_COCKTAIL_1;
    if (strstr(name, "Cocktail_2"))
        return SPAWN_COCKTAIL_2;
    if (strstr(name, "Cocktail_3"))
        return SPAWN_COCKTAIL_3;

    return -1;
}

// save logic
void save_scene(const char *filename)
{
    FILE *sceneFile = fopen(filename, "w");
    if (!sceneFile)
    {
        printf("Error: Could not save to %s\n", filename);
        return;
    }

    // save objects
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *sceneObject = &objects[i];

        // skip non-movable objects
        if (!sceneObject->movable)
            continue;

        fprintf(sceneFile, "O,%s,%.4f,%.4f,%.4f,%.4f,%.4f\n",
                sceneObject->name,
                sceneObject->x,
                sceneObject->y,
                sceneObject->z,
                sceneObject->rotation,
                sceneObject->scale);
    }

    fclose(sceneFile);
    printf("Scene saved to %s\n", filename);
}

// load logic
void load_scene(const char *filename)
{
    FILE *sceneFile = fopen(filename, "r");
    if (!sceneFile)
    {
        printf("Error: Could not load %s\n", filename);
        return;
    }

    // remove all movable objects
    for (int i = objectCount - 1; i >= 0; i--)
    {
        if (objects[i].movable)
        {
            // Shift the remaining objects down to fill the gap
            for (int j = i; j < objectCount - 1; j++)
            {
                objects[j] = objects[j + 1];
                objects[j].id = j;
            }
            objectCount--;
        }
    }
    selectedObject = NULL;

    // read file line by line
    char line[256];
    while (fgets(line, sizeof(line), sceneFile))
    {
        // check line type
        if (line[0] == 'O') // Object
        {
            char nameBuffer[64];
            float savedX, savedY, savedZ, savedRotation, savedScale;

            // parse object line
            if (sscanf(line, "O,%[^,],%f,%f,%f,%f,%f",
                       nameBuffer, &savedX, &savedY, &savedZ, &savedRotation, &savedScale) == 6)
            {
                int spawnType = get_type_from_name(nameBuffer);
                if (spawnType >= 0)
                {
                    // spawn object
                    SceneObject *spawnedObject = scene_spawn_object((SceneSpawnType)spawnType);
                    if (spawnedObject)
                    {
                        spawnedObject->x = savedX;
                        spawnedObject->y = savedY;
                        spawnedObject->z = savedZ;
                        spawnedObject->rotation = savedRotation;
                        spawnedObject->scale = savedScale;
                    }
                }
            }
        }
    }

    fclose(sceneFile);
    printf("Scene loaded from %s\n", filename);
    glutPostRedisplay();
}
