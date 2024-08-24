#include <stdio.h>   // printf (function), fprintf (function), perror (function), FILE (type)
#include <stdlib.h>  // getenv (function), exit (function)
#include <string.h>  // strcmp (function), strcspn (function)
#include <unistd.h>  // getcwd (function), chdir (function), execvp (function)

#include "actions.h"  // MAX_PATH (macro), CONFIG_FILE (macro), function prototypes

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
    snprintf(temp_path, sizeof(temp_path), "%s.tmp", config_path);  // Construct temporary file path

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

                if (chdir(saved_dir) == -1) {            // Change to saved directory
                    perror("Error changing directory");  // Print error message
                    fclose(file);                        // Close config file
                    exit(1);                             // Exit program with error status
                } else {
                    // Check if facad is available and execute it, otherwise use ls
                    if (system("which facad > /dev/null 2>&1") == 0) {
                        system("facad");  // Execute custom 'facad' command if available
                    } else {
                        // Execute ls command with specific options if facad is not available
                        system(
                            "/usr/bin/ls -A -F --group-directories-first --sort=extension "
                            "--color=always");
                    }
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

    printf("Saved nooks:\n\n");                        // Print header for saved nooks
    char line[MAX_PATH];                               // Buffer for reading lines from config file
    while (fgets(line, sizeof(line), file) != NULL) {  // Read config file line by line
        line[strcspn(line, "\n")] = '\0';              // Remove trailing newline character

        char saved_spot[MAX_PATH];  // Buffer for spot name in current line
        char saved_dir[MAX_PATH];   // Buffer for directory path in current line
        if (sscanf(line, "%s %s", saved_spot, saved_dir) ==
            2) {  // Extract spot and directory from line
            printf("\033[48;5;24m\033[37m%s:\033[0m \033[3m%s\033[0m\n", saved_spot,
                   saved_dir);  // Print spot and directory with formatting
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
    printf("  -s, --save-mark [NAME] Save the current directory with an mark name\n");  // Print
                                                                                        // save-mark
                                                                                        // option
    printf("  -q, --quiet            Quiet mode\n");                  // Print quiet mode option
    printf("  -a, --all              Show all saved directories\n");  // Print show all option
    printf("  -h, --help             Display this help and exit\n");  // Print help option
    printf("\n");                                                     // Print blank line
    printf("Examples:\n");                                            // Print examples header
    printf(
        "  nooks -s work              Save the current directory with the name 'work'\n");  // Print
                                                                                            // save
                                                                                            // example
    printf(
        "  nooks work                 Navigate to the directory saved with the name 'work'\n");  // Print navigate example
    printf("  nooks -a                   Show all saved directories\n");  // Print show all example
    printf("\n");                                                         // Print blank line
    printf(
        "Report bugs to https://github.com/yellow-footed-honeyguide/nooks/issues\n");  // Print
                                                                                       // bug
                                                                                       // report
                                                                                       // information
}

/**
 * @brief Prints the version information of the program
 *
 * This function outputs the version number of the application,
 * which is defined in the NOOKS_VERSION macro.
 */
void print_version() {
    printf("%s\n", NOOKS_VERSION);  // Print version information
}
