#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include "file.h"
#include "socket_op.h"

using namespace IO;

socket_op::socket_op(){}
socket_op::~socket_op(){}

bool socket_op::check_error(int code){
	if(code == -1){
		this->errno_bak = errno;
		return false;
	}
	return true;
}

sptr<IO::socket> socket_op::create(const std::string & path,
	int domain,
	int type)
{
	return create(path, domain, type, 0);
}

sptr<IO::socket> socket_op::create(const std::string & path,
	int domain,
	int type,
	int protocol)
{
	int fd = ::socket(domain, type, protocol);
	if(!check_error(fd)){ return nullptr; }
	sptr<IO::socket> f(new IO::socket(path));
	f->set_fd(fd);
	return f;
}

sptr<IO::socket> socket_op::create(int domain,
	int type,
	int protocol)
{
	int fd = ::socket(domain, type, protocol);
	if(!check_error(fd)){ return nullptr; }
	sptr<IO::socket> f(new IO::socket());
	f->set_fd(fd);
	return f;
}

bool socket_op::bind(const sptr<IO::socket> &f){
	if(!f){ return false; }
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, f->get_path().c_str(), sizeof(addr.sun_path) - 1);
	int code = ::bind(f->get_fd(), (struct sockaddr*)&addr, sizeof(addr));
	return check_error(code);
}

bool socket_op::bind(const sptr<IO::socket> &f, int port){
	if(!f){ return false; }
	struct sockaddr_in addr;
	addr.sin_family = AF_INET; 
	addr.sin_addr.s_addr = INADDR_ANY; 
	addr.sin_port = htons(port);
	int code = ::bind(f->get_fd(), (struct sockaddr *)&addr, sizeof(addr));
	return check_error(code);
}

bool socket_op::listen(const sptr<IO::socket> &f){
	return listen(f, 1);
}

bool socket_op::listen(const sptr<IO::socket> &f, const int max_clients){
	if(!f){ return false; }
	int code = ::listen(f->get_fd(), max_clients);
	return check_error(code);
}

sptr<IO::socket> socket_op::accept(const sptr<IO::socket> &f){
	if(!f){ return nullptr; }
	//struct sockaddr_un clientname;
	//socklen_t size = sizeof(clientname);

	int fd = ::accept(f->get_fd(), 0,0);//(struct sockaddr *) &clientname, &size);
	if(!check_error(fd)){ return nullptr; }
	sptr<IO::socket> cl(new IO::socket());//clientname.sun_path));
	cl->set_fd(fd);
	return cl;
}

bool socket_op::connect(const sptr<IO::socket> &f){
	if(!f){ return false; }
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, f->get_path().c_str(), sizeof(addr.sun_path)-1);
	int code = ::connect(f->get_fd(), (struct sockaddr*)&addr, sizeof(addr));
	return check_error(code);
}

bool socket_op::connect(const sptr<IO::socket> &f, const std::string &ip, int port){
	if(!f){ return false; }
	struct sockaddr_in addr;
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port);
	if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0){ 
		this->errno_bak = errno;
		return false;
	}
	int code = ::connect(f->get_fd(), (struct sockaddr *)&addr, sizeof(addr));
	return check_error(code);
}

bool socket_op::close(sptr<IO::socket> f){
	if(!f){ return false; }
	int code = ::close(f->get_fd());
	if(check_error(code)){
		f->set_fd(-1);
		return true;
	}
	return false;
}

bool socket_op::recv(const sptr<IO::socket> &f, std::string &str, int size){
	return recv(f, str, size, 0);
}

bool socket_op::recv(const sptr<IO::socket> &f, std::string &str, int size, int flags){
	if(!f){ return false; }
	str.clear();
	char buffer[size];
	int nread = ::recv(f->get_fd(), &buffer, size, flags);
	str = buffer;
	return check_error(nread);
}

bool socket_op::recv(const sptr<IO::socket> &f, int &number){
	return recv(f,number,0);
}
bool socket_op::recv(const sptr<IO::socket> &f, int &number, int flags){
	if(!f){ return false; }
	int nread = ::recv(f->get_fd(), &number, sizeof(int), flags);
	return check_error(nread);
}
bool socket_op::read(const sptr<IO::socket> &f, std::string &str, int size){
	if(!f){ return false; }
	str.clear();
	str.reserve(size);
	int nread = ::read(f->get_fd(), &str[0], size);
	str.shrink_to_fit();
	return check_error(nread);
}

bool socket_op::read(const sptr<IO::socket> &f, int &number){
	if(!f){ return false; }
	int nread = ::read(f->get_fd(), &number, sizeof(int));
	return check_error(nread);
}
bool socket_op::send(const sptr<IO::socket> &f, const std::string &data){
	if(!f){ return false; }
	int fd = f->get_fd();
	int status = ::send(fd,
			data.data(),
			data.length(),
			0);
	return check_error(status);
}

bool socket_op::write(const sptr<IO::socket> &f, const std::string &data){
	if(!f){ return false; }
	int status = ::write(f->get_fd(),
			data.data(),
			data.length()+1);
	return check_error(status);
}

int socket_op::get_errno()const{
	return this->errno_bak;
}

std::string socket_op::get_errmes()const{
	return std::string(strerror(this->errno_bak));
}
