/*
	Copyright 2015,暗夜幽灵 <darknightghost.cn@gmail.com>

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

#ifndef	MM_H_INCLUDE
#define	MM_H_INCLUDE

#ifdef	X86
	#include "../../common/arch/x86/types.h"
	#include "paging/arch/x86/page_table.h"
	#include "heap/heap.h"
#endif	//X86

void		mm_heap_create();
void		mm_heap_destroy();
void*		mm_heap_alloc(size_t size, void* heap_addr);
bool		mm_heap_chk(void* heap_addr);
void		mm_heap_free(void* addr, void* heap_addr);

#endif	//!	MM_H_INCLUDE

