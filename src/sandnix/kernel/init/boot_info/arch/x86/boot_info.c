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

#include "../../../../../../common/common.h"
#include "../../../../../boot/multiboot2.h"
#include "../../../../debug/debug.h"
#include "../../../../exceptions/exceptions.h"
#include "../../../../rtl/rtl.h"
#include "../../../../mm/mm.h"
#include "../../../init.h"
#include "../../boot_info.h"

static	boot_info_t	boot_info;

static	void	analyse_cmdline(char* p_cmdline);
static	char*	jmp_space(char* p);
static	char*	get_word(char* p_start, char* buf);
static	void	analyse_mmap(pmultiboot_tag_mmap_t p_tag);

void analyse_boot_info(u32 magic, void* p_load_info)
{
	pmultiboot_tag_t p_tag;
	void* p_current;
	u32 boot_info_size;
	pmultiboot_tag_module_t p_module_info;
	pmultiboot_tag_string_t p_cmdline_info;
	pmultiboot_tag_mmap_t p_mmap_info;

	dbg_kprint("\nAnalysing boot informations...\n");

	//Check magic
	if(magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
		excpt_panic(EKERNELARG, "Illegal multiboot2 magic.\n");
	}

	rtl_memset(&boot_info, 0, sizeof(boot_info));

	//Analyse tags
	dbg_kprint("Boot information at %P.\n", p_load_info);
	p_current = p_load_info;
	boot_info_size = *((u32*)p_current);
	p_current = (void*)((u32)p_current + sizeof(u32) * 2);

	while((u32)p_current - (u32)p_load_info < boot_info_size) {
		p_tag = (pmultiboot_tag_t)p_current;

		if(p_tag->type == MULTIBOOT_TAG_TYPE_END) {
			break;
		}

		switch(p_tag->type) {
			//Init ramdisk
			case MULTIBOOT_TAG_TYPE_MODULE:
				p_module_info = (pmultiboot_tag_module_t)p_tag;
				boot_info.initrd_begin = (void*)(p_module_info->mod_start
				                                 + KERNEL_MEM_BASE);
				boot_info.initrd_size = (size_t)(p_module_info->mod_end
				                                 - p_module_info->mod_start + 1);

				dbg_kprint("Initrd address : %P.\nInitrd size : %P.\n",
				           boot_info.initrd_begin, boot_info.initrd_size);
				break;

			//Kerne; command line
			case MULTIBOOT_TAG_TYPE_CMDLINE:
				p_cmdline_info = (pmultiboot_tag_string_t)p_tag;
				analyse_cmdline(p_cmdline_info->string);
				dbg_kprint("Kernel command line : \"%s\".\n",
				           p_cmdline_info->string);
				break;

			//Physical memory info
			case MULTIBOOT_TAG_TYPE_MMAP:
				p_mmap_info = (pmultiboot_tag_mmap_t)p_tag;
				analyse_mmap(p_mmap_info);
				break;
		}

		p_current = (void*)((u32)p_current + p_tag->size);

		//Make the pointer 8-bytes aligned
		if((u32)p_current % 8 != 0) {
			p_current = (void*)((u32)p_current + (8 - (u32)p_current % 8));
		}
	}

	return;
}

void analyse_cmdline(char* p_cmdline)
{
	char* p_key;
	char* p_value;
	char* p_start;
	char* p_end;
	char* buf;
	size_t len;
	pkernel_param_t p_param;

	buf = mm_hp_alloc(rtl_strlen(p_cmdline) + 1, NULL);

	for(p_start = jmp_space(p_cmdline);
	    *p_start != '\0';
	    p_start = jmp_space(p_end)) {
		//Get key
		p_end = get_word(p_start, buf);
		len = rtl_strlen(buf) + 1;
		p_key = mm_hp_alloc(len, NULL);
		rtl_strncpy(p_key, len, buf);

		p_end = jmp_space(p_end);

		if(*p_end == '=') {
			//Get value
			p_start = p_end + 1;
			p_end = get_word(p_start, buf);
			len = rtl_strlen(buf) + 1;
			p_value = mm_hp_alloc(len, NULL);
			rtl_strncpy(p_value, len, buf);

		} else {
			p_value = "";
		}

		p_param = mm_hp_alloc(sizeof(kernel_param_t), NULL);
		p_param->key = p_key;
		p_param->value = p_value;

		rtl_list_insert_after(&(boot_info.kernel_params), NULL, p_param, NULL);
	}

	mm_hp_free(buf, NULL);
	return;
}

char* jmp_space(char* p)
{
	while(*p == ' ' || *p == '\t') {
		p++;
	}

	return p;
}

char* get_word(char* p_start, char* buf)
{
	char* p_str;
	char* p_buf;
	bool quote_flag;
	bool escap_flag;

	p_str = jmp_space(p_start);
	p_buf = buf;

	quote_flag = false;
	escap_flag = false;

	while(1) {
		if(*p_str == '\0') {
			break;
		}

		if(escap_flag) {
			*p_buf = *p_str;
			escap_flag = false;
			p_buf++;
			p_str++;

		} else {
			if(*p_str == '\"') {
				quote_flag = !quote_flag;
				p_str++;

			} else if(*p_str == '\\') {
				escap_flag = true;
				p_str++;

			} else if(!quote_flag
			          && (*p_str == ' ' || *p_str == '\t')) {
				break;

			} else if(*p_str == '=') {
				break;

			} else {
				*p_buf = *p_str;
				p_buf++;
				p_str++;
			}
		}
	}

	*p_buf = '\0';
	return p_str;
}

void analyse_mmap(pmultiboot_tag_mmap_t p_tag)
{
	UNREFERRED_PARAMETER(p_tag);
}

