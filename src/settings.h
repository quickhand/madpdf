#ifndef MADPDF_SETTINGS_H
#define MADPDF_SETTINGS_H

typedef struct 
{
    int hpan;
    int vpan;
    int ltrimpad;
    int rtrimpad;
    int zoominc;
} progsettings;

void save_settings(const char *filename);
progsettings *get_settings();
void load_settings(const char *filename);
void free_settings();

#endif
