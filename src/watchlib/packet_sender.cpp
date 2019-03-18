#include <stdexcept>
#include <sys/socket.h>
#include "packet_sender.h"
#include "data_protocol.h"

using namespace watches;

packet_sender::packet_sender(){
	dbg = nullptr;
#ifdef DEBUG
	dbg = logger::get_instance();
#endif
	print_dbg("created", 0);
}
packet_sender::~packet_sender(){
	print_dbg("disconnecting and destructing", 0);
	for(sptr<IO::socket> &sock:listeners){
		print_dbg(std::string("disconnecting from ")+sock->get_path(), 0);
		s_op.close(sock);
	}
	listeners.clear();
}
void packet_sender::print_dbg(const std::string &info, int verb){
	if(dbg) dbg->output("packet_sender", info, verb);
}
void packet_sender::throw_ex(const std::string &header){
	std::string mes = std::string("packet_sender: ")+ header;
	throw std::runtime_error(mes);
}
void packet_sender::connect(const std::string &path){
	print_dbg(std::string("connecting to:")+path, 0);
	for(const sptr<IO::socket> &sock:listeners){
		if(sock->get_path() == path)
			throw_ex(std::string("socket at:")+path+(" allready connected"));
	}
	sptr<IO::socket> sock = s_op.create(path, AF_UNIX, SOCK_STREAM);
	s_op.connect(sock);
	listeners.emplace_back(sock);
}
void packet_sender::send(const std::string &path, const packet p){
	const std::string data = p.serialize();
	print_dbg(std::string("sending packet:")+data+" to:"+path, 0);
	for(const sptr<IO::socket> &sock:listeners){
		if(sock->get_path() == path){
			data_protocol::send(s_op, sock, data);
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
bool packet_sender::is_connected(const sptr<IO::socket> &listener)const{
	for(const sptr<IO::socket> &sock:listeners){
		if(sock == listener)
			return true;
	}
	return false;
}
bool packet_sender::is_connected(const std::string &path)const{
	for(const sptr<IO::socket> &sock:listeners){
		if(sock->get_path() == path)
			return true;
	}
	return false;
}
