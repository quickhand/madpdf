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

#ifndef ZLEWLENTRY_H
#define ZLEWLENTRY_H

#include <Ewl.h>

typedef void (*entry_handler)(int value, Ewl_Widget *parent);
Ewl_Widget *init_entry(char *text, int value, entry_handler handler, Ewl_Widget *parent, int master);
Ewl_Widget *entry_get_parent(Ewl_Widget *w);
void fini_entry(Ewl_Widget *win);

#endif
