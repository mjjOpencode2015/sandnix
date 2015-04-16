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

#ifndef	KERNEL_IMAGE_H_INCLUDE
#define	KERNEL_IMAGE_H_INCLUDE

#include "types.h"

#define	KERNEL_MEM_BASE					0xC0000000
#define	KERNEL_BASE						0xC0100400
#define	KERNEL_PARAMETER_PHYSICAL		0x00100000
#define	KERNEL_PARAMETER				0xC0100000
#define	KERNEL_PARAMETER_MAX_LEN		1024
#define	VIRTUAL_ADDR_OFFSET				KERNEL_MEM_BASE
#define	KERNEL_MAX_SIZE					(1024*8)		//The maximum size of kernel in memory

#endif	//!	KERNEL_IMAGE_H_INCLUDE