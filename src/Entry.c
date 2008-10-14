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

#include "Entry.h"


//extern void redraw_text();

typedef struct _entry_info_struct {
	int master;
	entry_handler handler;
	Ewl_Widget *parent;
} entry_info_struct;

static void entry_reveal_cb(Ewl_Widget *w, void *ev, void *data) {
	ewl_window_move(EWL_WINDOW(w), (600 - CURRENT_W(w)) / 2, (800 - CURRENT_H(w)) / 2);
	ewl_window_keyboard_grab_set(EWL_WINDOW(w), 1);
}

static void entry_realize_cb(Ewl_Widget *w, void *ev, void *data) {
	Ewl_Widget *win;
	win = ewl_widget_name_find("mainwindow");
	if(win)
		ewl_window_keyboard_grab_set(EWL_WINDOW(win), 0);
}

static void entry_unrealize_cb(Ewl_Widget *w, void *ev, void *data) {
	Ewl_Widget *win;
	win = ewl_widget_name_find("mainwindow");
	if(win)
		ewl_window_keyboard_grab_set(EWL_WINDOW(win), 1);
}

static void entry_keyhandler(Ewl_Widget *w, void *ev, void *data)
{
	Ewl_Event_Key_Down *e;
	Ewl_Widget *entry, *dialog;
	char *s;
	int n;

	e = (Ewl_Event_Key_Down*)ev;
	entry = ewl_widget_name_find((char *)ewl_widget_data_get(w, (void *)"entry_name"));

	if(!strcmp(e->base.keyname, "Escape")) {
		s = ewl_text_text_get(EWL_TEXT(entry));
		if(s && (strlen(s) > 0)) {
			free(s);
			ewl_entry_delete_left(EWL_ENTRY(entry));
		} else {
			//ewl_widget_hide(dialog);
			//ewl_widget_destroy(dialog);

			entry_info_struct *info = 
				(entry_info_struct *)ewl_widget_data_get(w, (void *)"entry_info");
			(info->handler)(-1, w);

			//fini_entry(w);
		}
	} else if(!strcmp(e->base.keyname, "Return")) {
		s = ewl_text_text_get(EWL_TEXT(entry));
		if(s) {
			n = atoi(s);
			free(s);
		} else {
			n = 0;
		}

		entry_info_struct *info = 
			(entry_info_struct *)ewl_widget_data_get(w, (void *)"entry_info");
		(info->handler)(n, w);

		//fini_entry(w);
	}
}

Ewl_Widget *init_entry(char *text, int value, entry_handler handler, Ewl_Widget *parent, int master)
{
	Ewl_Widget *w, *label, *entry, *entry_hbox;

	entry_info_struct *info = 
		(entry_info_struct *)malloc(sizeof(entry_info_struct));

	info->handler = handler;
	info->master = master;
	info->parent = parent;

	w = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(w), "Entry");
	ewl_window_name_set(EWL_WINDOW(w), "EWL_WINDOW");
	ewl_window_class_set(EWL_WINDOW(w), "Entry");
	ewl_widget_name_set(w, "entry_win");
	ewl_callback_append(w, EWL_CALLBACK_KEY_UP, entry_keyhandler, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REVEAL, entry_reveal_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, entry_realize_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_UNREALIZE, entry_unrealize_cb, NULL);
	ewl_widget_data_set(EWL_WIDGET(w), (void *)"entry_info", (void *)info);
	ewl_window_keyboard_grab_set(EWL_WINDOW(w), 1);
	EWL_EMBED(w)->x = 600;
	EWL_EMBED(w)->y = 0;
	ewl_widget_show(w);

	entry_hbox = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(w), entry_hbox);
	ewl_widget_show(entry_hbox);

	label = ewl_label_new();
	ewl_label_text_set(EWL_LABEL(label), text);
	ewl_container_child_append(EWL_CONTAINER(entry_hbox), label);
	ewl_widget_show(label);

	entry = ewl_entry_new();
	ewl_object_custom_w_set(EWL_OBJECT(entry), 70);
	ewl_container_child_append(EWL_CONTAINER(entry_hbox), entry);
	ewl_theme_data_str_set(EWL_WIDGET(entry),"/entry/group","ewl/dlg_entry");
	ewl_theme_data_str_set(EWL_WIDGET(entry),"/entry/cursor/group","ewl/dlg_entry/cursor");
	ewl_theme_data_str_set(EWL_WIDGET(entry),"/entry/selection_area/group","ewl/dlg_entry/selection");

	char *val;
	asprintf(&val, "%d", value >= 0 ? value : 0);
	ewl_text_text_append(&EWL_ENTRY(entry)->text, val);
	if(val)
		free(val);

	ewl_widget_name_set(entry, "entry");
	ewl_widget_show(entry);

	ewl_widget_data_set(EWL_WIDGET(w), (void *)"entry_name", (void *)"entry");

	ewl_widget_focus_send(w);
	ewl_widget_focus_send(entry);

	return w;
}

void fini_entry(Ewl_Widget *win)
{
	int master = FALSE;
	ewl_widget_hide(win);
	entry_info_struct *infostruct = 
		(entry_info_struct *)ewl_widget_data_get(win, (void *)"entry_info");

	if(infostruct->master)
		master = TRUE;

	free(infostruct);
	ewl_widget_destroy(win);
	//if(master)
	//	redraw_text();
}

Ewl_Widget *entry_get_parent(Ewl_Widget *w)
{
	entry_info_struct *infostruct;
	infostruct =
		(entry_info_struct *) ewl_widget_data_get(w, (void *)"entry_info");

	return infostruct->parent;
}
