#include <chrono>
#include <stdexcept>
#include <sys/socket.h>
#include "packet_listener.h"
#include "file.h"
#include "file_op.h"
#include "socket_op.h"
#include "data_protocol.h"

using namespace watches;
using namespace IO;

packet_listener::packet_listener(const std::string &path, int max_clients){
	sock = nullptr;
	sock = s_op.create(path, AF_UNIX, SOCK_STREAM);
	if(!sock){
		throw_ex(std::string("Can't create socket at ")+path);
	}
	if(!s_op.bind(sock)){
		throw_ex(std::string("Can't bind socket at ")+path);
	}
	if(!s_op.listen(sock, max_clients)){
		throw_ex(std::string("Can't listen on socket at ")+path);
	}
}
packet_listener::~packet_listener(){
	if(sock){
		if(!s_op.close(sock)){
			throw_ex(std::string("Can't close socket at ")+sock->get_path());
		}
	}
}
void packet_listener::throw_ex(const std::string &header){
	std::string mes = std::string("Packet_listener: ")+
		header+(":\n\t") + s_op.get_errmes();
	throw std::runtime_error(mes);
}
void packet_listener::add_callback(const packet p, func f){
	mp[p] = f;
	//TODO: retie if callback allready assigned
}
void packet_listener::accept_func(){
	sptr<IO::socket> s = s_op.accept(sock);
	if(!s){
		throw_ex("Can't accept on socket");
	}
	mt.lock();
	clients.emplace_back(s);
	mt.unlock();
}
void packet_listener::try_accepting(const int time){
	std::thread thr = std::thread([=]{ accept_func(); });
	thr.detach();
	std::this_thread::sleep_for(std::chrono::milliseconds(time));
	thr.std::thread::~thread();
}
void packet_listener::recv_func(sptr<IO::socket> cli){
	std::string data;
	if(data_protocol::recv(s_op, cli, data)){
		mt.lock();
		packet p(data);
		std::map<packet, func>::iterator it = mp.find(p);
		if(it != mp.end()){
			(it->second)(p);
		}
		mt.unlock();
	}
}
void packet_listener::try_recv(sptr<IO::socket> cli, const int time){
	std::thread thr = std::thread([=]{ recv_func(cli); });
	thr.detach();
	std::this_thread::sleep_for(std::chrono::milliseconds(time));
	thr.std::thread::~thread();
}
void packet_listener::spin(const int recv_time){
	for(sptr<IO::socket> &cl:clients){
		try_recv(cl, recv_time);
	}
}
