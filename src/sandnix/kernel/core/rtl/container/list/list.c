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

#include "../../obj/obj.h"
#include "../../../mm/mm.h"
#include "list.h"

static	void	qsort_adjust(plist_node_t begin, plist_node_t end,
                             item_compare_t compare, bool b2s);

plist_node_t core_rtl_list_insert_before(plist_node_t pos, plist_t p_list,
        void* p_item, pheap_t heap)
{
    plist_node_t p_new_node;

    //Allocate new node
    p_new_node = core_mm_heap_alloc(sizeof(list_node_t), heap);

    if(p_new_node == NULL) {
        return NULL;
    }

    if(pos == NULL) {
        //Insert in the front of the list
        if(*p_list == NULL) {
            (*p_list) = p_new_node;
            p_new_node->p_prev = p_new_node;
            p_new_node->p_next = p_new_node;
            p_new_node->p_item = p_item;

        } else {
            p_new_node->p_prev = (*p_list)->p_prev;
            (*p_list)->p_prev->p_next = p_new_node;
            (*p_list)->p_prev = p_new_node;
            p_new_node->p_next = *p_list;
            p_new_node->p_item = p_item;
            *p_list = p_new_node;
        }

    } else {
        p_new_node->p_prev = pos->p_prev;
        pos->p_prev->p_next = p_new_node;
        pos->p_prev = p_new_node;
        p_new_node->p_next = pos;
        p_new_node->p_item = p_item;

        if(pos == *p_list) {
            *p_list = p_new_node;
        }
    }

    return p_new_node;
}

plist_node_t core_rtl_list_insert_node_before(plist_node_t pos, plist_t p_list,
        plist_node_t p_node)
{
    if(pos == NULL) {
        //Insert in the front of the list
        if(*p_list == NULL) {
            (*p_list) = p_node;
            p_node->p_prev = p_node;
            p_node->p_next = p_node;

        } else {
            p_node->p_prev = (*p_list)->p_prev;
            (*p_list)->p_prev->p_next = p_node;
            (*p_list)->p_prev = p_node;
            p_node->p_next = *p_list;
            *p_list = p_node;
        }

    } else {
        p_node->p_prev = pos->p_prev;
        pos->p_prev->p_next = p_node;
        pos->p_prev = p_node;
        p_node->p_next = pos;

        if(pos == *p_list) {
            *p_list = p_node;
        }
    }

    return p_node;
}

plist_node_t core_rtl_list_insert_after(plist_node_t pos, plist_t p_list,
                                        void* p_item, pheap_t heap)
{
    plist_node_t p_new_node;

    //Allocate new node
    p_new_node = core_mm_heap_alloc(sizeof(list_node_t), heap);

    if(p_new_node == NULL) {
        return NULL;
    }

    if(pos == NULL) {
        //Insert at the end of the list
        if(*p_list == NULL) {
            (*p_list) = p_new_node;
            p_new_node->p_prev = p_new_node;
            p_new_node->p_next = p_new_node;
            p_new_node->p_item = p_item;

        } else {
            p_new_node->p_prev = (*p_list)->p_prev;
            (*p_list)->p_prev->p_next = p_new_node;
            (*p_list)->p_prev = p_new_node;
            p_new_node->p_next = *p_list;
            p_new_node->p_item = p_item;
        }

    } else {
        p_new_node->p_prev = pos;
        p_new_node->p_next = pos->p_next;
        p_new_node->p_prev->p_next = p_new_node;
        p_new_node->p_next->p_prev = p_new_node;
        p_new_node->p_item = p_item;

    }

    return p_new_node;
}

plist_node_t core_rtl_list_insert_node_after(plist_node_t pos, plist_t p_list,
        plist_node_t p_node)
{
    if(pos == NULL) {
        //Insert at the end of the list
        if(*p_list == NULL) {
            (*p_list) = p_node;
            p_node->p_prev = p_node;
            p_node->p_next = p_node;

        } else {
            p_node->p_prev = (*p_list)->p_prev;
            (*p_list)->p_prev->p_next = p_node;
            (*p_list)->p_prev = p_node;
            p_node->p_next = *p_list;
        }

    } else {
        p_node->p_prev = pos;
        p_node->p_next = pos->p_next;
        p_node->p_prev->p_next = p_node;
        p_node->p_next->p_prev = p_node;

    }

    return p_node;
}

void* core_rtl_list_remove(plist_node_t pos, plist_t p_list, pheap_t heap)
{
    void* ret;

    if(pos == NULL) {
        return NULL;
    }

    ret = pos->p_item;

    if(pos->p_prev == pos) {
        *p_list = NULL;

    } else {
        if(*p_list == pos) {
            *p_list = pos->p_next;
        }

        pos->p_prev->p_next = pos->p_next;
        pos->p_next->p_prev = pos->p_prev;
    }

    core_mm_heap_free(pos, heap);

    return ret;
}

void* core_rtl_list_node_remove(plist_node_t pos, plist_t p_list)
{
    void* ret;

    if(pos == NULL) {
        return NULL;
    }

    ret = pos->p_item;

    if(pos->p_prev == pos) {
        *p_list = NULL;

    } else {
        if(*p_list == pos) {
            *p_list = pos->p_next;
        }

        pos->p_prev->p_next = pos->p_next;
        pos->p_next->p_prev = pos->p_prev;
    }

    return ret;
}

void core_rtl_list_destroy(plist_t p_list, pheap_t heap,
                           item_destroyer_t destroier, void* arg)
{
    plist_node_t p_node;
    plist_node_t p_remove_node;

    if(*p_list == NULL) {
        return;
    }

    p_node = *p_list;

    do {
        p_remove_node = p_node;
        p_node = p_node->p_next;

        if(destroier != NULL) {
            destroier(p_remove_node->p_item, arg);
        }

        core_mm_heap_free(p_remove_node, heap);
    } while(p_node != *p_list);

    *p_list = NULL;
    return;
}

void core_rtl_list_join(plist_t p_src, plist_t p_dest, pheap_t src_heap,
                        pheap_t dest_heap)
{
    while(*p_src != NULL) {
        core_rtl_list_insert_after(
            NULL,
            p_dest,
            core_rtl_list_remove(*p_src, p_src, src_heap),
            dest_heap);
    }

    return;
}

void core_rtl_list_qsort(plist_t p_list, item_compare_t compare, bool b2s)
{
    if(*p_list != (*p_list)->p_prev) {
        qsort_adjust(*p_list, (*p_list)->p_prev, compare, b2s);
    }

    return;
}

void qsort_adjust(plist_node_t begin, plist_node_t end,
                  item_compare_t compare, bool b2s)

{
    plist_node_t p_front;
    plist_node_t p_back;
    void* p_key;
    void* p_tmp_item;
    int cmp_result;

    p_key = begin->p_item;
    p_front = begin;
    p_back = end;

    while(p_front != p_back) {
        //Back
        while(p_front != p_back) {
            cmp_result = compare(p_back->p_item, p_key);

            if(b2s) {
                cmp_result = -cmp_result;
            }

            if(cmp_result < 0) {
                break;
            }

            p_back = p_back->p_prev;
        }

        p_tmp_item = p_front->p_item;
        p_front->p_item = p_back->p_item;
        p_back->p_item = p_tmp_item;

        //Front
        while(p_front != p_back) {
            cmp_result = compare(p_front->p_item, p_key);

            if(b2s) {
                cmp_result = -cmp_result;
            }

            if(cmp_result > 0) {
                break;
            }

            p_front = p_front->p_next;
        }

        p_tmp_item = p_front->p_item;
        p_front->p_item = p_back->p_item;
        p_back->p_item = p_tmp_item;
    }

    if(begin != p_front->p_prev && begin != p_front) {
        qsort_adjust(begin, p_front->p_prev, compare, b2s);
    }

    if(end != p_back->p_next && end != p_back) {
        qsort_adjust(p_back->p_next, end, compare, b2s);
    }

    return;
}
