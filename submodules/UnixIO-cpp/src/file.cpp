#include "file.h"

using namespace IO;

unix_file::unix_file(const std::string &path){ this->path = path; }
unix_file::~unix_file(){}
std::string unix_file::get_path()const{ return path; }

file::file(int fd, const std::string &path):unix_file(path),fd(fd){}
file::file(const std::string &path):unix_file(path),fd(-1){}
file::~file(){} 
int file::get_fd()const{ return fd; }
void file::set_fd(const int fd){ this->fd = fd; }

dir::dir(const std::string &path):unix_file(path){}
dir::~dir(){}

symlink::symlink(const std::string &path):file(path){}
symlink::~symlink(){}

socket::socket(const std::string &path):file(path){}
socket::socket():file(""){}
socket::~socket(){}
int socket::get_fd()const{ return file::get_fd(); }
void socket::set_fd(const int fd){ file::set_fd(fd); }

fifo::fifo(int fd, const std::string &path):file(path),fd(fd){}
fifo::fifo(const std::string &path):file(path),fd(-1){}
fifo::~fifo(){}
int fifo::get_fd()const{ return fd; }
void fifo::set_fd(const int fd){ this->fd = fd; }

ch_dev::ch_dev(const std::string &path):unix_file(path){}
ch_dev::~ch_dev(){}

blk_dev::blk_dev(const std::string &path):unix_file(path){}
blk_dev::~blk_dev(){}
