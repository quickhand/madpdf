#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <Ewl.h>
#include "opt_dlg.h"
#include "settings.h"
#include "madpdf.h"

static Ewl_Widget *dialogwidget=NULL;
int selectedflag=0;

static long minl(long a, long b)
{
   if(a < b) return a;
   return b;
}

static long get_widget_val(const char *name)
{
    Ewl_Widget *widget = ewl_widget_name_find(name);
    return strtol(ewl_text_text_get(EWL_TEXT(widget)), NULL, 10);
}

static void commit_settings()
{
    get_settings()->hpan = (int) minl(get_widget_val("dlg_p1_hb1_entry1"), 100);
    get_settings()->vpan = (int) minl(get_widget_val("dlg_p1_hb1_entry2"), 100);
    get_settings()->zoominc = (int) get_widget_val("dlg_p1_hb2_entry1");
    get_settings()->ltrimpad = (int) get_widget_val("dlg_p1_hb3_entry1");
    get_settings()->rtrimpad = (int) get_widget_val("dlg_p1_hb3_entry2");
}

Ewl_Widget *opt_dlg_widget_get()
{
    return dialogwidget;
}
void cb_dialog_entrybox_focused(Ewl_Widget *w, void *ev, void *data)
{
    ewl_text_all_select(EWL_TEXT(w));
}
void disable_optionboxes()
{
    int count;
    Ewl_Widget *curwidget;
    char tempname[100];
    for(count=1;count<=8;count++)
    {
        sprintf(tempname,"dlg_p1_hb%d",count);
        curwidget=ewl_widget_name_find(tempname);
        if(curwidget==NULL)
            continue;
        else
        {
            ewl_widget_disable(curwidget);
            ewl_widget_configure(curwidget);
        }
    }    
    selectedflag=0;    
   
}

static void cb_dialog_revealed(Ewl_Widget *w, void *ev, void *data)
{
    disable_optionboxes();
}

void cb_dialog_key_down(Ewl_Widget *w, void *ev, void *data)
{
    Ewl_Widget *curwidget;
    int count;
    char tempname[100];
    Ewl_Event_Key_Down *e;
    e = (Ewl_Event_Key_Down*)ev;
    int k = translate_key(e);
    switch(k)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            if(!selectedflag)
            {
                selectedflag=0;
                for(count=1;count<=8;count++)
                {
                    sprintf(tempname,"dlg_p1_hb%d",count);
                    curwidget=ewl_widget_name_find(tempname);
                    if(curwidget==NULL)
                        continue;
                    else if(k==count)
                    {
                        ewl_widget_enable(curwidget);
                        ewl_widget_focus_send(curwidget);
                        ewl_widget_configure(curwidget);
                        ewl_embed_tab_order_next(EWL_EMBED(dialogwidget));
                        selectedflag=1;    
                    }
                    else
                    {
                        ewl_widget_disable(curwidget);
                        ewl_widget_configure(curwidget);
                    }
                }
            }
            break;
        case K_RETURN:
            if(selectedflag)
            {
                int startind=ecore_dlist_index(EWL_EMBED(dialogwidget)->tab_order);
                ewl_embed_tab_order_next(EWL_EMBED(dialogwidget));
                int endind=ecore_dlist_index(EWL_EMBED(dialogwidget)->tab_order);
                if(endind<startind)
                    disable_optionboxes();

            }
            else
            {
                commit_settings();
                ewl_widget_destroy(dialogwidget);
            }
            break;
        case K_ESCAPE:
            if(selectedflag)
                disable_optionboxes();
            else
            {
                ewl_widget_destroy(dialogwidget);
                //ewl_widget_unrealize(dialogwidget);
            }
            break;
        default:
            return;
    }
}










void opt_dlg_init()
{
    //set up options dialog
    char tempo[100];
    
    dialogwidget=ewl_window_new();
    //ewl_window_transient_for(EWL_WINDOW(dialogwidget),EWL_WINDOW(win));
    ewl_window_dialog_set(EWL_WINDOW(dialogwidget), 1);
    //ewl_window_borderless_set(EWL_WINDOW(dialogwidget),1);
    ewl_callback_append(dialogwidget, EWL_CALLBACK_KEY_DOWN, cb_dialog_key_down, NULL);
    ewl_callback_append(dialogwidget, EWL_CALLBACK_REVEAL,cb_dialog_revealed,NULL);
    
    Ewl_Widget *dlg_vbox=ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(dialogwidget),dlg_vbox);
    ewl_widget_show(dlg_vbox);
    
    Ewl_Widget *dlg_hbox1=ewl_hbox_new();
    ewl_container_child_append(EWL_CONTAINER(dlg_vbox),dlg_hbox1);
    //ewl_theme_data_str_set(EWL_WIDGET(dlg_hbox1),"/hbox/file","/usr/share/madpdf/madpdf.edj");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_hbox1),"/hbox/group","ewl/box/dlg_optionbox");
    ewl_widget_focusable_set(dlg_hbox1,1);
    //ewl_widget_appearance_part_text_set(dlg_hbox1,"ewl/box/dlg_optionbox/text","1.");
    //ewl_object_fill_policy_set(EWL_OBJECT(dlg_hbox1),EWL_FLAG_FILL_HFILL);
    ewl_widget_name_set(dlg_hbox1,"dlg_p1_hb1");
    //ewl_widget_disable(dlg_hbox1);
    ewl_widget_show(dlg_hbox1);
    
    Ewl_Widget *dlg_btn_label1=ewl_label_new();
    //ewl_theme_data_str_set(EWL_WIDGET(dlg_btn_label1),"/label/file","/usr/share/madpdf/madpdf.edj");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_btn_label1),"/label/group","ewl/label/dlg_label");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_btn_label1),"/label/textpart","ewl/label/dlg_label/text");
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox1),dlg_btn_label1);
    ewl_label_text_set(EWL_LABEL(dlg_btn_label1),"1. Pan Percentage  ");
    //ewl_object_fill_policy_set(EWL_OBJECT(dlg_btn_label1),EWL_FLAG_FILL_HFILL);
    ewl_widget_configure(dlg_btn_label1);
    ewl_widget_show(dlg_btn_label1);
    
    Ewl_Widget *dlg_entry1_label=ewl_label_new();
    //ewl_theme_data_str_set(EWL_WIDGET(dlg_entry1_label),"/label/file","/usr/share/madpdf/madpdf.edj");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry1_label),"/label/group","ewl/label/dlg_label");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry1_label),"/label/textpart","ewl/label/dlg_label/text");
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox1),dlg_entry1_label);
    ewl_label_text_set(EWL_LABEL(dlg_entry1_label),"H:");
    ewl_widget_show(dlg_entry1_label);

    Ewl_Widget *dlg_entry1=ewl_entry_new();
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox1),dlg_entry1);
    ewl_callback_append(dlg_entry1, EWL_CALLBACK_FOCUS_IN,cb_dialog_entrybox_focused,NULL);
    sprintf(tempo,"%d",get_settings()->hpan);
    ewl_text_text_set(EWL_TEXT(dlg_entry1),tempo);
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry1),"/entry/group","ewl/dlg_entry");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry1),"/entry/cursor/group","ewl/dlg_entry/cursor");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry1),"/entry/selection_area/group","ewl/dlg_entry/selection");
    //ewl_object_fill_policy_set(EWL_OBJECT(dlg_entry1),EWL_FLAG_FILL_HSHRINKABLE);
    ewl_widget_name_set(dlg_entry1,"dlg_p1_hb1_entry1");
    ewl_widget_show(dlg_entry1);
    
    Ewl_Widget *dlg_entry2_label=ewl_label_new();
    //ewl_theme_data_str_set(EWL_WIDGET(dlg_entry2_label),"/label/file","/usr/share/madpdf/madpdf.edj");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry2_label),"/label/group","ewl/label/dlg_label");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry2_label),"/label/textpart","ewl/label/dlg_label/text");
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox1),dlg_entry2_label);
    ewl_label_text_set(EWL_LABEL(dlg_entry2_label),"V:");
    ewl_widget_show(dlg_entry2_label);
    
    Ewl_Widget *dlg_entry2=ewl_entry_new();
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox1),dlg_entry2);
    ewl_callback_append(dlg_entry2, EWL_CALLBACK_FOCUS_IN,cb_dialog_entrybox_focused,NULL);
    //ewl_object_fill_policy_set(EWL_OBJECT(dlg_entry1),EWL_FLAG_FILL_HSHRINKABLE);
    sprintf(tempo,"%d",get_settings()->vpan);
    ewl_text_text_set(EWL_TEXT(dlg_entry2),tempo);
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry2),"/entry/group","ewl/dlg_entry");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry2),"/entry/cursor/group","ewl/dlg_entry/cursor");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry2),"/entry/selection_area/group","ewl/dlg_entry/selection");
    ewl_widget_name_set(dlg_entry2,"dlg_p1_hb1_entry2");
    ewl_widget_show(dlg_entry2);
    
    Ewl_Widget *dlg_hbox2=ewl_hbox_new();
    ewl_container_child_append(EWL_CONTAINER(dlg_vbox),dlg_hbox2);
    ewl_widget_focusable_set(dlg_hbox2,1);
    //ewl_theme_data_str_set(EWL_WIDGET(dlg_hbox2),"/hbox/file","/usr/share/madpdf/madpdf.edj");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_hbox2),"/hbox/group","ewl/box/dlg_optionbox");
    ewl_widget_name_set(dlg_hbox2,"dlg_p1_hb2");
    ewl_widget_show(dlg_hbox2);
    
    Ewl_Widget *dlg_btn_label2=ewl_label_new();
    //ewl_theme_data_str_set(EWL_WIDGET(dlg_btn_label2),"/label/file","/usr/share/madpdf/madpdf.edj");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_btn_label2),"/label/group","ewl/label/dlg_label");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_btn_label2),"/label/textpart","ewl/label/dlg_label/text");
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox2),dlg_btn_label2);
    ewl_label_text_set(EWL_LABEL(dlg_btn_label2),"2. Zoom Increment (%)  ");
    ewl_widget_show(dlg_btn_label2);
    
    Ewl_Widget *dlg_entry3=ewl_entry_new();
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox2),dlg_entry3);
    ewl_callback_append(dlg_entry3, EWL_CALLBACK_FOCUS_IN,cb_dialog_entrybox_focused,NULL);
    sprintf(tempo,"%d",get_settings()->zoominc);
    ewl_text_text_set(EWL_TEXT(dlg_entry3),tempo);
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry3),"/entry/group","ewl/dlg_entry");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry3),"/entry/cursor/group","ewl/dlg_entry/cursor");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry3),"/entry/selection_area/group","ewl/dlg_entry/selection");
    ewl_widget_name_set(dlg_entry3,"dlg_p1_hb2_entry1");
    ewl_widget_show(dlg_entry3);
    
    
    
    Ewl_Widget *dlg_hbox3=ewl_hbox_new();
    ewl_container_child_append(EWL_CONTAINER(dlg_vbox),dlg_hbox3);
    ewl_widget_focusable_set(dlg_hbox3,1);
    //ewl_object_custom_h_set(EWL_OBJECT(dlg_hbox3),50);
    //ewl_theme_data_str_set(EWL_WIDGET(dlg_hbox3),"/hbox/file","/usr/share/madpdf/madpdf.edj");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_hbox3),"/hbox/group","ewl/box/dlg_optionbox");
    //ewl_widget_appearance_part_text_set(dlg_hbox3,"ewl/box/dlg_optionbox/text","3.");
    ewl_widget_name_set(dlg_hbox3,"dlg_p1_hb3");
    
    ewl_widget_show(dlg_hbox3);
    
    Ewl_Widget *dlg_btn_label3=ewl_label_new();
    //ewl_theme_data_str_set(EWL_WIDGET(dlg_btn_label3),"/label/file","/usr/share/madpdf/madpdf.edj");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_btn_label3),"/label/group","ewl/label/dlg_label");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_btn_label3),"/label/textpart","ewl/label/dlg_label/text");
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox3),dlg_btn_label3);
    ewl_label_text_set(EWL_LABEL(dlg_btn_label3),"3. Trim Padding (px)  ");
    ewl_widget_show(dlg_btn_label3);
    
    Ewl_Widget *dlg_entry4_label=ewl_label_new();
    //ewl_theme_data_str_set(EWL_WIDGET(dlg_entry4_label),"/label/file","/usr/share/madpdf/madpdf.edj");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry4_label),"/label/group","ewl/label/dlg_label");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry4_label),"/label/textpart","ewl/label/dlg_label/text");
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox3),dlg_entry4_label);
    ewl_label_text_set(EWL_LABEL(dlg_entry4_label),"L:");
    ewl_widget_show(dlg_entry4_label);

    Ewl_Widget *dlg_entry4=ewl_entry_new();
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox3),dlg_entry4);
    ewl_callback_append(dlg_entry4, EWL_CALLBACK_FOCUS_IN,cb_dialog_entrybox_focused,NULL);
    sprintf(tempo,"%d",get_settings()->ltrimpad);
    ewl_text_text_set(EWL_TEXT(dlg_entry4),tempo);
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry4),"/entry/group","ewl/dlg_entry");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry4),"/entry/cursor/group","ewl/dlg_entry/cursor");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry4),"/entry/selection_area/group","ewl/dlg_entry/selection");
    ewl_widget_name_set(dlg_entry4,"dlg_p1_hb3_entry1");
    ewl_widget_show(dlg_entry4);
    
    Ewl_Widget *dlg_entry5_label=ewl_label_new();
    //ewl_theme_data_str_set(EWL_WIDGET(dlg_entry5_label),"/label/file","/usr/share/madpdf/madpdf.edj");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry5_label),"/label/group","ewl/label/dlg_label");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry5_label),"/label/textpart","ewl/label/dlg_label/text");
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox3),dlg_entry5_label);
    ewl_label_text_set(EWL_LABEL(dlg_entry5_label),"R:");
    ewl_widget_show(dlg_entry5_label);
    
    Ewl_Widget *dlg_entry5=ewl_entry_new();
    ewl_container_child_append(EWL_CONTAINER(dlg_hbox3),dlg_entry5);
    ewl_callback_append(dlg_entry5, EWL_CALLBACK_FOCUS_IN,cb_dialog_entrybox_focused,NULL);
    //ewl_object_fill_policy_set(EWL_OBJECT(dlg_entry5),EWL_FLAG_FILL_HSHRINKABLE);
    sprintf(tempo,"%d",get_settings()->rtrimpad);
    ewl_text_text_set(EWL_TEXT(dlg_entry5),tempo);
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry5),"/entry/group","ewl/dlg_entry");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry5),"/entry/cursor/group","ewl/dlg_entry/cursor");
    ewl_theme_data_str_set(EWL_WIDGET(dlg_entry5),"/entry/selection_area/group","ewl/dlg_entry/selection");
    ewl_widget_name_set(dlg_entry5,"dlg_p1_hb3_entry2");
    ewl_widget_show(dlg_entry5);
}
