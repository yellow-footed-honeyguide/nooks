#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "actions.h"

void save_current_directory(const char *spot) {
    char *home_dir = getenv("HOME");
    char config_path[MAX_PATH];
    snprintf(config_path, sizeof(config_path), "%s%s", home_dir, CONFIG_FILE);

    char temp_path[MAX_PATH];
    snprintf(temp_path, sizeof(temp_path), "%s.tmp", config_path);

    FILE *file = fopen(config_path, "r");
    FILE *temp_file = fopen(temp_path, "w");

    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    if (temp_file == NULL) {
        perror("Error creating temporary file");
        fclose(file);
        exit(1);
    }

    char current_dir[MAX_PATH];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("Error getting current directory");
        fclose(file);
        fclose(temp_file);
        exit(1);
    }

    int spot_found = 0;
    char line[MAX_PATH];
    while (fgets(line, sizeof(line), file) != NULL) {
        char saved_spot[MAX_PATH];
        sscanf(line, "%s", saved_spot);

        if (strcmp(saved_spot, spot) == 0) {
            fprintf(temp_file, "%s %s\n", spot, current_dir);
            spot_found = 1;
        } else {
            fprintf(temp_file, "%s", line);
        }
    }

    if (!spot_found) {
        fprintf(temp_file, "%s %s\n", spot, current_dir);
    }

    fclose(file);
    fclose(temp_file);

    if (remove(config_path) != 0) {
        perror("Error deleting original file");
        exit(1);
    }

    if (rename(temp_path, config_path) != 0) {
        perror("Error renaming temporary file");
        exit(1);
    }

    printf("Current directory saved as spot '%s'\n", spot);
}

/**
 * Changes the current directory to the specified spot.
 *
 * @param spot The name of the spot to go to.
 */
void go_to_directory(const char *spot) {
    // Get the user's home directory
    char *home_dir = getenv("HOME");
    if (home_dir == NULL) {
        perror("Error getting home directory");
        exit(1);
    }

    // Construct the path to the configuration file
    char config_path[MAX_PATH];
    snprintf(config_path, sizeof(config_path), "%s%s", home_dir, CONFIG_FILE);

    // Open the configuration file for reading
    FILE *file = fopen(config_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // Read the configuration file line by line
    char line[MAX_PATH];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove the trailing newline character
        line[strcspn(line, "\n")] = '\0';

        // Parse the spot and directory from the line
        char saved_spot[MAX_PATH];
        char saved_dir[MAX_PATH];
        if (sscanf(line, "%s %s", saved_spot, saved_dir) == 2) {
            // Check if the spot matches the specified spot
            if (strcmp(saved_spot, spot) == 0) {
                // Print the spot to which the relocation will occur
                printf("Relocating to spot '%s': %s\n", spot, saved_dir);

                // Change the current directory to the saved directory
                if (chdir(saved_dir) == -1) {
                    perror("Error changing directory");
                    fclose(file);
                    exit(1);
                }

                // Close the configuration file
                fclose(file);

                // Get the user's preferred shell
                char *shell = getenv("SHELL");
                if (shell == NULL) {
                    // Set the default shell to "/bin/sh" if not found
                    shell = "/bin/sh";
                }

                // Execute the shell in the new directory
                char *argv[] = {shell, NULL};
                execvp(shell, argv);

                // If execvp returns, an error occurred
                perror("Error executing shell");
                exit(1);
            }
        }
    }

    // Close the configuration file
    fclose(file);

    // Print an error message if the spot is not found
    fprintf(stderr, "Spot '%s' not found\n", spot);
}

void show_all_spots() {
    char *home_dir = getenv("HOME");
    char config_path[MAX_PATH];
    snprintf(config_path, sizeof(config_path), "%s%s", home_dir, CONFIG_FILE);

    FILE *file = fopen(config_path, "r");
    if (file == NULL) {
        printf("No spots saved yet.\n");
        return;
    }

    printf("Saved spots:\n");
    char line[MAX_PATH];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';  // Remove trailing newline character

        char saved_spot[MAX_PATH];
        char saved_dir[MAX_PATH];
        if (sscanf(line, "%s %s", saved_spot, saved_dir) == 2) {
            printf("%s: %s\n", saved_spot, saved_dir);
        }
    }

    fclose(file);
}
