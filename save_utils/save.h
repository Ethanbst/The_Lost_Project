#ifndef save_h
#define save_h

void save_progress(char* world_name, int battle_done[], int dialogs_done[]);
int exist_save();
char* getlastworldname();
int* get_battles();
int* get_dialogs();

#endif