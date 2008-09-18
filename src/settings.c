#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <expat.h>
#include "settings.h"
#include "madpdf.h"

typedef struct
{
    int in_panning;
    int in_panning_horizontal;
    int in_panning_vertical;
    int in_padding;
    int in_padding_left;
    int in_padding_right;
    int in_zoom;
    int in_zoom_increment;
} parsenav;


progsettings *settings=NULL;
parsenav *parsinginfo=NULL;


void save_settings(const char *filename)
{
    char temp[300];
    if(!file_exists(filename))
        creat(filename,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    int filehandle=open(filename,O_WRONLY|O_TRUNC);
    sprintf(temp,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<settings>\n<panning>\n<horizontal>");
    write(filehandle,temp,strlen(temp));
    sprintf(temp,"%d",settings->hpan);
    write(filehandle,temp,strlen(temp));
    sprintf(temp,"</horizontal>\n<vertical>");
    write(filehandle,temp,strlen(temp));
    sprintf(temp,"%d",settings->vpan);
    write(filehandle,temp,strlen(temp));
    sprintf(temp,"</vertical>\n</panning>\n<zoom>\n<increment>");
    write(filehandle,temp,strlen(temp));
    sprintf(temp,"%d",settings->zoominc);
    write(filehandle,temp,strlen(temp));
    sprintf(temp,"</increment>\n</zoom>\n<trimpadding>\n<left>");
    write(filehandle,temp,strlen(temp));
    sprintf(temp,"%d",settings->ltrimpad);
    write(filehandle,temp,strlen(temp));
    sprintf(temp,"</left>\n<right>");
    write(filehandle,temp,strlen(temp));
    sprintf(temp,"%d",settings->rtrimpad);
    write(filehandle,temp,strlen(temp));
    sprintf(temp,"</right>\n</trimpadding>\n</settings>\n");
    write(filehandle,temp,strlen(temp));
    close(filehandle);
}

progsettings *get_settings()
{
    return settings;
}

void handlestart(void *userData,const XML_Char *name,const XML_Char **atts)
{
    
    
    if(strcmp(name,"panning")==0)
        parsinginfo->in_panning=1;
    else if(strcmp(name,"horizontal")==0 && parsinginfo->in_panning)
        parsinginfo->in_panning_horizontal=1;
    else if(strcmp(name,"vertical")==0 && parsinginfo->in_panning)
        parsinginfo->in_panning_vertical=1;
    else if(strcmp(name,"zoom")==0)
        parsinginfo->in_zoom=1;
    else if(strcmp(name,"increment")==0 && parsinginfo->in_zoom)
        parsinginfo->in_zoom_increment=1;
    else if(strcmp(name,"trimpadding")==0)
        parsinginfo->in_padding=1;
    else if(strcmp(name,"left")==0 && parsinginfo->in_padding)
        parsinginfo->in_padding_left=1;
    else if(strcmp(name,"right")==0 && parsinginfo->in_padding)
        parsinginfo->in_padding_right=1;
}

void handleend(void *userData,const XML_Char *name)
{
    if(strcmp(name,"panning")==0)
        parsinginfo->in_panning=0;
    else if(strcmp(name,"horizontal")==0 && parsinginfo->in_panning)
        parsinginfo->in_panning_horizontal=0;
    else if(strcmp(name,"vertical")==0 && parsinginfo->in_panning)
        parsinginfo->in_panning_vertical=0;
    else if(strcmp(name,"zoom")==0)
        parsinginfo->in_zoom=0;
    else if(strcmp(name,"increment")==0 && parsinginfo->in_zoom)
        parsinginfo->in_zoom_increment=0;
    else if(strcmp(name,"trimpadding")==0)
        parsinginfo->in_padding=0;
    else if(strcmp(name,"left")==0 && parsinginfo->in_padding)
        parsinginfo->in_padding_left=0;
    else if(strcmp(name,"right")==0 && parsinginfo->in_padding)
        parsinginfo->in_padding_right=0;
}


void handlechar(void *userData,const XML_Char *s,int len)
{
    
    char *temp2=(char *)calloc(len+1,sizeof(char));
    
    strncpy(temp2,s,len);
    temp2[len]='\0';
    
    if(len>0)
    {
        if(parsinginfo->in_panning_horizontal)
            settings->hpan=(int)strtol(temp2,NULL,10);
        else if(parsinginfo->in_panning_vertical)
            settings->vpan=(int)strtol(temp2,NULL,10);
        else if(parsinginfo->in_padding_left)
            settings->ltrimpad=(int)strtol(temp2,NULL,10);
        else if(parsinginfo->in_padding_right)
            settings->rtrimpad=(int)strtol(temp2,NULL,10);
    }
    
}




void init_settings()
{
    free_settings();
    settings=(progsettings*)malloc(sizeof(progsettings));
    settings->hpan=100;
    settings->vpan=100;
    settings->ltrimpad=0;
    settings->rtrimpad=0;
    settings->zoominc=15;
}
void init_parsinginfo()
{
    parsinginfo=(parsenav*)malloc(sizeof(parsenav));
    parsinginfo->in_panning=0;
    parsinginfo->in_panning_horizontal=0;
    parsinginfo->in_panning_vertical=0;
    parsinginfo->in_padding=0;
    parsinginfo->in_padding_left=0;
    parsinginfo->in_padding_right=0;
    parsinginfo->in_zoom=0;
    parsinginfo->in_zoom_increment=0;
}

void load_settings(const char *filename)
{
    struct stat stat_p;
    int filehandle;
    char *buffer;
    long nread;
    XML_Parser myparse;
    
    init_settings();
    init_parsinginfo();
    
    
    myparse=XML_ParserCreate("UTF-8");
    XML_UseParserAsHandlerArg(myparse);
    XML_SetElementHandler(myparse,handlestart,handleend);
    XML_SetCharacterDataHandler(myparse,handlechar);
    
    
    
    
    filehandle=open(filename,O_RDONLY);
    if(!filehandle)
        return;
    stat(filename,&stat_p);
    buffer=(char *)malloc(stat_p.st_size);
    nread=read(filehandle,(void *)buffer,stat_p.st_size);
    XML_Parse(myparse,buffer,nread,1);
    
    
    free(buffer);
    free_parsinginfo();
    close(filehandle);
    
    XML_ParserFree(myparse);

}


void free_settings()
{
    if(settings!=NULL)
    {
        free(settings);
        settings=NULL;
    }

}
void free_parsinginfo()
{
    if(parsinginfo!=NULL)
    {
        free(parsinginfo);
        parsinginfo=NULL;
    }

}

