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
	if(dbg) dbg->output("packet_listener_client",
			"created packet_listener_client",
			0);
}
void packet_listener_client::process_packets(){
	while(connected){
		if(dbg) dbg->output("packet_listener_client",
			std::string("ID:")+std::to_string(id)+" recieving packet",
			0);
		std::string data;
		if(!data_protocol::recv(s_op, sock, data)){
			throw_ex(std::string("Can't recieve data on socket"));
		}
		if(dbg) dbg->output("packet_listener_client",
			std::string("ID:")+std::to_string(id)+" got packet "+data,
			0);
		const packet p(data);
		querry.emplace_back(p);
		got_packets_flag = true;
	}
}
packet_listener_client::~packet_listener_client(){
	if(connected){
		disconnect();
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
bool packet_listener_client::got_packets()const{
	return got_packets_flag;
}
std::vector<packet> packet_listener_client::get_querry(){
	//TODO:lock
	std::vector<packet> bak = std::move(querry);
	got_packets_flag = false;
	return bak;
	//TODO:unlock
}
void packet_listener_client::disconnect(){
	connected = false;
	if(dbg) dbg->output("packet_listener_client",
			"disconnecting packet_listener_client",
			0);
	socket_op s_op;
	if(thr->joinable()){
		thr->join();
	}
	s_op.close(sock);
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
	if(dbg) dbg->output("packet_listener",
			std::string("creating socket at:")+path,
			0);
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
	if(dbg) dbg->output("packet_listener",
			std::string("creating socket at:")+path+" done",
			0);
}
packet_listener::~packet_listener(){
	if(dbg) dbg->output("packet_listener",
			std::string("starting destruction:"),
			0);
	mt.lock();
	for(sptr<packet_listener_client> &cl:clients){
		if(dbg) dbg->output("packet_listener",
			std::string("forcing disconnection of client:") +
			std::to_string(cl->get_id()),
			0);
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
	if(dbg) dbg->output("packet_listener","starting accept",0);
	int id = -1;
	while(!quit_requested){
		sptr<IO::socket> s = s_op.accept(sock);
		if(clients.size() >= max_clients){
			if(dbg) dbg->output("packet_listener",
					"can't accept more clients",0);
			continue;
			//TODO:refuse connection
		}
		if(!s){
			throw_ex("Can't accept on socket");
		}
		id++;
		if(dbg) dbg->output("packet_listener",
				std::string("got connection, ID=") + std::to_string(id) ,0);
		mt.lock();
		sptr<packet_listener_client> cli(new packet_listener_client(s, id));
		clients.emplace_back(cli);
		if(!reaction_thread){
			reaction_thread = std::make_shared<std::thread>
				(&packet_listener::reaction_func, this);
		}
		mt.unlock();
	}
	if(reaction_thread->joinable()){
		reaction_thread->join();
	}
	if(dbg) dbg->output("packet_listener","ending accept",0);
}
void packet_listener::reaction_func(){
	while(!quit_requested){
		std::vector<packet> querry;
		for(sptr<packet_listener_client> &cli:clients){
			if(!cli->got_packets()){
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
	}
}
void packet_listener::start(){
	if(dbg) dbg->output("packet_listener",
			std::string("starting"),
			0);
	accept_thread = std::make_shared<std::thread>
		(&packet_listener::accept_client, this);
	while(!quit_requested){
		std::this_thread::sleep_for(std::chrono::seconds(10));
		mt.lock();
		auto it = clients.begin();
		while(it != clients.end()){
			if(!(*it)->get_connected()){
				if(dbg) dbg->output("packet_listener",
					std::string("destroing client")+
					std::to_string((*it)->get_id()),0);
				it = clients.erase(it);
				if(clients.size() == 0){
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
	if(dbg) dbg->output("packet_listener","ending",0);
}
