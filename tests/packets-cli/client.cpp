#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unistd.h>
#include "packet.h"
#include "packet_sender.h"

using namespace watches;

int main(){
	const std::string path = "socket";
	packet_sender sndr;
	try{
		sndr.connect(path);
	}catch(const std::exception &e){
		std::cout<<e.what();
	}
	std::vector<std::string> args;
	args.emplace_back("arg0");
	args.emplace_back("arg1");
	args.emplace_back("arg2");
	args.emplace_back("arg3");
	packet p(0,0,0,"generic-name",args);
	for(int i=0; i<10; i++){
		sndr.send(path,p);
		usleep(100);
	}
	sndr.disconnect(path);
}
