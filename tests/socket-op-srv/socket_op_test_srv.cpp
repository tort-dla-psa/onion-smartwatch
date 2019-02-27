#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "file.h"
#include "socket_op.h"
#include "file_op.h"

using namespace IO;

int main(const int argc, char *argv[]){
	const std::string path = "socket";
	socket_op s_op;
	file_op f_op;
	sptr<IO::socket> s, cl;
	std::string mes = "some data\0";
	std::cout<<"SRV:creating\n";
	//if(!(s = s_op.create(path, AF_UNIX, SOCK_STREAM)))
	if(!(s = s_op.create(AF_INET, SOCK_STREAM, 0)))
		goto exit_srv;
	std::cout<<"SRV:binding\n";
	if(!s_op.bind(s, std::atoi(argv[1])))
		goto exit_srv;
	std::cout<<"SRV:listening\n";
	if(!s_op.listen(s, 5))
		goto exit_srv;
	std::cout<<"SRV:accepting\n";
	if(!(cl = s_op.accept(s)))
		goto exit_srv;
	std::cout<<"SRV:recieving\n";
	if(!s_op.recv(cl, mes, 100))
		goto exit_srv;
	std::cout << "SRV:" << mes << '\n';
	std::cout<<"SRV:sending\n";
	if(!s_op.send(cl,mes+"_2"))
		goto exit_srv;
exit_srv:
	std::cout<<"SRV:ending\n";
	std::cout << s_op.get_errmes();
	s_op.close(cl);
	f_op.remove(std::static_pointer_cast<file>(s));
}
