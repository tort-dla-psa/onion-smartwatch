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
	print_dbg("created",0);
}
void packet_listener_client::print_dbg(const std::string &info, int verb){
	if(dbg) dbg->output(std::string("packet_listener_client[")+
			std::to_string(id)+"]", info, verb);
}
void packet_listener_client::process_packets(){
	while(connected){
		print_dbg("recieving packet",0);
		std::string data = "";
		try{
			data_protocol::recv(s_op, sock, data);
		}catch(...){
			print_dbg("can't recieve data on socket",0);
			break;
		}
		print_dbg(std::string("got packet:")+data,0);
		got_packets_flag = true;
		mt.lock();
		querry.emplace_back(data);
		mt.unlock();
	}
	print_dbg("exit recieving thread loop",0);
}
packet_listener_client::~packet_listener_client(){
	disconnect();
	if(thr->joinable()){
		print_dbg("joining thread",0);
		thr->join();
	}
}
void packet_listener_client::throw_ex(const std::string &header){
	std::string mes = std::string("Packet_listener_client: ")+ header;
	throw std::runtime_error(mes);
}
int packet_listener_client::get_id()const{
	return id;
}
bool packet_listener_client::got_packets(){
	return got_packets_flag;
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
	connected = false;
	print_dbg("disconnecting",0);
	socket_op s_op;
	mt.lock();
	s_op.close(sock);
	//s_op.shutdown(sock);
	mt.unlock();
}
bool packet_listener_client::get_connected(){
	return connected;
}

//============packet_listener==============//
packet_listener::packet_listener(const std::string &path, const int max_clients,
		const int proc_sleep)
{
	quit_requested = false;
	this->proc_sleep = proc_sleep;
	sock = nullptr;
	dbg = nullptr;
	accept_thread = process_thread = nullptr;
#ifdef DEBUG
	dbg = debugger::get_instance();
#endif
	print_dbg(std::string("creating socket at:")+path, 0);
	sock = s_op.create(path, AF_UNIX, SOCK_STREAM);
	s_op.bind(sock);
	s_op.listen(sock, max_clients);
	print_dbg(std::string("creating socket at:")+path+" done", 0);
}
packet_listener::~packet_listener(){
	print_dbg(std::string("starting destruction:"), 0);
	if(!quit_requested){
		print_dbg(std::string("I was not stopper, stopping now"), 0);
		stop();
	}

	print_dbg(std::string("removing socket file"), 0);
	file_op f_op;
	f_op.remove(sock);
	//throw_ex(std::string("Can't close socket at ")+sock->get_path());
	//throw_ex(std::string("Can't unlink socket at ")+sock->get_path());
	mt.unlock();
}
void packet_listener::print_dbg(const std::string &info, int verb){
	if(dbg) dbg->output("packet_listener", info, verb);
}
void packet_listener::throw_ex(const std::string &header){
	std::string mes = std::string("packet_listener: ")+ header;
	throw std::runtime_error(mes);
}
void packet_listener::add_callback(API_CALL code, func f){
	mp[code] = f;
	//TODO: retie if callback allready assigned
}
void packet_listener::accept_func(){
	print_dbg("starting accepting clients", 0);
	int id = -1;
	while(!quit_requested){
		sptr<IO::socket> s = nullptr; 
		try{
			s = s_op.accept(sock);
		}catch(const std::runtime_error &e){
			print_dbg("invalid argument exception on accepting thread, finishing it", 0);
			break;
		}
		if(clients.size() >= max_clients){
			print_dbg("can't accept more clients",0);
			s_op.close(s);
			continue;
		}
		id++;
		print_dbg(std::string("got connection, ID=") + std::to_string(id) ,0);
		mt.lock();
		clients.emplace_back(new packet_listener_client(s, id));
		mt.unlock();
	}
	print_dbg("finishing accepting clients", 0);
}
void packet_listener::process_func(){
	while(!quit_requested){
		//sleep for CPU saving
		std::this_thread::sleep_for(std::chrono::milliseconds(proc_sleep));
		std::vector<packet> querry;
		mt.lock();
		auto it = clients.begin();
		while(it != clients.end()){
			sptr<packet_listener_client> cli = (*it);
			if(cli->got_packets()){
				const std::vector<packet> packets = cli->get_querry();
				querry.insert(querry.end(), packets.begin(), packets.end());
			}
			if(!cli->get_connected()){
				print_dbg(std::string("destroing client")+
					std::to_string((*it)->get_id()),0);
				it = clients.erase(it);
			}
			it++;
		}
		for(const packet &p:querry){
			std::map<API_CALL, func>::iterator it = 
				mp.find(p.get_val());
			if(it != mp.end()){
				(it->second)(p);
			}
		}
		mt.unlock();
	}
}
void packet_listener::start(){
	print_dbg(std::string("starting"), 0);
	accept_thread = std::make_shared<std::thread>
		(&packet_listener::accept_func, this);
	process_thread = std::make_shared<std::thread>
		(&packet_listener::process_func, this);
}
void packet_listener::stop(){
	quit_requested = true;
	print_dbg(std::string("stopping"), 0);

	mt.lock();
	print_dbg(std::string("shutting socket down"), 0);
	s_op.shutdown(sock);
	mt.unlock();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	if(accept_thread->joinable()){
		accept_thread->join();
	}
	if(process_thread->joinable()){
		process_thread->join();
	}

	print_dbg(std::string("disconnecting clients"), 0);
	mt.lock();
	for(sptr<packet_listener_client> &cl:clients){
		print_dbg(std::string("forcing disconnection of client:") +
			std::to_string(cl->get_id()), 0);
		cl->disconnect();
	}
	clients.clear();
	print_dbg(std::string("closing socket"), 0);
	s_op.close(sock);
	mt.unlock();
	print_dbg("stopped", 0);
}
int packet_listener::get_processing_sleep()const{
	return proc_sleep;
}
void packet_listener::change_processing_sleep(const int delta){
	proc_sleep += delta;
}
