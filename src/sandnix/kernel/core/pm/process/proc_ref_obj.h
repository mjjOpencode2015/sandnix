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

#include "../../rtl/rtl_defs.h"
#include "../../mm/mm_defs.h"

#include "./proc_ref_obj_defs.h"

//Construction function
pproc_ref_obj_t proc_ref_obj(u32 process_id,
                             u32 class_id,
                             proc_ref_obj_fork_t on_fork,
                             destructor_t destructor,
                             compare_obj_t compare_func,
                             to_string_t to_string_func,
                             pheap_t heap, size_t size);

