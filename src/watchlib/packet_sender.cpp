#include <stdexcept>
#include <sys/socket.h>
#include "packet_sender.h"
#include "data_protocol.h"

using namespace watches;

packet_sender::packet_sender(){
	dbg = nullptr;
#ifdef DEBUG
	dbg = debugger::get_instance();
#endif
	print_dbg("created", 0);
}
packet_sender::~packet_sender(){
	print_dbg("disconnecting and destructing", 0);
	for(sptr<IO::socket> &sock:listeners){
		s_op.close(sock);
	}
	listeners.clear();
}
void packet_sender::print_dbg(const std::string &info, int verb){
	if(dbg) dbg->output("packet_sender", info, verb);
}
void packet_sender::throw_ex(const std::string &header){
	std::string mes = std::string("Packet_sender: ")+
		header+(":\n\t") + s_op.get_errmes();
	throw std::runtime_error(mes);
}
void packet_sender::connect(const std::string &path){
	print_dbg(std::string("connecting to:")+path, 0);
	for(const sptr<IO::socket> &sock:listeners){
		if(sock->get_path() == path)
			throw_ex(std::string("Socket ")+path+(" allready connected"));
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
	const std::string data = p.serialize();
	print_dbg(std::string("sending packet:")+data+" to:"+path, 0);
	for(const sptr<IO::socket> &sock:listeners){
		if(sock->get_path() == path){
			if(!data_protocol::send(s_op, sock, data)){
				throw_ex(std::string("can't send to:")+path+
						", "+s_op.get_errmes());
			}
			return;
		}
	}
	print_dbg(std::string("packet_listener at:")+path+" is not connected", 0);
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
	print_dbg(std::string("packet_listener at:")+path+" is not connected", 0);
	throw_ex(std::string("Socket ")+path+(" is not connected"));
}
