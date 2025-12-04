#include "CSCIx229.h"

// Helper function to check which spawn type matches a given name
static int get_type_from_name(const char *name)
{
    // Check if the object name contains specific words
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

    return -1; // Could not identify the object type
}

// Helper function to save the current room setup to a text file
void save_scene(const char *filename)
{
    // Create a new file or overwrite existing one
    FILE *sceneFile = fopen(filename, "w");
    if (!sceneFile)
    {
        printf("Error: Could not save to %s\n", filename);
        return;
    }

    // Loop through every object currently in the scene
    for (int i = 0; i < objectCount; i++)
    {
        SceneObject *sceneObject = &objects[i];

        // Save furniture only (movable objects)
        if (!sceneObject->movable)
            continue;

        // Format: O,Name,X,Y,Z,Rotation,Scale
        fprintf(sceneFile, "O,%s,%.4f,%.4f,%.4f,%.4f,%.4f\n",
                sceneObject->name,
                sceneObject->x,
                sceneObject->y,
                sceneObject->z,
                sceneObject->rotation,
                sceneObject->scale);
    }

    // Close the file (gets saved)
    fclose(sceneFile);
    printf("Scene saved to %s\n", filename);
}

// Helper function to load room setup from a text file
void load_scene(const char *filename)
{
    // Open file
    FILE *sceneFile = fopen(filename, "r");
    if (!sceneFile)
    {
        printf("Error: Could not load %s\n", filename);
        return;
    }

    // Delete all current furniture
    for (int i = objectCount - 1; i >= 0; i--)
    {
        if (objects[i].movable)
        {
            // To remove an object, we shift everything after it down by one slot
            for (int j = i; j < objectCount - 1; j++)
            {
                objects[j] = objects[j + 1];
                objects[j].id = j;
            }
            objectCount--; // Reduce the total count of objects
        }
    }
    selectedObject = NULL;

    // Buffer to hold each line of text we read from the file
    char line[256];
    
    // Read the file line by line
    while (fgets(line, sizeof(line), sceneFile))
    {
        // Check if this line describes an Object (starts with 'O')
        if (line[0] == 'O') 
        {
            char nameBuffer[64];
            float savedX, savedY, savedZ, savedRotation, savedScale;

            // Extract the numbers and name
            if (sscanf(line, "O,%[^,],%f,%f,%f,%f,%f",
                       nameBuffer, &savedX, &savedY, &savedZ, &savedRotation, &savedScale) == 6)
            {
                // Figure out which type of furniture this name corresponds to
                int spawnType = get_type_from_name(nameBuffer);
                
                // If it is a valid type, create it
                if (spawnType >= 0)
                {
                    // Create the new object in the scene
                    SceneObject *spawnedObject = scene_spawn_object((SceneSpawnType)spawnType);
                    
                    // Update its position and rotation to match what was in the file
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

    // Done reading, close the file
    fclose(sceneFile);
    printf("Scene loaded from %s\n", filename);
    
    // Redraw the screen with the new objects
    glutPostRedisplay();
}
