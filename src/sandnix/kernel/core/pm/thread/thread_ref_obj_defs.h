/*
    Copyright 2016,王思远 <darknightghost.cn@gmail.com>

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

#include "../../rtl/obj/obj_defs.h"
#include "../../rtl/container/list/list_defs.h"

struct	_thread_ref_obj;
typedef	struct _thread_ref_obj*		(*thread_obj_fork_t)(struct _thread_ref_obj*, u32, u32);

typedef struct	_thread_ref_obj {
    obj_t			obj;
    u32				process_id;		//Which process the thread belongs to
    u32				thread_id;		//Which thread the object belongs to

    //Create a copy of the object
    //pthread_ref_obj_t		fork(pthread_ref_obj_t p_this, u32 dest_id, u32 dest_process);
    thread_obj_fork_t	fork;	//Create a copy of object
} thread_ref_obj_t, *pthread_ref_obj_t;
