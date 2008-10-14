/*
 * Copyright (C) 2008 Alexander Kerner <lunohod@openinkpot.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include "madpdf.h"
#include "Dialogs.h"
#include "Choicebox.h"
#include "Entry.h"
#include "settings.h"
#include <ewl/Ewl.h>




// Options dialogs

static long minl(long a, long b)
{
   if(a < b) return a;
   return b;
}

Ewl_Widget *zoom_inc_entry,*pan_inc_h_entry,*pan_inc_v_entry,*pad_trim_l_entry,*pad_trim_r_entry;



void zoom_inc_handler(int value, Ewl_Widget *parent)
{
	if(value >= 0) {
		get_settings()->zoominc = value;
		char *o;
		asprintf(&o, "%d%%", get_settings()->zoominc);
		update_label(entry_get_parent(parent), 0, o);
		if(o)
			free(o);
        
	}
	fini_entry(zoom_inc_entry);

	//redraw_text();
}
void pan_inc_h_handler(int value, Ewl_Widget *parent)
{
	if(value >= 0) {
		get_settings()->hpan = (int) minl(value, 100);
		char *o;
		asprintf(&o, "%d%%", get_settings()->hpan);
		update_label(entry_get_parent(parent), 1, o);
		if(o)
			free(o);
        update_main_app();
	}
	fini_entry(pan_inc_h_entry);

	//redraw_text();
}
void pan_inc_v_handler(int value, Ewl_Widget *parent)
{
	if(value >= 0) {
		get_settings()->vpan = (int) minl(value, 100);
		char *o;
		asprintf(&o, "%d%%", get_settings()->vpan);
		update_label(entry_get_parent(parent), 2, o);
		if(o)
			free(o);
        update_main_app();
	}
	fini_entry(pan_inc_v_entry);

	//redraw_text();
}
void pad_trim_l_handler(int value, Ewl_Widget *parent)
{
	if(value >= 0) {
		get_settings()->ltrimpad = (int) minl(value, 100);
		char *o;
		asprintf(&o, "%dpx", get_settings()->ltrimpad);
		update_label(entry_get_parent(parent), 3, o);
		if(o)
			free(o);
        update_main_app();
	}
	fini_entry(pad_trim_l_entry);

	//redraw_text();
}
void pad_trim_r_handler(int value, Ewl_Widget *parent)
{
	if(value >= 0) {
		get_settings()->rtrimpad = (int) minl(value, 100);
		char *o;
		asprintf(&o, "%dpx", get_settings()->rtrimpad);
		update_label(entry_get_parent(parent), 4, o);
		if(o)
			free(o);
        update_main_app();
	}
	fini_entry(pad_trim_r_entry);

	//redraw_text();
}

void options_dialog_choicehandler(int choice, Ewl_Widget *parent)
{
    progsettings *cursettings=get_settings();
	if (choice == 0) {
		ewl_widget_show(zoom_inc_entry = init_entry("Zoom Increment",cursettings->zoominc, zoom_inc_handler, parent,FALSE));
	} else if (choice == 1) {
		ewl_widget_show(pan_inc_h_entry = init_entry("Panning Increment (Hor.)",cursettings->hpan, pan_inc_h_handler, parent,FALSE));
	} else if (choice == 2) {
		ewl_widget_show(pan_inc_v_entry = init_entry("Panning Increment (Ver.)",cursettings->vpan, pan_inc_v_handler, parent,FALSE));
	} else if (choice == 3) {
		ewl_widget_show(pad_trim_l_entry = init_entry("Padding on Trim (Right)",cursettings->ltrimpad, pad_trim_l_handler, parent,FALSE));
	} else if (choice == 4) {
		ewl_widget_show(pad_trim_r_entry = init_entry("Padding on Trim (Right)",cursettings->rtrimpad, pad_trim_r_handler, parent,FALSE));
	}
}

void OptionsDialog()
{
	Ewl_Widget *w = ewl_widget_name_find("mainwindow");

	const char *initchoices[] = { 
		"1. Zoom Increment",
		"2. Panning Increment (Hor.)",
		"3. Panning Increment (Ver.)",
		"4. Padding on Trim (Left)",
		"5. Padding on Trim (Right)",
	};

	progsettings *cursettings=get_settings();
    char *zoom_inc,*pan_inc_h,*pan_inc_v,*pad_trim_l,*pad_trim_r;
    asprintf(&zoom_inc,"%d%%",cursettings->zoominc);
    asprintf(&pan_inc_h,"%d%%",cursettings->hpan);
    asprintf(&pan_inc_v,"%d%%",cursettings->vpan);
    asprintf(&pad_trim_l,"%dpx",cursettings->ltrimpad);
    asprintf(&pad_trim_r,"%dpx",cursettings->rtrimpad);
	const char *values[] = {
		zoom_inc,	
		pan_inc_h,
		pan_inc_v,
		pad_trim_l,
		pad_trim_r,
	};

	ewl_widget_show(init_choicebox(initchoices, values, 5, options_dialog_choicehandler, "Settings", w, TRUE));
    free(zoom_inc);
    free(pan_inc_h);
    free(pan_inc_v);
    free(pad_trim_r);
    free(pad_trim_l);
}

