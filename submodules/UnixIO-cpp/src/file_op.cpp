#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "file_op.h"

using namespace IO;

file_op::file_op(){}
file_op::~file_op(){}

bool file_op::check(sptr<file> f){
	return check(f->get_path());
}

bool file_op::check(const std::string & path){
	bool status = (access( path.c_str(), F_OK ) != -1 );
	if(!status) this->errno_bak = errno;
	return status;
}

sptr<file> file_op::create(const std::string & path, int flags, int perms){
	int fd = ::open(path.c_str(), flags | O_CREAT, perms);
	if(fd == -1){
		this->errno_bak = errno;
		return nullptr;
	}
	return std::make_shared<file>(fd, path);
}

sptr<file> file_op::open(const std::string & path, int flags){
	int fd = ::open(path.c_str(), flags);
	if(fd == -1){
		this->errno_bak = errno;
		return nullptr;
	}
	return std::make_shared<file>(fd, path);
}

bool file_op::close(sptr<file> f){
	if(!f)
		return false;
	bool status = (::close(f->get_fd()) == 0);
	if(!status)
		this->errno_bak = errno;
	else
		f->set_fd(-1);
	return status;
}

bool file_op::remove(sptr<file> f){
	if(!f)
		return false;
	bool status = (unlink(f->get_path().c_str()) == 0);
	if(!status)
		this->errno_bak = errno;
	return status;
}

bool file_op::rename(sptr<file> f, const std::string & name){
	if(!f)
		return false;
	//TODO:solve common root problem
	bool status = (::rename(f->get_path().c_str(), name.c_str()) == 0);
	if(!status)
		this->errno_bak = errno;
	return status;
}

bool file_op::move(sptr<file> f, const std::string & path){
	return rename(f, path);
}

bool file_op::copy(sptr<file> f, const std::string & path){
	if(!f)
		return false;
	sptr<file> dst;
	char buf[4096];
	ssize_t nread;

	int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd == -1)
		goto out_error;
	dst = std::make_shared<file>(fd, path);

	{
		std::string data = read(f);
		if(data == "@ERROR@1337@")
			goto out_error;
		if(!write(dst, data)){
			goto out_error;
		}
	}

out_error:
	this->errno_bak = errno;
	if (dst){
		close(dst);
	}
	return false;
}

bool file_op::clear(sptr<file> f){
	if(!f)
		return false;
	bool status = (truncate(f->get_path().c_str(), 0) == 0);
	if(!status)
		this->errno_bak = errno;
	return status;
}

bool file_op::write(sptr<file> f, const std::string & data){
	if(!f)
		return false;
	bool status = (::write(f->get_fd(), &data[0], data.size()) != -1);
	if(!status)
		this->errno_bak = errno;
	return status;
}

std::string file_op::read(sptr<file> f){
	if(!f)
		return nullptr;
	int nread = 0, bufsize = 4096, seek=0;
	std::string data;
	data.reserve(bufsize);
	char* buffer = new char[bufsize];

	while (nread = ::read(f->get_fd(), buffer, bufsize), nread > 0){
		const int datasize = data.size();
		if(seek + nread >= datasize){//reaallocate data pointer
			if(seek + nread < datasize*2){
				data.resize(data.size()*2);
			}else{
				data.resize(seek + nread);
			}
		}
		for(int i = 0; i < nread; i++){
			data[seek+i] = buffer[i];
		}
		seek += nread;
	}
	delete[] buffer;
	//truncate
	data.shrink_to_fit();
	return data;
}

int file_op::get_errno()const{
	return this->errno_bak;
}

std::string file_op::get_errmes()const{
	return std::string(strerror(this->errno_bak));
}
