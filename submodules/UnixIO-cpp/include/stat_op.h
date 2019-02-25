#ifndef stat_op_h
#define stat_op_h

#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

namespace IO{

class stat_op{
	std::string path;
	struct stat sb;
public:
	stat_op();
	~stat_op();

	bool set_path(const std::string &path);
	std::string get_path();
	struct stat get_stat();

	bool check_right(int flag);
	bool check_exec_user();
	bool check_read_user();
	bool check_write_user();
	bool check_exec_group();
	bool check_read_group();
	bool check_write_group();
	bool check_exec_other();
	bool check_read_other();
	bool check_write_other();
	
	bool is_file();
	bool is_socket();
	bool is_symlink();
	bool is_dir();
	bool is_fifo();
	bool is_char_dev();
	bool is_blk_dev();

	unsigned long get_mode();
	long get_inode();
	long get_link_count();
	long get_major_dev();
	long get_minor_dev();
	long get_UID();
	long get_GID();
	long get_block_size();
	long long get_size();
	long long get_blocks();
	time_t get_last_change();
	time_t get_last_access();
	time_t get_last_mod();
};
}

#endif
