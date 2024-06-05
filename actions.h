#ifndef ACTIONS_H
#define ACTIONS_H

#define MAX_PATH 1024
#define CONFIG_FILE "/.nooks"

void save_current_directory(const char *spot);
void go_to_directory(const char *spot);
void show_all_spots();

#endif