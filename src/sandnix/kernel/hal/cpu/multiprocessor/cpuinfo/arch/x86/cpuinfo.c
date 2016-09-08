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

#include "../../cpuinfo.h"
#include "../../../../../io/io.h"
#include "../../../../../../core/rtl/rtl.h"
#include "../../../../../../core/pm/pm.h"
#include "../../../../../../core/mm/mm.h"

static	bool		initialized = false;
static	map_t		cpuid_map;
static	pcpuinfo_t	cpuinfo;

static	int			cpuid_cmp(void* cpuid1, void* cpuid2);

void cpuinfo_init()
{
    core_rtl_map_init(&cpuid_map, cpuid_cmp , NULL);
    initialized = true;
}

u32	hal_cpu_get_cpu_id()
{
    return hal_io_apic_read32(LOCAL_APIC_ID_REG);
}

u32	hal_cpu_get_cpu_index()
{
    if(!initialized) {
        return 0;
    }

    pcpuinfo_t p_info = (pcpuinfo_t)core_rtl_map_get(&cpuid_map,
                        (void*)hal_io_apic_read32(LOCAL_APIC_ID_REG));

    if(p_info == NULL) {
        return INVALID_CPU_INDEX;
    }

    return p_info->index;
}

u32	hal_cpu_get_cpu_id_by_index(u32 index);
u32	hal_cpu_get_cpu_index_by_id(u32 id);

int cpuid_cmp(void* cpuid1, void* cpuid2)
{
    if((u32)cpuid1 > (u32)cpuid2) {
        return 1;

    } else if((u32)cpuid1 = (u32)cpuid2) {
        return 0;

    } else {
        return -1;
    }
}