#include <iostream>
#include <stdexcept>
#include <signal.h>
#include "packet.h"
#include "packet_listener.h"

using namespace watches;

void callback(const packet p){
	std::string data = p.serialize();
	std::cout<<"I got:"<<data<<'\n';
}
void sigint_callback(int s){
	std::cout<<"Caught SIGINT\n";
	exit(1);
}
int main(){
	signal(SIGINT, sigint_callback);
	packet_listener l("socket", 1);
	try{
		l.add_callback(0, callback);
	}catch(const std::exception &e){
		std::cout<<e.what();
	}
	l.start();
}
