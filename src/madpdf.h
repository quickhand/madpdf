#ifndef MADPDF_H
#define MADPDF_H
#include <Ewl.h>
#define K_UNKNOWN -1
#define K_ESCAPE 10
#define K_RETURN 11
int translate_key(Ewl_Event_Key_Down* e);
int file_exists(const char *filename);
#endif
