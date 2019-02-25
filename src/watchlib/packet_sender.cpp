#include <stdexcept>
#include <sys/socket.h>
#include "packet_sender.h"

using namespace watches;

packet_sender::packet_sender(){ }
packet_sender::~packet_sender(){
	for(sptr<IO::socket> &sock:listeners){
		s_op.close(sock);
	}
	listeners.clear();
}
void packet_sender::throw_ex(const std::string &header){
	std::string mes = std::string("Packet_sender: ")+
		header+(":\n\t") + s_op.get_errmes();
	throw std::runtime_error(mes);
}
void packet_sender::connect(const std::string &path){
	for(const sptr<IO::socket> &sock:listeners){
		if(sock->get_path() == path){
			throw_ex(std::string("Socket ")+path+(" allready connected"));
		}
	}
	sptr<IO::socket> sock = s_op.create(path, AF_UNIX, SOCK_STREAM);
	if(!sock){
		throw_ex("Can't create AF_UNIX socket");
	}
	if(!s_op.connect(sock)){
		throw_ex(std::string("Can't connect to ")+path);
	}
	listeners.emplace_back(sock);
}
void packet_sender::send(const std::string &path, const packet p){
	for(const sptr<IO::socket> &sock:listeners){
		if(sock->get_path() == path){
			s_op.send(sock, p.serialize());
			return;
		}
	}
	throw_ex(std::string("Socket ")+path+(" is not connected"));
}
void packet_sender::disconnect(const std::string &path){
	auto it = listeners.begin();
	while(it != listeners.end()){
		if((*it)->get_path() == path){
			s_op.close(*it);
			it = listeners.erase(it);
			return;
		}
		it++;
	}
	throw_ex(std::string("Socket ")+path+(" is not connected"));
}
