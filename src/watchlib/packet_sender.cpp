#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <sys/socket.h>
#include "packet_sender.h"
#include "data_protocol.h"

using namespace watches;

packet_sender::packet_sender(){ }
packet_sender::~packet_sender(){
	for(sptr<IO::socket> &sock:listeners){
		try{
			s_op.close(sock);
		}catch(std::runtime_error e){
			throw_ex(e.what());
		}
	}
	listeners.clear();
}
void packet_sender::throw_ex(const std::string &header){
	std::string mes = std::string("packet_sender: ")+ header;
	throw std::runtime_error(mes);
}
void packet_sender::connect(const std::string &path){
	const auto it = std::find_if(listeners.begin(), listeners.end(), 
		[&](const auto &sock){ return sock->get_path() == path; });
	if(it != listeners.end()){ //socket is connected
		return;
	}
	try{
		sptr<IO::socket> sock = s_op.create(path, AF_UNIX, SOCK_STREAM);
		s_op.connect(sock);
		listeners.emplace_back(sock);
	}catch(std::runtime_error e){
		throw_ex(std::string("can't connect to ")+path+" "+e.what());
	}
}
void packet_sender::send_by_path(const std::string &path, const packet &p){
#ifdef DEBUG
	std::cout<<"PS:...sending to:"<<path<<",call:"<<p.get_pid()<<",args:\n";
	for(const auto &arg:p.get_args()){
		std::cout<<"PS:arg:"<<arg<<"\n";
	}
#endif
	const auto it = std::find_if(listeners.begin(), listeners.end(), 
		[&](const auto &sock){ return sock->get_path() == path; });
	if(it == listeners.end()){ //socket is not connected
		throw_ex("isn't connected to: "+path);
	}else{
		const auto sock = *it;
		try{
			const auto data = p.serialize();
			data_protocol::send(s_op, sock, data);
		}catch(std::runtime_error e){
			throw_ex(std::string("can't send to ")+path+
				" "+e.what()+", disconnecting");
			disconn_by_path(path);
		}
	}
}
void packet_sender::send_by_name(const std::string &name, const packet &p){
	std::cout<<"PS:sending to:"<<name<<"...\n";
	auto it = name_path_pair.find(name);
	if(it == name_path_pair.end()){ //socket is not connected
		throw_ex("no path is associated with:"+name);
	}else{
		send_by_path(it->second, p);
	}
}
void packet_sender::disconn_by_path(const std::string &path){
	auto it = listeners.begin();
	while(it != listeners.end()){
		if((*it)->get_path() == path){
			try{
				s_op.close(*it);
			}catch(std::runtime_error e){
				//can't close socket
			}
			it = listeners.erase(it);
			return;
		}
		it++;
	}
	throw_ex(std::string("socket ")+path+(" is not connected"));
}
bool packet_sender::is_conn(const sptr<IO::socket> &listener)const{
	for(const auto &sock : listeners){
		if(sock == listener)
			return true;
	}
	return false;
}
bool packet_sender::is_conn_by_path(const std::string &path)const{
	//find app with desired path
	const auto it = std::find_if(listeners.begin(), listeners.end(),
		[&](const auto &sock){ return (sock->get_path() == path); });
	return (it != listeners.end());
}
bool packet_sender::is_conn_by_name(const std::string &name)const{
	//find app with desired name
	const auto it = name_path_pair.find(name);
	return (it != name_path_pair.end());
}
void packet_sender::associate(const std::string &path, const std::string &name){
	name_path_pair[name] = path;
}

