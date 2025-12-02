#include "CSCIx229.h"

// map object name substrings to spawn types
static int get_type_from_name(const char *name)
{
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
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        printf("Error: Could not save to %s\n", filename);
        return;
    }

    // save objects
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *obj = &objects[i];

        // skip non-movable objects
        if (!obj->movable)
            continue;

        fprintf(f, "O,%s,%.4f,%.4f,%.4f,%.4f,%.4f\n",
                obj->name, obj->x, obj->y, obj->z, obj->rotation, obj->scale);
    }

    fclose(f);
    printf("Scene saved to %s\n", filename);
}

// load logic
void load_scene(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        printf("Error: Could not load %s\n", filename);
        return;
    }

    // remove all movable objects
    for (int i = objectCount - 1; i >= 0; i--)
    {
        if (objects[i].movable)
        {
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
    while (fgets(line, sizeof(line), f))
    {
        // check line type
        if (line[0] == 'O') // Object
        {
            char nameBuffer[64];
            float x, y, z, rot, scl;

            // parse object line
            if (sscanf(line, "O,%[^,],%f,%f,%f,%f,%f",
                       nameBuffer, &x, &y, &z, &rot, &scl) == 6)
            {
                int type = get_type_from_name(nameBuffer);
                if (type >= 0)
                {
                    // spawn object
                    SceneObject *newObj = scene_spawn_object((SceneSpawnType)type);
                    if (newObj)
                    {
                        newObj->x = x;
                        newObj->y = y;
                        newObj->z = z;
                        newObj->rotation = rot;
                        newObj->scale = scl;
                    }
                }
            }
        }
    }

    fclose(f);
    printf("Scene loaded from %s\n", filename);
    glutPostRedisplay();
}
