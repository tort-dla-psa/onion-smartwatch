#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ftw.h>

#include "file.h"
#include "dir_op.h"
#include "file_op.h"

using namespace IO;

dir_op::dir_op(){}
dir_op::~dir_op(){}

bool dir_op::check(const std::string &path){
	struct stat sb;
	if(stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)){
		return true;
	}
	return false;
}

bool dir_op::check(sptr<dir> d){
	return check(d->get_path());
}

sptr<dir> dir_op::create(const std::string &path, int flags){
	bool status = (mkdir(path.c_str(), flags) == 0);
	if(status){
		return std::make_shared<dir>(path);
	}else{
		return nullptr;
	}
}

sptr<dir> dir_op::create(const std::string &path){
	return create(path, 0755);
}

sptr<dir> dir_op::open(const std::string &path){
	if(check(path)){
		return std::make_shared<dir>(path);
	}else{
		return nullptr;
	}
}

namespace copy_tmp{
	std::string dst_root;
	file_op f_op;
}

int copy_file(const char* src_path, const struct stat* sb, int typeflag) {
	std::string dst_path = copy_tmp::dst_root + std::string(src_path);
	switch(typeflag) {
		case FTW_D:
			mkdir(dst_path.c_str(), sb->st_mode);
		break;
		case FTW_F:
			sptr<file> f = copy_tmp::f_op.open(src_path, O_WRONLY);
			copy_tmp::f_op.copy(f, dst_path);
		break;
	}
	return 0;
}

bool dir_op::copy(sptr<dir> src, sptr<dir> dst){
	if(!src)
		return false;
	const int ftw_max_fd = 20; // I don't know appropriate value for this
	copy_tmp::dst_root = dst->get_path();
	ftw(src->get_path().c_str(), copy_file, ftw_max_fd);
}

bool dir_op::move(sptr<dir> src, sptr<dir> dst){
	if(!src || !dst)
		return false;
	if(check(dst)){
		//TODO: fix common root problem;
		const std::string newpath = dst->get_path() + 
			src->get_path();
		return rename(src, newpath);
	}else{
		return rename(src, dst->get_path());
	}
}

bool dir_op::rename(sptr<dir> src, const std::string &dst){
	if(!src)
		return false;
	return ::rename(src->get_path().c_str(), dst.c_str());
}

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
	return remove(fpath);
}

bool dir_op::remove(sptr<dir> d){
	return (nftw(d->get_path().c_str(), unlink_cb, 64, FTW_DEPTH | FTW_PHYS) != -1);
}

std::vector<sptr<unix_file>> dir_op::list(sptr<dir> d){
	std::vector<sptr<unix_file>> vec;
	if(!d){
		return vec;
	}
	DIR *unix_dir;
	struct dirent *dir_st;
	if(!(unix_dir = opendir(d->get_path().c_str()))){
		return vec;
	}
	char** array = new char*[1024];
	int count = 0;
	while ((dir_st = readdir(unix_dir)) != NULL){
		char* name = dir_st->d_name;
		array[count] = name;
		count++;
	}
	closedir(unix_dir);

	vec.reserve(count);
	for(int i=0; i<count; i++){
		vec.emplace_back(new unix_file(std::string(array[i])));
	}
	return vec;
}

int dir_op::count_files(sptr<dir> d){
	if(!d)
		return -1;
	int file_count = 0;
	DIR * dirp;
	struct dirent* entry;

	dirp = opendir(d->get_path().c_str());
	while ((entry = readdir(dirp)) != NULL) {
//		if (entry->d_type == DT_REG) { /* If the entry is a regular file */
			file_count++;
//		}
	}
	closedir(dirp);
	return file_count;
}
