/*
	  Copyright 2016,暗夜幽灵 <darknightghost.cn@gmail.com>

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

#include "../../../../common/common.h"
#include "../../rtl/rtl.h"
#include "../../om/om.h"

#ifdef	X86
	#include "../arch/x86/phymem/phymem.h"
#endif	//!	X86

#define	PHY_MEM_ALLOCATABLE		0
#define	PHY_MEM_DMA				1
#define	PHY_MEM_ALLOCATED		2
#define	PHY_MEM_DMA_ALLOCATED	3
#define	PHY_MEM_RESERVED		4
#define	PHY_MEM_SYSTEM			5
#define	PHY_MEM_BAD				6

#define	PHYMEM_HEAP_SIZE			4096
#define	PHY_INIT_BITMAP_SIZE		4096

#define	PHYMEM_BITMAP_NORMAL		0
#define	PHYMEM_BITMAP_DMA			1

#ifndef	_ASM
typedef	struct	_phymem_tbl_entry {
	void*	base;
	size_t	size;
	u32		status;
} phymem_tbl_entry_t, *pphymem_tbl_entry_t;

typedef	struct	_phymem_bitmap {
	void*		base;
	size_t		num;
	size_t		avail_num;
	u32			status;
	pbitmap_t	p_bitmap;
} phymem_bitmap_t, *pphymem_bitmap_t;

typedef	struct	_phymem_block {
	void*		base;
	size_t		num;
} phymem_block_t, *pphymem_block_t;

typedef	struct	_phymem_obj {
	kobject_t		obj;
	phymem_block_t	mem_block;
} phymem_obj_t, *pphymem_obj_t;

void	phymem_init();
void	phymem_manage_all();

pphymem_obj_t	mm_phymem_alloc(size_t num, bool is_dma);

#endif	//!	_ASM