/***************************************************************************
 *   Copyright (C) 2008 by Marc Lajoie   *
 *   marc@gatherer   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <Ewl.h>
#include <Epdf.h>
#include <ewl_pdf.h>
#include "madpdf.h"
#include "settings.h"
#include "opt_dlg.h"

Ewl_Widget *win = NULL;
Ewl_Widget *pdfwidget = NULL;
Ewl_Widget *scrollpane = NULL;
Ewl_Widget *trimpane = NULL;
Ewl_Widget *menu=NULL;
char statlabel1str[100];
Ewl_Widget *statlabel1=NULL;
char statlabel2str[100];
Ewl_Widget *statlabel2=NULL;
Ewl_Widget *goto_entry;
double curscale=1.0;
//double moveinc=0.1;
//for later, margin cuttoffs
int fitmode=0;
double leftmarge=0;
double rightmarge=0;


/* Returns edje theme file name (pointer to static buffer). */
const char* get_theme_file()
{
    static const char system_theme[] = "/usr/share/madpdf/madpdf.edj";
    static const char rel_theme[] = "./themes/edje/madpdf.edj";

    if(0 == access(rel_theme, R_OK))
        return rel_theme;

    if(0 == access(system_theme, R_OK))
        return system_theme;

    fprintf(stderr, "Unable to find any theme. Silly me.\n");
    exit(1);
}

double get_horizontal_pan_inc()
{
    double ws=(double)CURRENT_W(scrollpane);
    double wt=(double)CURRENT_W(trimpane);
    if(wt<=ws)
        return 0.0;
    return ((double)get_settings()->hpan)*ws/(100.0*(wt-ws));
    
}
double get_vertical_pan_inc()
{
    double hs=(double)CURRENT_H(scrollpane);
    double ht=(double)CURRENT_H(trimpane);
    if(ht<=hs)
        return 0.0;
    return ((double)get_settings()->vpan)*hs/(100.0*(ht-hs));
}

int translate_key(Ewl_Event_Key_Down* e)
{
    const char* k = e->base.keyname;

    if (!strcmp(k, "Escape"))
        return K_ESCAPE;
    if (!strcmp(k, "Return"))
        return K_RETURN;
    if (isdigit(k[0]) && !k[1])
        return k[0] - '0';
    return K_UNKNOWN;
}

int file_exists(const char *filename)
{
    struct stat stFileInfo;
    int intStat = stat(filename,&stFileInfo);
    if(intStat == 0) {
        return 1;
    }
    return 0;
}

int get_left_margin()
{
    int imgw,imgh;
    int hor,ver;
    int firstpix;
    int *imgptr=evas_object_image_data_get(EWL_PDF(pdfwidget)->image,0);
    evas_object_image_size_get(EWL_PDF(pdfwidget)->image,&imgw,&imgh);
    firstpix=imgptr[0];
    for(hor=0;hor<imgw;hor++)
    {
        
        //fprintf(stderr,"firstpix:%d",firstpix);
        for(ver=0;ver<imgh;ver++)
        {
            
            if(imgptr[ver*imgw+hor]!=firstpix)
                return hor;
        }
    }
    return 0;
}
int get_right_margin()
{
    int imgw,imgh;
    int hor,ver;
    int firstpix;
    int *imgptr=evas_object_image_data_get(EWL_PDF(pdfwidget)->image,0);
    evas_object_image_size_get(EWL_PDF(pdfwidget)->image,&imgw,&imgh);
    firstpix=imgptr[imgw-1];
    for(hor=imgw-1;hor>=0;hor--)
    {

        //fprintf(stderr,"firstpix:%d",firstpix);
        for(ver=0;ver<imgh;ver++)
        {
            //fprintf(stderr,"pix:%d",imgptr[ver*imgw+hor]);
            if(imgptr[ver*imgw+hor]!=firstpix)
                return imgw-1-hor;
        }
    }
    return 0;
}
int get_top_margin()
{
    int imgw,imgh;
    int hor,ver;
    int firstpix;
    int *imgptr=evas_object_image_data_get(EWL_PDF(pdfwidget)->image,0);
    evas_object_image_size_get(EWL_PDF(pdfwidget)->image,&imgw,&imgh);
    firstpix=imgptr[0];
    for(ver=0;ver<imgh;ver++)
    {
        for(hor=0;hor<imgw;hor++)
        {
        
            if(imgptr[ver*imgw+hor]!=firstpix)
                return ver;
        }
    }
    return 0;
}
int get_bottom_margin()
{
    int imgw,imgh;
    int hor,ver;
    int firstpix;
    int curpix;
    int *imgptr=evas_object_image_data_get(EWL_PDF(pdfwidget)->image,0);
    evas_object_image_size_get(EWL_PDF(pdfwidget)->image,&imgw,&imgh);
    firstpix=imgptr[(imgh-1)*imgw];
    for(ver=imgh-1;ver>=0;ver--)
    {
        
        
        for(hor=0;hor<imgw;hor++)
        {
        
            if(imgptr[ver*imgw+hor]!=firstpix)
                return imgh-1-ver;
        }
    }
    return 0;
}
void calculate_margins()
{
    int docwidth,docheight;
    double hscale,vscale;
    epdf_page_size_get (EWL_PDF(pdfwidget)->pdf_page,&docwidth,&docheight);
    ewl_pdf_scale_get(EWL_PDF(pdfwidget),&hscale,&vscale);
    leftmarge=((double)get_left_margin())/floor(((double)docwidth)*hscale);
    rightmarge=((double)get_right_margin())/floor(((double)docwidth)*hscale);
    fprintf(stderr,"%f %f",leftmarge,rightmarge);
}
void resize_and_rescale(double scale)
{
    int docwidth,docheight;
    double docscale;
    int sp_inner;
    double ltrimpct=0.0,rtrimpct=0.0;
    
    //ewl_object_maximum_h_set(EWL_OBJECT(pdfwidget),99999);
    //ewl_object_minimum_h_set(EWL_OBJECT(pdfwidget),0);
    sp_inner=CURRENT_W(scrollpane)-INSET_HORIZONTAL(scrollpane)-PADDING_HORIZONTAL(scrollpane);
    //if(EWL_SCROLLPANE(scrollpane)->vflag)
    //    sp_inner-=CURRENT_W(EWL_OBJECT(EWL_SCROLLPANE(scrollpane)->vscrollbar));
        
    //ewl_pdf_size_get(EWL_PDF(pdfwidget),&docwidth,&docheight);
    epdf_page_size_get (EWL_PDF(pdfwidget)->pdf_page,&docwidth,&docheight);
    if(fitmode==0)
        docscale=((double)sp_inner)/((double)docwidth)*scale;
    else if(fitmode==1)
    {
        ltrimpct=((double)get_settings()->ltrimpad)/((double)docwidth);
        rtrimpct=((double)get_settings()->rtrimpad)/((double)docwidth);
        docscale=((double)sp_inner)/((1.0-leftmarge+ltrimpct-rightmarge+rtrimpct)*((double)docwidth))*scale;
        
    }
    ewl_pdf_scale_set(EWL_PDF(pdfwidget),docscale,docscale);
    //ewl_object_custom_w_set(EWL_OBJECT(pdfwidget),floor(((double)sp_inner)*scale));
    
    //ewl_object_custom_w_set(EWL_OBJECT(pdfwidget),floor(((double)sp_inner)*scale));
    //ewl_object_custom_h_set(EWL_OBJECT(pdfwidget),floor(((double)docheight)/((double)docwidth)*((double)sp_inner)*scale));
    ewl_object_custom_w_set(EWL_OBJECT(pdfwidget),floor(((double)docwidth)*docscale));
    ewl_object_custom_h_set(EWL_OBJECT(pdfwidget),floor(((double)docheight)*docscale));
    //ewl_object_position_request(EWL_OBJECT(pdfwidget),0,0);
    ewl_widget_configure(pdfwidget);
    
    //ewl_object_custom_w_set(EWL_OBJECT(trimpane),floor(((double)docwidth)*docscale));
    //ewl_object_custom_h_set(EWL_OBJECT(trimpane),floor(((double)docheight)*docscale));
    ewl_object_custom_w_set(EWL_OBJECT(trimpane),floor(((double)sp_inner)*scale));
    ewl_object_custom_h_set(EWL_OBJECT(trimpane),floor(((double)docheight)*docscale));
    ewl_object_position_request(EWL_OBJECT(trimpane),0,0);
    

    //ewl_object_place(EWL_OBJECT(pdfwidget),0,0,floor(((double)docwidth)*docscale),floor(((double)docheight)*docscale));
    ewl_widget_configure(trimpane);
    ewl_widget_configure(scrollpane);
    if(fitmode==0)
        ewl_scrollpane_hscrollbar_value_set(EWL_SCROLLPANE(trimpane),0.0);
    else if(fitmode==1)
        ewl_scrollpane_hscrollbar_value_set(EWL_SCROLLPANE(trimpane),(leftmarge-ltrimpct)/(leftmarge-ltrimpct+rightmarge-rtrimpct));
}
void update_statusbar()
{
    sprintf(statlabel1str,"MadPDF (OK for Menu)");
    ewl_label_text_set(EWL_LABEL(statlabel1),statlabel1str);
    
    int curpage,totalpage;
    curpage=ewl_pdf_page_get(EWL_PDF(pdfwidget))+1;
    totalpage=epdf_document_page_count_get(ewl_pdf_pdf_document_get(EWL_PDF(pdfwidget)));
    
    sprintf(statlabel2str,"pg: %d/%d  zoom: %d%%  ",curpage,totalpage,(int)round(curscale*100.0));
    double hpos,vpos;
    int leftarr=0,rightarr=0,downarr=0,uparr=0;
    if(CURRENT_W(trimpane)>CURRENT_W(scrollpane))
    {
        hpos=ewl_scrollpane_hscrollbar_value_get(EWL_SCROLLPANE(scrollpane));
        if(hpos>0.0)
        {
            leftarr=1;
        }
        if(hpos<1.0)
        {
            rightarr=1;    
        }
        
    }
    if(CURRENT_H(trimpane)>CURRENT_H(scrollpane))
    {
        vpos=ewl_scrollpane_vscrollbar_value_get(EWL_SCROLLPANE(scrollpane));
        if(vpos>0.0)
        {
            uparr=1;
        }
        if(vpos<1.0)
        {
            downarr=1;    
        }
        
    }
    if(leftarr||rightarr||downarr||uparr)
        strcat(statlabel2str,"pan: ");
    else
        strcat(statlabel2str,"pan: none");
    if(leftarr)
        strcat(statlabel2str,"←");
    if(rightarr)
        strcat(statlabel2str,"→");
    if(downarr)
        strcat(statlabel2str,"↓");
    if(uparr)
        strcat(statlabel2str,"↑");
    ewl_label_text_set(EWL_LABEL(statlabel2),statlabel2str);
}

void cb_key_down(Ewl_Widget *w, void *ev, void *data)
{
    Ewl_Widget *curwidget;
    Ewl_Event_Key_Down *e;
    e = (Ewl_Event_Key_Down*)ev;
    int k = translate_key(e);
    
    curwidget=ewl_widget_name_find("pdfwidget");
    switch(k)
    {
    case 0:
        ewl_pdf_page_next(EWL_PDF(curwidget));
        resize_and_rescale(curscale);
        fprintf(stderr,"l:%d; r:%d; t:%d; b:%d\n",get_left_margin(),get_right_margin(),get_top_margin(),get_bottom_margin());
        //update_statusbar();
        break;
    case 9:
        ewl_pdf_page_previous(EWL_PDF(curwidget));
        resize_and_rescale(curscale);
        //update_statusbar();
        break;
    case 8:
        curscale+=((double)get_settings()->zoominc)/100.0;
        resize_and_rescale(curscale);
        //update_statusbar();
        break;
    case 7:
        curscale-=((double)get_settings()->zoominc)/100.0;
        resize_and_rescale(curscale);
        //update_statusbar();
        break;
    /*case 6:
        if(ewl_pdf_orientation_get(EWL_PDF(curwidget))==EPDF_PAGE_ORIENTATION_LANDSCAPE)
            ewl_pdf_orientation_set(EWL_PDF(curwidget),EPDF_PAGE_ORIENTATION_PORTRAIT);
        else
            ewl_pdf_orientation_set(EWL_PDF(curwidget),EPDF_PAGE_ORIENTATION_LANDSCAPE);
        resize_and_rescale(curscale);
        //update_statusbar();
        break;*/
    case 1:
        ewl_scrollpane_hscrollbar_value_set(EWL_SCROLLPANE(scrollpane),fmax(0.0,ewl_scrollpane_hscrollbar_value_get(EWL_SCROLLPANE(scrollpane))-get_horizontal_pan_inc()));
        update_statusbar();
        break;
    case 2:
        ewl_scrollpane_hscrollbar_value_set(EWL_SCROLLPANE(scrollpane),fmin(1.0,ewl_scrollpane_hscrollbar_value_get(EWL_SCROLLPANE(scrollpane))+get_horizontal_pan_inc()));
        update_statusbar();
        break;
    case 3:
        ewl_scrollpane_vscrollbar_value_set(EWL_SCROLLPANE(scrollpane),fmin(1.0,ewl_scrollpane_vscrollbar_value_get(EWL_SCROLLPANE(scrollpane))+get_vertical_pan_inc()));
        update_statusbar();
        break;    
    case 4:
        ewl_scrollpane_vscrollbar_value_set(EWL_SCROLLPANE(scrollpane),fmax(0.0,ewl_scrollpane_vscrollbar_value_get(EWL_SCROLLPANE(scrollpane))-get_vertical_pan_inc()));
        update_statusbar();
        break;
    case 5:
        if(fitmode==0)
            fitmode=1;
        else if(fitmode==1)
            fitmode=0;
        calculate_margins();
        resize_and_rescale(curscale);
        break;
    case K_RETURN:
        ewl_widget_show(menu);
        ewl_widget_focus_send(menu);
        break;
    case K_ESCAPE:
        ewl_main_quit();
        break;
    default:
        return;
    }
    
    
    
}
void cb_menu_key_down(Ewl_Widget *w, void *ev, void *data)
{
    Ewl_Widget *curwidget;
    char temp[50];
    Ewl_Event_Key_Down *e;
    e = (Ewl_Event_Key_Down*)ev;
    int k = translate_key(e);
    
    switch(k)
    {
    case 1:
        sprintf(temp,"menuitem1");
        curwidget=ewl_widget_name_find(temp);
        ewl_menu_cb_expand(curwidget,NULL,NULL);
        ewl_widget_focus_send(EWL_WIDGET(EWL_MENU(curwidget)->popup));
        int curpage=ewl_pdf_page_get(EWL_PDF(pdfwidget))+1;
        //sprintf(temp,"%d",curpage);
        ewl_text_text_set(EWL_TEXT(goto_entry),"");
        ewl_widget_focus_send(goto_entry);
        break;
    case 2:
        ewl_widget_hide(menu);
        opt_dlg_init();    
        ewl_window_transient_for(EWL_WINDOW(opt_dlg_widget_get()),EWL_WINDOW(win));
        ewl_widget_show(opt_dlg_widget_get());
        ewl_widget_focus_send(opt_dlg_widget_get());
        break;
    case K_ESCAPE:
        ewl_widget_hide(menu);
        break;
    default:
        return;
    }
    
}
void cb_goto_key_down(Ewl_Widget *w, void *ev, void *data)
{
    Ewl_Widget *curwidget;
    char temp[50];
    Ewl_Event_Key_Down *e;
    e = (Ewl_Event_Key_Down*)ev;
    int k = translate_key(e);
    int page,totalpages;
    switch(k)
    {
    case K_RETURN:
        ewl_widget_hide(menu);
        totalpages=epdf_document_page_count_get(ewl_pdf_pdf_document_get(EWL_PDF(pdfwidget)));
        page=(int)strtol(ewl_text_text_get(EWL_TEXT(goto_entry)),NULL,10);
        sprintf(temp,"menuitem1");
        curwidget=ewl_widget_name_find(temp);
        ewl_menu_collapse(EWL_MENU(curwidget));
        if(page>0&&page<=totalpages)
            ewl_pdf_page_set(EWL_PDF(pdfwidget),page-1);
        break;
    case K_ESCAPE:
        sprintf(temp,"menuitem1");
        curwidget=ewl_widget_name_find(temp);
        ewl_menu_collapse(EWL_MENU(curwidget));
        ewl_widget_focus_send(menu);
        break;

    default:
        return;
    }
    
}
void cb_scrollpane_revealed(Ewl_Widget *w, void *ev, void *data)
{
    resize_and_rescale(curscale);
    update_statusbar();
}
void destroy_cb ( Ewl_Widget *w, void *event, void *data )
{
    ewl_widget_destroy ( w );
    ewl_main_quit();
    //double cropwidth=ewl_pdf_pdf_page_get(EWL_PDF(pdfwidget))->page->getCropWidth();
    //int docwidth,docheight;
    //epdf_page_size_get (EWL_PDF(pdfwidget)->pdf_page,&docwidth,&docheight);
    //fprintf(stderr,"full:%d crop:%f",docwidth,cropwidth);
}
void cb_pdfwidget_resized ( Ewl_Widget *w, void *event, void *data )
{
    update_statusbar();
    
}

int main ( int argc, char ** argv )
{	

    
    Ewl_Widget *vbox=NULL;
    Ewl_Widget *statbar=NULL;
    char *homedir;
    char *configfile;
    if(argc<2)
        return 1;
    
    if ( !ewl_init ( &argc, argv ) )
    {
        return 1;
    }

    //setlocale(LC_ALL, "");
    //textdomain("elementpdf");
    ewl_theme_theme_set(get_theme_file());
    
    homedir=getenv("HOME");
    configfile=(char *)calloc(strlen(homedir)+21 + 1, sizeof(char));
    strcat(configfile,homedir);
    strcat(configfile,"/.madpdf");
    
    if(!file_exists(configfile))
    {
        mkdir (configfile,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
        
    }
    
    strcat(configfile,"/settings.xml");
    
    
    
    load_settings(configfile);
    
    win = ewl_window_new();
    ewl_window_title_set ( EWL_WINDOW ( win ), "EWL_WINDOW" );
    ewl_window_name_set ( EWL_WINDOW ( win ), "EWL_WINDOW" );
    ewl_window_class_set ( EWL_WINDOW ( win ), "EWLWindow" );
    ewl_object_size_request ( EWL_OBJECT ( win ), 600, 800 );
    ewl_callback_append ( win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb, NULL );
    ewl_callback_append(win, EWL_CALLBACK_KEY_DOWN, cb_key_down, NULL);
    ewl_widget_name_set(win,"mainwindow");
    ewl_widget_show ( win );
 
    vbox=ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(win),vbox);
    ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_FILL);
    ewl_widget_show(vbox);
    
    scrollpane=ewl_scrollpane_new();
    ewl_container_child_append(EWL_CONTAINER(vbox),scrollpane);
    ewl_callback_append(scrollpane,EWL_CALLBACK_REVEAL,cb_scrollpane_revealed,NULL);
    ewl_scrollpane_hscrollbar_flag_set(EWL_SCROLLPANE(scrollpane),EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN);
    ewl_scrollpane_vscrollbar_flag_set(EWL_SCROLLPANE(scrollpane),EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN);
    //ewl_object_fill_policy_set(EWL_OBJECT(scrollpane), EWL_FLAG_FILL_FILL);
    //ewl_theme_data_str_set(EWL_WIDGET(scrollpane),"/scrollpane/group","ewl/blank");
    ewl_widget_show(scrollpane);
    
    trimpane=ewl_scrollpane_new();
    ewl_container_child_append(EWL_CONTAINER(scrollpane),trimpane);
    ewl_object_alignment_set(EWL_OBJECT(trimpane),EWL_FLAG_ALIGN_LEFT|EWL_FLAG_ALIGN_TOP);
    ewl_scrollpane_hscrollbar_flag_set(EWL_SCROLLPANE(trimpane),EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN);
    ewl_scrollpane_vscrollbar_flag_set(EWL_SCROLLPANE(trimpane),EWL_SCROLLPANE_FLAG_ALWAYS_HIDDEN);
    //ewl_theme_data_str_set(EWL_WIDGET(trimpane),"/scrollpane/group","ewl/blank");
    ewl_widget_show(trimpane);
    
    statbar=ewl_hbox_new();
    ewl_container_child_append(EWL_CONTAINER(vbox),statbar);
    ewl_theme_data_str_set(EWL_WIDGET(statbar),"/hbox/group","ewl/menu/oi_menu");
    ewl_object_fill_policy_set(EWL_OBJECT(statbar),EWL_FLAG_FILL_HFILL|EWL_FLAG_FILL_VSHRINKABLE);
    ewl_widget_show(statbar);
    
    statlabel1=ewl_label_new();   
    //ewl_statusbar_left_append(EWL_STATUSBAR(statbar),statlabel1);
    ewl_container_child_append(EWL_CONTAINER(statbar),statlabel1);
    ewl_theme_data_str_set(EWL_WIDGET(statlabel1),"/label/group","ewl/oi_statbar_label_left");
    ewl_theme_data_str_set(EWL_WIDGET(statlabel1),"/label/textpart","ewl/oi_statbar_label_left/text");
    ewl_object_fill_policy_set(EWL_OBJECT(statlabel1),EWL_FLAG_FILL_HSHRINKABLE);
    ewl_widget_show(statlabel1);
    
    statlabel2=ewl_label_new();   
    //ewl_statusbar_right_append(EWL_STATUSBAR(statbar),statlabel2);
    ewl_container_child_append(EWL_CONTAINER(statbar),statlabel2);
    ewl_theme_data_str_set(EWL_WIDGET(statlabel2),"/label/group","ewl/oi_statbar_label_right");
    ewl_theme_data_str_set(EWL_WIDGET(statlabel2),"/label/textpart","ewl/oi_statbar_label_right/text");
    ewl_object_fill_policy_set(EWL_OBJECT(statlabel2),EWL_FLAG_FILL_HFILL);
    ewl_widget_show(statlabel2);
    
    
    pdfwidget = ewl_pdf_new();
    ewl_pdf_file_set (EWL_PDF (pdfwidget), argv[1]);
    ewl_container_child_append(EWL_CONTAINER(trimpane),pdfwidget);
    ewl_object_alignment_set(EWL_OBJECT(pdfwidget),EWL_FLAG_ALIGN_LEFT|EWL_FLAG_ALIGN_TOP);
    ewl_widget_name_set(pdfwidget,"pdfwidget");
    ewl_callback_append (pdfwidget, EWL_CALLBACK_CONFIGURE, cb_pdfwidget_resized, NULL );
    ewl_widget_show (pdfwidget);
    
    //set up menu
    menu=ewl_context_menu_new();
    
    ewl_callback_append(menu, EWL_CALLBACK_KEY_DOWN, cb_menu_key_down, NULL);
    ewl_theme_data_str_set(EWL_WIDGET(menu),"/menu/group","ewl/menu/oi_menu");
    ewl_context_menu_attach(EWL_CONTEXT_MENU(menu), statbar);
    
    Ewl_Widget *temp=ewl_menu_new();
    ewl_container_child_append(EWL_CONTAINER(menu),temp);
    ewl_widget_name_set(temp,"menuitem1");
    ewl_button_label_set(EWL_BUTTON(temp),"1. Go to page...");
    ewl_widget_show(temp);

    goto_entry=ewl_entry_new();
    ewl_container_child_append(EWL_CONTAINER(temp),goto_entry);
    ewl_object_custom_w_set(EWL_OBJECT(goto_entry),50);
    ewl_callback_append(goto_entry, EWL_CALLBACK_KEY_DOWN, cb_goto_key_down, NULL);
    ewl_widget_show(goto_entry);
    
    
    temp=ewl_menu_item_new();
    ewl_widget_name_set(temp,"menuitem2");
    ewl_container_child_append(EWL_CONTAINER(menu),temp);
    ewl_button_label_set(EWL_BUTTON(temp),"2. Preferences...");
    ewl_widget_show(temp);

        
    
    
    ewl_main();
    save_settings(configfile);
    free(configfile);
    free_settings();
    return 0;
}

// vim:set ts=4 sw=4 et:
