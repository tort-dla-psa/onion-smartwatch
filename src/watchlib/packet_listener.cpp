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
	this->id = id;
	this->thr = std::make_shared<std::thread>
		(&packet_listener_client::process_packets, this);
}
void packet_listener_client::process_packets(){
	while(connected){
		std::string data = "";
		try{
			data_protocol::recv(s_op, sock, data);
		}catch(...){
			//can't recieve packet
			break;
		}
		got_packets_flag = true;
		mt.lock();
		queue.emplace_back(data);
		mt.unlock();
	}
	//exit recieving thread loop
}
packet_listener_client::~packet_listener_client(){
	disconnect();
	if(thr->joinable()){
		thr->join();
	}
}
void packet_listener_client::throw_ex(const std::string &header){
	const auto mes = std::string("packet_listener_client: ")+ header;
	throw std::runtime_error(mes);
}
int packet_listener_client::get_id()const{
	return id;
}
bool packet_listener_client::got_packets(){
	return got_packets_flag;
}
std::vector<packet> packet_listener_client::get_queue(){
	mt.lock();
	const auto bak = std::move(queue);
	got_packets_flag = false;
	mt.unlock();
	return bak;
}
void packet_listener_client::disconnect(){
	if(!connected)
		return;
	connected = false;
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
		const int proc_sleep,
		const std::shared_ptr<types::concurrent_queue<packet>> packets_queue)
{
	quit_requested = false;
	this->proc_sleep = proc_sleep;
	this->packets_queue = packets_queue;
	this->max_clients = max_clients;
	sock = nullptr;
	accept_thread = process_thread = nullptr;
	//creating socket
	try{
		sock = s_op.create(path, AF_UNIX, SOCK_STREAM);
		s_op.bind(sock);
		s_op.listen(sock, max_clients);
	}catch(std::runtime_error e){
		throw_ex("can't establish socket at "+path+": "+e.what());
	}
}
packet_listener::~packet_listener(){
	if(!quit_requested){
		//I was not stopper, stopping now
		stop();
	}
}
void packet_listener::throw_ex(const std::string &header){
	std::string mes = std::string("packet_listener: ")+ header;
	throw std::runtime_error(mes);
}
void packet_listener::accept_func(){
	//starting accepting clients
	int id = -1;
	while(!quit_requested){
		sptr<IO::socket> s = nullptr; 
		try{
			s = s_op.accept(sock);
		}catch(const std::runtime_error &e){
			//can't accept
			break;
		}
		if(clients.size() >= max_clients){
			//can't accept more
			//TODO: send deny to client
			//s_op.close(s);
			continue;
		}
		id++;
		mt.lock();
		clients.emplace_back(new packet_listener_client(s, id));
		mt.unlock();
	}
}
void packet_listener::process_func(){
	while(!quit_requested){
		//sleep for CPU saving
		std::this_thread::sleep_for(std::chrono::milliseconds(proc_sleep));
		std::vector<packet> queue;
		//get all packets
		mt.lock();
		auto it = clients.begin();
		while(it != clients.end()){
			auto cli = (*it);
			if(cli->got_packets()){
				auto packets = cli->get_queue();
				queue.reserve(queue.size() + packets.size());
				std::move(packets.begin(), packets.end(),
					std::back_inserter(queue));
			}
			if(!cli->get_connected()){
				//destroing client
				it = clients.erase(it);
				continue;
			}
			it++;
		}
		mt.unlock();
		for(const auto &p:queue){
			packets_queue->push(p);
		}
	}
}
void packet_listener::start(){
	accept_thread = std::make_shared<std::thread>
		(&packet_listener::accept_func, this);
	process_thread = std::make_shared<std::thread>
		(&packet_listener::process_func, this);
}
void packet_listener::stop(){
	quit_requested = true;

	mt.lock();
	try{
		s_op.shutdown(sock);
	}catch(...){
	}
	mt.unlock();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	if(accept_thread->joinable()){
		accept_thread->join();
	}
	if(process_thread->joinable()){
		process_thread->join();
	}

	//disconnecting clients
	mt.lock();
	for(sptr<packet_listener_client> &cl:clients){
		//forcing disconnection of client
		try{
			cl->disconnect();
		}catch(...){
			continue;
		}
	}
	clients.clear();
	s_op.close(sock);
	//removing socket file
	file_op f_op;
	f_op.remove(sock);
	mt.unlock();
}
int packet_listener::get_processing_sleep()const{
	return proc_sleep;
}
void packet_listener::change_processing_sleep(const int delta){
	proc_sleep += delta;
}
