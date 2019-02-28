#include <chrono>
#include <stdexcept>
#include <algorithm>
#include <sys/socket.h>
#include "packet_listener.h"
#include "file.h"
#include "file_op.h"
#include "socket_op.h"
#include "data_protocol.h"

using namespace watches;
using namespace IO;

packet_listener_client::packet_listener_client(sptr<IO::socket> sock, int id){
	connected = true;
	this->sock = sock;
	thr = nullptr;
	dbg = nullptr;
	this->id = id;
	this->thr = std::make_shared<std::thread>
		(&packet_listener_client::process_packets, this);
#ifdef DEBUG
	dbg = debugger::get_instance();
#endif
	print_dbg("created packet_listener_client",0);
}
void packet_listener_client::print_dbg(const std::string &info, int verb){
	if(dbg) dbg->output(std::string("packet_listener_client[")+
			std::to_string(id)+"]", info, verb);
}
void packet_listener_client::process_packets(){
	while(connected){
		print_dbg("recieving packet",0);
		std::string data = "";
		if(!data_protocol::recv(s_op, sock, data)){
			print_dbg("can't recieve data on socket",2);
			disconnect();
			return;
		}
		print_dbg(std::string("got packet:")+data,0);
		const packet p(data);
		querry.emplace_back(p);
		got_packets_flag = true;
	}
}
packet_listener_client::~packet_listener_client(){
	disconnect();
	if(thr->joinable()){
		print_dbg("joining thread",0);
		thr->join();
	}
}
void packet_listener_client::throw_ex(const std::string &header){
	std::string mes = std::string("Packet_listener_client: ")+
		header+(":\n\t") + s_op.get_errmes();
	throw std::runtime_error(mes);
}
int packet_listener_client::get_id()const{
	return id;
}
bool packet_listener_client::got_packets(){
	mt.lock();
	const bool bak = got_packets_flag;
	mt.unlock();
	return bak;
}
std::vector<packet> packet_listener_client::get_querry(){
	mt.lock();
	std::vector<packet> bak = std::move(querry);
	got_packets_flag = false;
	mt.unlock();
	return bak;
}
void packet_listener_client::disconnect(){
	if(!connected)
		return;
	mt.lock();
	connected = false;
	print_dbg("disconnecting packet_listener_client",0);
	socket_op s_op;
	s_op.close(sock);
	mt.unlock();
}
bool packet_listener_client::get_connected(){
	return connected;
}

//============packet_listener==============//
packet_listener::packet_listener(const std::string &path, int max_clients){
	quit_requested = false;
	sock = nullptr;
	dbg = nullptr;
	accept_thread = reaction_thread = nullptr;
#ifdef DEBUG
	dbg = debugger::get_instance();
#endif
	print_dbg(std::string("creating socket at:")+path, 0);
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
	print_dbg(std::string("creating socket at:")+path+" done", 0);
}
packet_listener::~packet_listener(){
	print_dbg(std::string("starting destruction:"), 0);
	mt.lock();
	for(sptr<packet_listener_client> &cl:clients){
		print_dbg(std::string("forcing disconnection of client:") +
			std::to_string(cl->get_id()), 0);
		cl->disconnect();
	}
	clients.clear();
	if(sock && !s_op.close(sock)){
		throw_ex(std::string("Can't close socket at ")+sock->get_path());
	}
	file_op f_op;
	if(sock && !f_op.remove(sock)){
		throw_ex(std::string("Can't unlink socket at ")+sock->get_path());
	}
	mt.unlock();
}
void packet_listener::print_dbg(const std::string &info, int verb){
	if(dbg) dbg->output("packet_listener", info, verb);
}
void packet_listener::throw_ex(const std::string &header){
	std::string mes = std::string("Packet_listener: ")+
		header+(":\n\t") + s_op.get_errmes();
	throw std::runtime_error(mes);
}
void packet_listener::add_callback(int code, func f){
	mp[code] = f;
	//TODO: retie if callback allready assigned
}
void packet_listener::accept_client(){
	print_dbg("starting accepting clients", 0);
	int id = -1;
	while(!quit_requested){
		sptr<IO::socket> s = s_op.accept(sock);
		if(clients.size() >= max_clients){
			print_dbg("can't accept more clients",0);
			continue;
			//TODO:refuse connection
		}
		if(!s){
			throw_ex("Can't accept on socket");
		}
		id++;
		print_dbg(std::string("got connection, ID=") + std::to_string(id) ,0);
		mt.lock();
		sptr<packet_listener_client> cli(new packet_listener_client(s, id));
		clients.emplace_back(cli);
		if(!reaction_thread){
			reaction_thread = std::make_shared<std::thread>
				(&packet_listener::reaction_func, this);
		}
		mt.unlock();
	}
	print_dbg("finishing accepting clients", 0);
	if(reaction_thread->joinable()){
		reaction_thread->join();
	}
}
void packet_listener::reaction_func(){
	while(!quit_requested){
		std::vector<packet> querry;
		for(sptr<packet_listener_client> &cli:clients){
			if(!cli->got_packets()){
				continue;
			}
			std::vector<packet> packets = cli->get_querry();
			querry.insert(querry.end(), packets.begin(), packets.end());
		}
		for(const packet &p:querry){
			std::map<int, func>::iterator it = mp.find(p.get_val());
			if(it != mp.end()){
				(it->second)(p);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
void packet_listener::start(){
	print_dbg(std::string("starting main loop"), 0);
	accept_thread = std::make_shared<std::thread>
		(&packet_listener::accept_client, this);
	while(!quit_requested){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		mt.lock();
		auto it = clients.begin();
		while(it != clients.end()){
			if(!(*it)->get_connected()){
				print_dbg(std::string("destroing client")+
					std::to_string((*it)->get_id()),0);
				it = clients.erase(it);
				if(clients.size() == 0){
					print_dbg("no clients, requesting finish", 0);
					quit_requested = true;
				}
			}
			else ++it;
		}
		mt.unlock();
	}
	if(accept_thread->joinable()){
		accept_thread->join();
	}
	print_dbg("finishing main loop", 0);
}
