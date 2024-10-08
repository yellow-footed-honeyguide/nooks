#include <errno.h>
#include <stdio.h>   // printf (function), fprintf (function), perror (function), FILE (type)
#include <stdlib.h>  // getenv (function), exit (function)
#include <string.h>  // strcmp (function), strcspn (function)
#include <sys/wait.h>
#include <unistd.h>  // getcwd (function), chdir (function), execvp (function)

#include "actions.h"  // MAX_PATH (macro), CONFIG_FILE (macro), function prototypes

#define FACAD_CMD "facad"  // Define command name for custom 'facad' utility
#define LS_CMD "ls"        // Define command name for 'ls' utility
#define LS_ARGS "-A", "-F", "--group-directories-first", "--sort=extension", "--color=always"

int quiet_mode = 0;  // Global flag for quiet mode, initialized to false

/**
 * @brief Enables quiet mode for the application
 *
 * This function sets the global quiet_mode flag to true, which
 * suppresses certain output messages throughout the program.
 */
void quit_work() {
    quiet_mode = 1;  // Set quiet mode flag to true
}

/**
 * @brief Saves the current directory with a specified name
 *
 * @param spot The name to associate with the current directory
 *
 * This function saves the current working directory to a configuration
 * file, associating it with the provided 'spot' name. If the spot
 * already exists, it updates the associated directory. The function
 * handles file operations and error checking.
 */
void save_current_directory(const char *spot) {
    char *home_dir = getenv("HOME");  // Get user's home directory
    char config_path[MAX_PATH];       // Buffer for config file path
    snprintf(config_path, sizeof(config_path), "%s%s", home_dir,
             CONFIG_FILE);  // Construct config file path

    char temp_path[MAX_PATH];  // Buffer for temporary file path
    // Construct temporary file path
    if (strncat(strcpy(temp_path, config_path), ".tmp",
                sizeof(temp_path) - strlen(config_path) - 1) != temp_path) {
        fprintf(stderr, "Path truncated\n");
    }

    FILE *file = fopen(config_path, "r");     // Open config file for reading
    FILE *temp_file = fopen(temp_path, "w");  // Open temporary file for writing

    if (file == NULL) {                // Check if config file opened successfully
        perror("Error opening file");  // Print error message
        exit(1);                       // Exit program with error status
    }

    if (temp_file == NULL) {                      // Check if temporary file opened successfully
        perror("Error creating temporary file");  // Print error message
        fclose(file);                             // Close config file
        exit(1);                                  // Exit program with error status
    }

    char current_dir[MAX_PATH];                              // Buffer for current directory path
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {  // Get current working directory
        perror("Error getting current directory");           // Print error message
        fclose(file);                                        // Close config file
        fclose(temp_file);                                   // Close temporary file
        exit(1);                                             // Exit program with error status
    }

    int spot_found = 0;                                // Flag to check if spot already exists
    char line[MAX_PATH];                               // Buffer for reading lines from config file
    while (fgets(line, sizeof(line), file) != NULL) {  // Read config file line by line
        char saved_spot[MAX_PATH];                     // Buffer for spot name in current line
        sscanf(line, "%s", saved_spot);                // Extract spot name from line

        if (strcmp(saved_spot, spot) == 0) {                   // If spot already exists
            fprintf(temp_file, "%s %s\n", spot, current_dir);  // Update spot with new directory
            spot_found = 1;  // Set flag to indicate spot was found
        } else {
            fprintf(temp_file, "%s", line);  // Write unchanged line to temporary file
        }
    }

    if (!spot_found) {                                     // If spot doesn't exist
        fprintf(temp_file, "%s %s\n", spot, current_dir);  // Add new spot to temporary file
    }

    fclose(file);       // Close config file
    fclose(temp_file);  // Close temporary file

    if (remove(config_path) != 0) {              // Remove original config file
        perror("Error deleting original file");  // Print error message
        exit(1);                                 // Exit program with error status
    }

    if (rename(temp_path, config_path) != 0) {    // Rename temporary file to config file
        perror("Error renaming temporary file");  // Print error message
        exit(1);                                  // Exit program with error status
    }
    if (!quiet_mode) {                          // If not in quiet mode
        printf("Nook marked as '%s'\n", spot);  // Print success message
    }
}

/**
 * @brief Changes the current directory to a previously saved spot
 *
 * @param spot The name of the saved directory to navigate to
 *
 * This function reads the configuration file to find the directory
 * associated with the given spot name. If found, it changes the
 * current directory to that location and launches a new shell
 * instance in the new directory.
 */
void go_to_directory(const char *spot) {
    char *home_dir = getenv("HOME");             // Get user's home directory
    if (home_dir == NULL) {                      // Check if home directory was found
        perror("Error getting home directory");  // Print error message
        exit(1);                                 // Exit program with error status
    }

    char config_path[MAX_PATH];  // Buffer for config file path
    snprintf(config_path, sizeof(config_path), "%s%s", home_dir,
             CONFIG_FILE);  // Construct config file path

    FILE *file = fopen(config_path, "r");  // Open config file for reading
    if (file == NULL) {                    // Check if config file opened successfully
        perror("Error opening file");      // Print error message
        exit(1);                           // Exit program with error status
    }

    char line[MAX_PATH];                               // Buffer for reading lines from config file
    while (fgets(line, sizeof(line), file) != NULL) {  // Read config file line by line
        line[strcspn(line, "\n")] = '\0';              // Remove trailing newline character

        char saved_spot[MAX_PATH];  // Buffer for spot name in current line
        char saved_dir[MAX_PATH];   // Buffer for directory path in current line
        if (sscanf(line, "%s %s", saved_spot, saved_dir) ==
            2) {                                  // Extract spot and directory from line
            if (strcmp(saved_spot, spot) == 0) {  // If spot matches
                if (!quiet_mode) {                // If not in quiet mode
                    // Commented out print statements
                }

                if (chdir(saved_dir) == -1) {  // Attempt to change to the saved directory
                    fprintf(stderr, "Error changing to directory %s: %s\n", saved_dir,
                            strerror(errno));  // Print detailed error message
                    fclose(file);              // Close the config file
                    exit(1);                   // Return error code instead of exiting
                }

                pid_t pid = fork();     // Create a child process
                if (pid == -1) {        // Check if fork failed
                    perror("fork");     // Print error message for fork failure
                    fclose(file);       // Close the config file
                    exit(1);            // Return error code
                } else if (pid == 0) {  // Child process code
                    // Attempt to execute facad command
                    if (execlp(FACAD_CMD, FACAD_CMD, NULL) == -1) {
                        // If facad not found, try to execute ls command
                        execlp(LS_CMD, LS_CMD, LS_ARGS, NULL);
                    }
                    // If both facad and ls failed, print error and exit
                    perror("exec failed");
                    _exit(EXIT_FAILURE);
                } else {  // Parent process code
                    int status;
                    waitpid(pid, &status, 0);  // Wait for child process to finish
                }

                fclose(file);  // Close config file

                char *shell = getenv("SHELL");  // Get user's preferred shell
                if (shell == NULL) {            // If shell not found
                    shell = "/bin/sh";          // Use default shell
                }

                char *argv[] = {shell, NULL};  // Prepare arguments for execvp
                execvp(shell, argv);           // Execute shell in new directory

                perror("Error executing shell");  // Print error message if execvp fails
                exit(1);                          // Exit program with error status
            }
        }
    }

    fclose(file);  // Close config file

    if (!quiet_mode) {                                   // If not in quiet mode
        fprintf(stderr, "Nook '%s' not found\n", spot);  // Print error message
    }
}

/**
 * @brief Displays all saved spots and their associated directories
 *
 * This function reads the configuration file and prints out all
 * saved spots along with their corresponding directory paths.
 * It uses ANSI escape codes for formatted output.
 */
void show_all_spots() {
    char *home_dir = getenv("HOME");  // Get user's home directory
    char config_path[MAX_PATH];       // Buffer for config file path
    snprintf(config_path, sizeof(config_path), "%s%s", home_dir,
             CONFIG_FILE);  // Construct config file path

    FILE *file = fopen(config_path, "r");  // Open config file for reading
    if (file == NULL) {                    // Check if config file opened successfully
        printf("No nooks saved yet.\n");   // Print message if no nooks saved
        return;                            // Exit function
    }

    printf("Marked paths:\n");                         // Print header for saved nooks
    char line[MAX_PATH];                               // Buffer for reading lines from config file
    while (fgets(line, sizeof(line), file) != NULL) {  // Read config file line by line
        line[strcspn(line, "\n")] = '\0';              // Remove trailing newline character

        char saved_spot[MAX_PATH];  // Buffer for spot name in current line
        char saved_dir[MAX_PATH];   // Buffer for directory path in current line
        if (sscanf(line, "%s %s", saved_spot, saved_dir) ==
            2) {  // Extract spot and directory from line
            // Print spot and directory with formatting
            printf("\033[48;2;230;240;250m\033[38;2;0;70;120m\033[1m%s:\033[0m \033[38;2;80;80;80m%s\033[0m\n", saved_spot, saved_dir);
        }
    }

    fclose(file);  // Close config file
}

/**
 * @brief Displays the help information for the program
 *
 * This function prints out usage instructions, available options,
 * and examples for using the program. It provides a quick reference
 * for users on how to interact with the application.
 */
void print_help() {
    printf("Usage: nooks [OPTION]... [DIRECTORY]\n");  // Print usage information
    printf("Navigate to saved directories or save the current directory.\n");  // Print description
    printf("\n");                                                              // Print blank line
    printf("Options:\n");  // Print options header
    printf("  -s, --save-mark [NAME]   Save the current directory with an mark name\n");  
    printf("  -d, --delete-mark [NAME] Delete saved mark\n");  
    printf("  -q, --quiet              Quiet mode\n");      
    printf("  -a, --all-marks          Show all saved directories\n");
    printf("  -h, --help               Display this help and exit\n");
    printf("\n");                                                    
    printf("Examples:\n");                                          
    printf("  nooks -s work            Save the current directory with the name 'work'\n"); 
    printf("  nooks -d work            Delete mark 'work'\n"); 
    printf("  nooks work               Navigate to the directory saved with the name 'work'\n"); 
    printf("  nooks -a                 Show all saved directories\n");
    printf("\n");                                                      
    printf("Report bugs to https://github.com/yellow-footed-honeyguide/nooks/issues\n");
}

/**
 * @brief Prints the version information of the program
 *
 * Example: nooks -v, nooks --version
 * Value of NOOKS_VERSION macro defined in CMakeLists.txt
 */
void print_version() {
    printf("%s\n", NOOKS_VERSION);  // Print version information
}

/**
 * @brief Remove selected mark.
 * config_name is hardcoded
 * Example: nooks -d work
 *
 */
void remove_mark(const char *mark) {
    const char *home_dir = getenv("HOME");
    char config_name[256];
    snprintf(config_name, sizeof(config_name), "%s/.nooks", home_dir);
    FILE *f = fopen(config_name, "r"), *t = fopen("temp", "w");
    char s[100];
    size_t len = strlen(mark);
    while (fgets(s, 100, f))
        if (strncmp(s, mark, len)) fputs(s, t);
    fclose(f);
    fclose(t);
    remove(config_name);
    rename("temp", config_name);
}
