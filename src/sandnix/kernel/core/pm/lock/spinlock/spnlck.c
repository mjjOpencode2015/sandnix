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

#include "spnlck.h"
#include "../../../../hal/rtl/rtl.h"
#include "../../../../hal/exception/exception.h"
#include "../../../../hal/cpu/cpu.h"
#include "../../../exception/exception.h"
#include "../../pm.h"

void core_pm_spnlck_init(pspnlck_t p_lock)
{
    p_lock->owner = 0;
    p_lock->ticket = 0;
    p_lock->priority = 0;
    return;
}

void core_pm_spnlck_lock(pspnlck_t p_lock)
{
    u16 ticket;
    u32 priority;

    //Get ticket
    ticket = 1;
    hal_rtl_atomic_xaddw(p_lock->ticket, ticket);

    //Get lock
    priority = core_pm_get_currnt_thrd_priority();

    if(priority < PRIORITY_HIGHEST) {
        core_pm_set_currnt_thrd_priority(PRIORITY_HIGHEST);
    }

    while(p_lock->owner != ticket) {
        if(p_lock->cpu_index == hal_cpu_get_cpu_index()) {
            //Dead lock, raise exception
            pedeadlock_except_t p_except = edeadlock_except();
            RAISE(p_except, "Trying to get a spinlock whitch has been got.");
        }

        MEM_BLOCK;
    }

    p_lock->priority = priority;
    p_lock->cpu_index = hal_cpu_get_cpu_index();

    return;
}

void core_pm_spnlck_raw_lock(pspnlck_t p_lock)
{
    u16 ticket;

    //Get ticket
    ticket = 1;
    hal_rtl_atomic_xaddw(p_lock->ticket, ticket);

    //Get lock
    while(p_lock->owner != ticket) {
        if(p_lock->cpu_index == hal_cpu_get_cpu_index()) {
            //Dead lock, raise exception
            pedeadlock_except_t p_except = edeadlock_except();
            RAISE(p_except, "Trying to get a spinlock whitch has been got.");
        }

        MEM_BLOCK;
    }

    p_lock->cpu_index = hal_cpu_get_cpu_index();

    return;
}

kstatus_t core_pm_spnlck_trylock(pspnlck_t p_lock)
{
    u32 priority;
    u32 old_lock;
    u32 new_lock;
    u32 result;

    priority = core_pm_get_currnt_thrd_priority();

    if(priority < PRIORITY_HIGHEST) {
        core_pm_set_currnt_thrd_priority(PRIORITY_HIGHEST);
    }

    old_lock = p_lock->lock;
    new_lock = old_lock;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
    (*((u16*)(&new_lock)))++;
#pragma GCC diagnostic pop

    hal_rtl_atomic_cmpxchgl(p_lock->lock, new_lock, old_lock, result);

    if(!result) {
        core_pm_set_currnt_thrd_priority(priority);
        return EAGAIN;

    } else {
        p_lock->priority = priority;
        p_lock->cpu_index = hal_cpu_get_cpu_index();
        return ESUCCESS;
    }
}

kstatus_t core_pm_spnlck_raw_trylock(pspnlck_t p_lock)
{
    u32 old_lock;
    u32 new_lock;
    u32 result;

    old_lock = p_lock->lock;
    new_lock = old_lock;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
    (*((u16*)(&new_lock)))++;
#pragma GCC diagnostic pop

    hal_rtl_atomic_cmpxchgl(p_lock->lock, new_lock, old_lock, result);

    if(!result) {
        return EAGAIN;

    } else {
        p_lock->cpu_index = hal_cpu_get_cpu_index();
        return ESUCCESS;
    }
}

void core_pm_spnlck_unlock(pspnlck_t p_lock)
{
    u32 priority;

    priority = p_lock->priority;

    u16 ticket = p_lock->ticket;

    u16 inc_val = 1;
    hal_rtl_atomic_xaddw(p_lock->owner, inc_val);

    core_pm_set_currnt_thrd_priority(priority);

    if(ticket - (inc_val + (u16)1) > ticket - inc_val) {
        //Dead lock next time, raise exception
        pedeadlock_except_t p_except = edeadlock_except();
        RAISE(p_except, "Trying to release a spinlock whitch has been released.");
    }

    return;
}

void core_pm_spnlck_raw_unlock(pspnlck_t p_lock)
{

    u16 inc_val = 1;
    hal_rtl_atomic_xaddw(p_lock->owner, inc_val);
    u16 ticket = p_lock->ticket;

    if(ticket - (inc_val + (u16)1) > ticket - inc_val) {
        //Dead lock next time, raise exception
        pedeadlock_except_t p_except = edeadlock_except();
        RAISE(p_except, "Trying to release a spinlock whitch has been released.");
    }

    return;
}
