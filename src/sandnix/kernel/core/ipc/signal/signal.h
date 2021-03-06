/*
    Copyright 2017,王思远 <darknightghost.cn@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "../../../../../common/common.h"
#include "./thrd_signal_tbl_obj.h"

#define	MODULE_NAME		core_ipc

void	core_ipc_signal_init();


void	PRIVATE(add_singal_obj)(pthrd_signal_tbl_obj_t p_sig_obj);
void	PRIVATE(signal_quit)(int sig);
void	PRIVATE(signal_quit_with_core_dump)(int sig);
void	PRIVATE(signal_stop)(int sig);
void	PRIVATE(signal_cont)(int sig);

#undef	MODULE_NAME
