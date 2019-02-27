#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include "file.h"
#include "socket_op.h"
#include "file_op.h"

using namespace IO;

int main(const int argc, char *argv[]){
	const std::string path = "socket";
	socket_op s_op;
	sptr<IO::socket> s;
	std::string mes = "l33t-message\0";
	std::cout<<"CLI:creating\n";
	if(!(s = s_op.create(AF_INET, SOCK_STREAM, 0)))
		goto exit_cli;
	std::cout<<"CLI:connecting\n";
	if(!s_op.connect(s, "127.0.0.1", std::atoi(argv[1])))
		goto exit_cli;
	std::cout<<"CLI:sending\n";
	if(!s_op.send(s, mes))
		goto exit_cli;
	sleep(1);
	std::cout<<"CLI:recieving\n";
	if(!s_op.recv(s, mes, 100))
		goto exit_cli;
	std::cout << "CLI:" << mes << '\n';
exit_cli:
	std::cout<<"CLI:ending\n";
	std::cout << s_op.get_errmes();
	s_op.close(s);
}
