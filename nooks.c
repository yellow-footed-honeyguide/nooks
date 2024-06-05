#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH 1024
#define CONFIG_FILE "/.nooks"

void save_current_directory(const char *spot) {
    char *home_dir = getenv("HOME");
    char config_path[MAX_PATH];
    snprintf(config_path, sizeof(config_path), "%s%s", home_dir, CONFIG_FILE);

    FILE *file = fopen(config_path, "a");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    char current_dir[MAX_PATH];
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("Error getting current directory");
        fclose(file);
        exit(1);
    }

    fprintf(file, "%s %s\n", spot, current_dir);
    fclose(file);
    printf("Current directory saved as spot '%s'\n", spot);
}

void go_to_directory(const char *spot) {
    char *home_dir = getenv("HOME");
    char config_path[MAX_PATH];
    snprintf(config_path, sizeof(config_path), "%s%s", home_dir, CONFIG_FILE);

    FILE *file = fopen(config_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    char line[MAX_PATH];
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';  // Remove trailing newline character

        char saved_spot[MAX_PATH];
        char saved_dir[MAX_PATH];
        if (sscanf(line, "%s %s", saved_spot, saved_dir) == 2) {
            if (strcmp(saved_spot, spot) == 0) {
                if (chdir(saved_dir) == -1) {
                    perror("Error changing directory");
                    fclose(file);
                    exit(1);
                }
                fclose(file);

                char *shell = getenv("SHELL");
                if (shell == NULL) {
                    shell = "/bin/sh";
                }

                char *argv[] = {shell, NULL};
                execvp(shell, argv);

                perror("Error executing shell");
                exit(1);
            }
        }
    }

    fclose(file);
    printf("Spot '%s' not found\n", spot);
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "-s") == 0) {
        save_current_directory("default");
    } else if (argc == 3 && strcmp(argv[1], "-s") == 0) {
        save_current_directory(argv[2]);
    } else if (argc == 2) {
        go_to_directory(argv[1]);
    } else {
        go_to_directory("default");
    }

    return 0;
}
