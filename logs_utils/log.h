#ifndef log_h

#define log_h

//#include <windows.h>
//#include <dbghelp.h>

void create_log_file();
void add_log(const char *origine, const char *texte);
void add_log_error(const char *origine, const char *texte);
void add_log_info(const char *origine, const char *texte);
#endif