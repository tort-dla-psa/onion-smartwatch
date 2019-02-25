#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "fifo_op.h"

using namespace IO;

fifo_op::fifo_op(){}
fifo_op::~fifo_op(){}

sptr<fifo> fifo_op::create(const std::string & path, int perms){
	bool status = (::mkfifo(path.c_str(), perms) == 0);
	if(!status){
		this->errno_bak = errno;
		return nullptr;
	}
	return std::make_shared<fifo>(path);
}

sptr<fifo> fifo_op::open(const std::string & path, int flags){
	int fd = ::open(path.c_str(), flags);
	if(fd == -1){
		this->errno_bak = errno;
		return nullptr;
	}
	return std::make_shared<fifo>(fd, path);
}

sptr<fifo> fifo_op::open(sptr<fifo> f, int flags){
	if(!f)
		return nullptr;
	int fd = ::open(f->get_path().c_str(), flags);
	if(fd == -1){
		this->errno_bak = errno;
		return nullptr;
	}
	f->set_fd(fd);
	return f;
}

bool fifo_op::close(sptr<fifo> f){
	if(!f)
		return false;
	bool status = (::close(f->get_fd()) == 0);
	if(!status)
		this->errno_bak = errno;
	else
		f->set_fd(-1);
	return status;
}

bool fifo_op::write(sptr<fifo> f, const std::string & data){
	if(!f)
		return false;
	bool status = (::write(f->get_fd(), &data[0], data.size()) != -1);
	if(!status)
		this->errno_bak = errno;
	return status;
}

std::string fifo_op::read(sptr<fifo> f){
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
	data.shrink_to_fit();
	return data;
}

int fifo_op::get_errno()const{
	return this->errno_bak;
}

std::string fifo_op::get_errmes()const{
	return std::string(strerror(this->errno_bak));
}
