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

#include "../../../../../../common/common.h"
#include "spnlck.h"

#include "./spnlck_rw_defs.h"

//r/w lock
//Initialize the lock
void		core_pm_spnlck_rw_init(pspnlck_rw_t p_lock);

//Read lock
void		core_pm_spnlck_rw_r_lock(pspnlck_rw_t p_lock, u32* p_priority);

//Read try lock
kstatus_t	core_pm_spnlck_rw_r_trylock(pspnlck_rw_t p_lock, u32* p_priority);

//Read unlock
void		core_pm_spnlck_rw_r_unlock(pspnlck_rw_t p_lock, u32 priority);

//Write lock
void		core_pm_spnlck_rw_w_lock(pspnlck_rw_t p_lock);

//Write try lock
kstatus_t	core_pm_spnlck_rw_w_trylock(pspnlck_rw_t p_lock);

//Write unlock
void		core_pm_spnlck_rw_w_unlock(pspnlck_rw_t p_lock);
