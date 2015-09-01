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

#include "fs.h"
#include "../../pm/pm.h"
#include "../../rtl/rtl.h"
#include "../../exceptions/exceptions.h"
#include "ramdisk/ramdisk.h"
#include "tarfs/tarfs.h"
#include "../../debug/debug.h"

static	array_list_t	file_desc_info_table;
static	mutex_t			file_desc_info_table_lock;
static	array_list_t	file_obj_table;
static	mutex_t			file_obj_table_lock;

static	mount_point_t	root_info;
static	mutex_t			mount_point_lock;

static	pvfs_proc_info	get_proc_fs_info();
static	void			set_proc_fs_info(u32 process_id,
        pvfs_proc_info p_new_info);
static	void			ref_proc_destroy_callback(pfile_obj_ref_t p_item,
        pfile_obj_t p_file_object);
static	void			file_desc_destroy_callback(pfile_desc_t p_fd,
        void* p_null);

void fs_init()
{
	k_status status;
	pvfs_proc_info p_proc0_info;
	pdriver_obj_t p_drv;

	dbg_print("Initializing filesystem...\n");

	//Initialize tables
	status = rtl_array_list_init(&file_desc_info_table,
	                             MAX_PROCESS_NUM,
	                             NULL);

	if(status != ESUCCESS) {
		excpt_panic(status, "Failed to initialize file_desc_info_table.");
	}

	status = rtl_array_list_init(&file_obj_table,
	                             MAX_FILEOBJ_NUM,
	                             NULL);

	if(status != ESUCCESS) {
		excpt_panic(status, "Failed to initialize file_obj_table.");
	}

	pm_init_mutex(&file_desc_info_table_lock);
	pm_init_mutex(&mount_point_lock);

	//Initialize file descriptor for process 0
	p_proc0_info = mm_hp_alloc(sizeof(vfs_proc_info), NULL);

	if(p_proc0_info == NULL) {
		excpt_panic(EFAULT,
		            "Failed to initialize file descriptor for process 0");
	}

	pm_init_mutex(&(p_proc0_info->lock));

	status = rtl_array_list_set(&file_desc_info_table, 0, p_proc0_info, NULL);

	if(status != ESUCCESS) {
		excpt_panic(status, "Failed to initialize file_desc_info_table.");
	}

	//Create driver object of process 0.
	p_drv = vfs_create_drv_object("kernel");

	if(!OPERATE_SUCCESS) {
		excpt_panic(status, "Failed to create kernel driver object.");
	}

	p_drv->process_id = 0;

	vfs_reg_driver(p_drv);

	if(!OPERATE_SUCCESS) {
		excpt_panic(status, "Failed to regist kernel driver object.");
	}

	p_proc0_info->driver_obj = p_drv->driver_id;

	//Initialize ramdisk and tarfs
	ramdisk_init();
	tarfs_init();

	//Mount ramdisk as root filesytem
}

//Volumes
k_status		vfs_mount(char* src, char* target,
                          char* fs_type, u32 flags,
                          char* args);
k_status		vfs_umount(char* path);

//Path
k_status		vfs_chroot(char* path);
k_status		vfs_chdir(char* path);

//File descriptors
k_status vfs_fork(u32 dest_process)
{
	pvfs_proc_info p_src_info, p_dest_info;
	u32 i;
	pfile_desc_t p_src_fd, p_dest_fd;

	p_src_info = get_proc_fs_info();

	//Allocate memory for new info
	p_dest_info = mm_hp_alloc(sizeof(vfs_proc_info), NULL);

	if(p_dest_info == NULL) {
		pm_set_errno(ENOMEM);
		return ENOMEM;
	}

	rtl_memcpy(p_dest_info, p_src_info, sizeof(vfs_proc_info));
	pm_init_mutex(&(p_dest_info->lock));

	pm_set_errno(ESUCCESS);

	pm_acqr_mutex(&(p_src_info->lock), TIMEOUT_BLOCK);
	rtl_array_list_init(&(p_dest_info->file_descs),
	                    p_src_info->file_descs.size,
	                    NULL);

	for(i = 0;
	    OPERATE_SUCCESS;
	    i = rtl_array_list_get_next_index(&(p_src_info->file_descs), i + 1)) {

		//Get source file descriptor
		p_src_fd = rtl_array_list_get(&(p_src_info->file_descs), i);
		ASSERT(p_src_fd != NULL);

		//Duplicate file descriptor
		p_dest_fd = mm_hp_alloc(sizeof(file_desc_t), NULL);
		ASSERT(p_dest_fd != NULL);
		rtl_memcpy(p_dest_fd, p_src_fd, sizeof(file_desc_t));
		vfs_inc_obj_reference((pkobject_t)(p_dest_fd->file_obj));

		rtl_array_list_set(&(p_dest_info->file_descs), i, p_dest_fd, NULL);
		ASSERT(OPERATE_SUCCESS);
	}

	pm_rls_mutex(&(p_src_info->lock));

	//Add new info
	set_proc_fs_info(dest_process, p_dest_info);

	return ESUCCESS;
}

void vfs_clean(u32 process_id)
{
	pvfs_proc_info p_info;

	//Get info
	p_info = get_proc_fs_info(process_id);

	if(p_info == NULL) {
		pm_rls_mutex(&file_desc_info_table_lock);
		pm_set_errno(EFAULT);
		return;
	}

	pm_acqr_mutex(&file_desc_info_table_lock, TIMEOUT_BLOCK);

	rtl_array_list_release(&file_desc_info_table,
	                       process_id,
	                       NULL);
	pm_rls_mutex(&file_desc_info_table_lock);

	//Release info
	rtl_array_list_destroy(&(p_info->file_descs),
	                       (item_destroyer_callback)file_desc_destroy_callback,
	                       NULL,
	                       NULL);
	pm_set_errno(ESUCCESS);
	return;
}

//Files
u32				vfs_open(char* path, u32 flags, u32 mode);
k_status		vfs_chmod(u32 fd, u32 mode);
bool			vfs_access(char* path, u32 mode);
bool			vfs_close(u32 fd);
size_t			vfs_read(u32 fd, void* buf, size_t count);
size_t			vfs_write(u32 fd, void* buf, size_t count);
void			vfs_sync();
bool			vfs_syncfs(u32 volume_dev);
//s32			vfs_ioctl(u32 fd, u32 request, ...);

//File object
u32				vfs_create_file_object(u32 driver);

k_status vfs_send_file_message(u32 src_driver,
                               u32 dest_file,
                               pmsg_t p_msg,
                               u32* p_result)
{
	pdriver_obj_t p_src_drv;
	pfile_obj_t p_dest_file;

	p_src_drv = get_driver(src_driver);

	if(!OPERATE_SUCCESS) {
		return pm_get_errno();
	}

	p_dest_file = get_file_obj(dest_file);

	if(!OPERATE_SUCCESS) {
		return pm_get_errno();
	}

	p_msg->file_id = dest_file;
	p_msg->src_thread = pm_get_crrnt_thrd_id();
	p_msg->result_queue = p_src_drv->msg_queue;

	return msg_send(p_msg,
	                p_dest_file->p_driver->msg_queue,
	                p_result);
}

k_status add_file_obj(pfile_obj_t p_file_obj)
{
	u32 index;
	k_status status;

	pm_acqr_mutex(&(file_obj_table_lock), TIMEOUT_BLOCK);

	index = rtl_array_list_get_free_index(&file_obj_table);

	if(!OPERATE_SUCCESS) {
		status = pm_get_errno();
		pm_rls_mutex(&(file_obj_table_lock));
		pm_set_errno(status);
		return status;
	}

	rtl_array_list_set(&file_obj_table, index, p_file_obj, NULL);

	if(!OPERATE_SUCCESS) {
		status = pm_get_errno();
		pm_rls_mutex(&(file_obj_table_lock));
		pm_set_errno(status);
		return status;
	}

	p_file_obj->file_id = index;

	pm_rls_mutex(&(file_obj_table_lock));

	pm_set_errno(ESUCCESS);
	return ESUCCESS;
}

void remove_file_obj(pfile_obj_t p_file_obj)
{
	//Release file id
	vfs_inc_obj_reference((pkobject_t)p_file_obj);

	pm_acqr_mutex(&file_obj_table_lock, TIMEOUT_BLOCK);

	rtl_array_list_release(&file_obj_table, p_file_obj->file_id, NULL);

	pm_rls_mutex(&file_obj_table_lock);

	pm_acqr_mutex(&(p_file_obj->refered_proc_list_lock), TIMEOUT_BLOCK);

	rtl_list_destroy(&(p_file_obj->refered_proc_list),
	                 NULL,
	                 (item_destroyer_callback)ref_proc_destroy_callback,
	                 p_file_obj);

	pm_rls_mutex(&(p_file_obj->refered_proc_list_lock));

	vfs_dec_obj_reference((pkobject_t)p_file_obj);
	return;
}

pvfs_proc_info get_proc_fs_info()
{
	pvfs_proc_info ret;

	pm_acqr_mutex(&file_desc_info_table_lock, TIMEOUT_BLOCK);

	ret = rtl_array_list_get(&file_desc_info_table, pm_get_crrnt_process());

	if(ret == NULL) {
		excpt_panic(EFAULT,
		            "Current process doesn't have a file descriptor table.It seems impossible but it happend.It's properly because of buffer overflow.");
	}

	pm_rls_mutex(&file_desc_info_table_lock);
	return ret;
}

void set_proc_fs_info(u32 process_id,
                      pvfs_proc_info p_new_info)
{
	pm_acqr_mutex(&file_desc_info_table_lock, TIMEOUT_BLOCK);

	ASSERT((rtl_array_list_get(&file_desc_info_table, process_id),
	        !OPERATE_SUCCESS));

	rtl_array_list_set(&file_desc_info_table, process_id, p_new_info, NULL);

	pm_rls_mutex(&file_desc_info_table_lock);

	return;
}

void set_drv_obj(u32 driver_id)
{
	pvfs_proc_info p_info;

	p_info = get_proc_fs_info();

	p_info->driver_obj = driver_id;

	return;
}

bool has_drv_object()
{
	pvfs_proc_info p_info;

	p_info = get_proc_fs_info();

	if(p_info->driver_obj != INVALID_DRV) {
		return false;
	}

	return true;
}

void ref_proc_destroy_callback(pfile_obj_ref_t p_item,
                               pfile_obj_t p_file_object)
{
	pvfs_proc_info p_info;
	pfile_desc_t p_fd;

	//Release file descriptor
	pm_acqr_mutex(&file_desc_info_table_lock, TIMEOUT_BLOCK);
	p_info = rtl_array_list_get(&file_desc_info_table, p_item->process_id);

	if(p_info == NULL) {
		excpt_panic(EFAULT,
		            "VFS data structure broken!");
	}

	pm_rls_mutex(&file_desc_info_table_lock);

	pm_acqr_mutex(&(p_info->lock), TIMEOUT_BLOCK);

	p_fd = rtl_array_list_get(&(p_info->file_descs), p_item->fd);

	if(p_fd == NULL) {
		excpt_panic(EFAULT,
		            "VFS data structure broken!");
	}

	mm_hp_free(p_fd, NULL);

	rtl_array_list_release(&(p_info->file_descs), p_item->fd, NULL);

	pm_rls_mutex(&(p_info->lock));

	mm_hp_free(p_item, NULL);

	//Decrease reference counnt
	vfs_dec_obj_reference((pkobject_t)p_file_object);

	return;
}

pfile_obj_t get_file_obj(u32 id)
{
	pfile_obj_t ret;
	k_status status;

	pm_acqr_mutex(&file_obj_table_lock, TIMEOUT_BLOCK);

	ret = rtl_array_list_get(&file_desc_info_table, id);
	status = pm_get_errno();

	pm_rls_mutex(&file_obj_table_lock);

	pm_set_errno(status);

	return ret;
}

void file_desc_destroy_callback(pfile_desc_t p_fd,
                                void* p_null)
{
	pkobject_t p_file_obj;

	p_file_obj = (pkobject_t)get_file_obj(p_fd->file_obj);
	vfs_dec_obj_reference(p_file_obj);
	mm_hp_free(p_fd, NULL);

	UNREFERRED_PARAMETER(p_null);
	return;
}