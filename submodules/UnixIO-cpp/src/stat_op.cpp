#include "stat_op.h"

using namespace IO;

stat_op::stat_op(){
	path = "";
}
stat_op::~stat_op(){}

bool stat_op::set_path(const std::string &path){
	this->path = path;
	struct stat sb;
	return (stat(path.c_str(), &sb) == 0);
}
std::string stat_op::get_path(){	return path; }
struct stat stat_op::get_stat(){	return sb; }

bool stat_op::check_right(int flag){ 	return (sb.st_mode & flag); }
bool stat_op::check_exec_user(){ 	return check_right(S_IXUSR); }
bool stat_op::check_read_user(){	return check_right(S_IRUSR); }
bool stat_op::check_write_user(){	return check_right(S_IWUSR); }
bool stat_op::check_exec_group(){	return check_right(S_IXGRP); }
bool stat_op::check_read_group(){	return check_right(S_IRGRP); }
bool stat_op::check_write_group(){	return check_right(S_IWGRP); }
bool stat_op::check_exec_other(){	return check_right(S_IXOTH); }
bool stat_op::check_read_other(){	return check_right(S_IROTH); }
bool stat_op::check_write_other(){	return check_right(S_IWOTH); }

bool stat_op::is_file(){	return (sb.st_mode & S_IFMT == S_IFREG); }
bool stat_op::is_socket(){	return (sb.st_mode & S_IFMT == S_IFSOCK); }
bool stat_op::is_symlink(){	return (sb.st_mode & S_IFMT == S_IFLNK); }
bool stat_op::is_dir(){	return (sb.st_mode & S_IFMT == S_IFDIR); }
bool stat_op::is_fifo(){	return (sb.st_mode & S_IFMT == S_IFIFO); }
bool stat_op::is_char_dev(){	return (sb.st_mode & S_IFMT == S_IFCHR); }
bool stat_op::is_blk_dev(){	return (sb.st_mode & S_IFMT == S_IFBLK); }

unsigned long stat_op::get_mode(){	return sb.st_mode; }
long stat_op::get_inode(){		return sb.st_ino; }
long stat_op::get_link_count(){		return sb.st_nlink; }
long stat_op::get_major_dev(){		return major(sb.st_dev); }
long stat_op::get_minor_dev(){		return minor(sb.st_dev); }
long stat_op::get_UID(){		return sb.st_uid; }
long stat_op::get_GID(){		return sb.st_gid; }
long stat_op::get_block_size(){		return sb.st_blksize; }
long long stat_op::get_size(){		return sb.st_size; }
long long stat_op::get_blocks(){	return sb.st_blocks; }
time_t stat_op::get_last_change(){	return sb.st_ctime; }
time_t stat_op::get_last_access(){	return sb.st_atime; }
time_t stat_op::get_last_mod(){		return sb.st_mtime; }
