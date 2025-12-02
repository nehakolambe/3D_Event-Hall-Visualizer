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

        // skip non-movable objects except whiteboard
        if (!obj->movable && strcmp(obj->name, "Whiteboard") != 0)
            continue;

        fprintf(f, "O,%s,%.4f,%.4f,%.4f,%.4f,%.4f\n",
                obj->name, obj->x, obj->y, obj->z, obj->rotation, obj->scale);
    }

    // save whiteboard strokes
    int wCount = whiteboard_get_stroke_count();
    for (int i = 0; i < wCount; i++)
    {
        Stroke s = whiteboard_get_stroke(i);
        fprintf(f, "W,%.5f,%.5f,%.5f,%.5f,%d\n",
                s.u1, s.v1, s.u2, s.v2, s.erase);
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
    whiteboard_clear();
    whiteboard_background_invalidate();

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
        else if (line[0] == 'W') // Whiteboard Stroke
        {
            float u1, v1, u2, v2;
            int erase;

            if (sscanf(line, "W,%f,%f,%f,%f,%d",
                       &u1, &v1, &u2, &v2, &erase) == 5)
            {
                whiteboard_add_stroke(u1, v1, u2, v2, erase);
            }
        }
    }

    fclose(f);
    printf("Scene loaded from %s\n", filename);
    glutPostRedisplay();
}
