#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include "watchlib.h"

#include "file.h"
#include "file_op.h"
#include "dir_op.h"

const std::string watches_path = "/tmp/watches/";
const std::string lock_name = "update_lock";
const std::string lock_path = watches_path+lock_name;
const std::string lastpid_name = "lastpid";
const std::string lastpid_path = watches_path + lastpid_name;
const std::string p_lis_name = "p_lis";

using namespace watches;
using namespace IO;

watchlib::watchlib(const std::string &name)
	:name(name)
{
	init_status = false;
	app_pid = -1;
	appform = nullptr;
}

watchlib::~watchlib(){
	if(init_status){
		end();
	}
}
void watchlib::throw_ex(const std::string &mes){
	const std::string out_mes = std::string("watchlib: ")+mes+"\n";
	throw std::runtime_error(out_mes);
}
void watchlib::send_log(const std::string &mes, API_CALL LOG_api_call){
	if(LOG_api_call < API_CALL::LOG_send_info ||
		LOG_api_call > API_CALL::LOG_send_error)
	{
		return;
	}
	send("logger", LOG_api_call, { "watchlib", mes });
}
packet watchlib::construct_packet(API_CALL code, const std::vector<std::string> &args){
	return packet(code, app_pid, unix_pid, name, args);
}
void watchlib::cb_ask_info(const packet &p){
	const std::string path = watches_path + std::to_string(p.get_pid()) +
		"/" + p_lis_name;
	p_send->send_by_path(path, construct_packet(API_CALL::tell_info, {}));
}
void watchlib::cb_tell_info(const packet &p){
	const auto name = p.get_name();
	const std::string path = watches_path + std::to_string(p.get_pid()) +
		"/" + p_lis_name;
	const auto it = std::find_if(apps_info.begin(), apps_info.end(),
		[&](const auto &pair){ return (pair.first == path); });
	if(it == apps_info.end()){
		apps_info.emplace_back(path, name);
	}
}
void watchlib::init_dir(){
	file_op f_op;
	dir_op d_op;

	sptr<file> lock_f = nullptr, lastpid_f = nullptr;
	sptr<dir> process_d = nullptr;
	std::string dirname;

	//check and create watches dir
	if(!d_op.check(watches_path)){
		send_log(std::string("creating watches dir: ")+watches_path,
			API_CALL::LOG_send_info);
		try{
			d_op.create(watches_path);
		}catch(const std::runtime_error &e){
			send_log(e.what(), API_CALL::LOG_send_error);
			throw_ex(e.what());
		}
	}

	//TODO:change to subscription
	try{
		if(f_op.check(lock_path)){
			send_log(std::string("lock file exists at: ")+lock_path+", waiting",
				API_CALL::LOG_send_info);
			while(f_op.check(lock_path)) //check lock
				usleep(100000);
		}
	}catch(const std::runtime_error &e){
		send_log(e.what(), API_CALL::LOG_send_error);
		throw_ex(e.what());
	}


	try{//try to create lock file
		lock_f = f_op.create(lock_path, O_RDONLY, 0644);
		f_op.close(lock_f);
	}catch(const std::runtime_error &e){
		send_log(e.what(), API_CALL::LOG_send_error);
		throw_ex(e.what());
	}

	try{//open or try to create lastpid file
		if(f_op.check(lastpid_path)){
			lastpid_f = f_op.open(lastpid_path, O_RDWR);
		}else{
			lastpid_f = f_op.create(lastpid_path, O_RDWR, 0644);
		}
	}catch(const std::runtime_error &e){
		send_log(e.what(), API_CALL::LOG_send_error);
		throw_ex(e.what());
	}

	try{//read last pid and calculate new pid
		int last = -1;
		std::string str_last;
		f_op.read(lastpid_f, str_last);
		if(!str_last.empty())
			last = std::atoi(str_last.c_str());
		this->app_pid = last+1;
		dirname = std::to_string(app_pid);
	}catch(const std::runtime_error &e){
		send_log(e.what(), API_CALL::LOG_send_error);
		throw_ex(e.what());
	}

	try{
		f_op.clear(lastpid_f);
		f_op.write(lastpid_f, dirname);
		f_op.close(lastpid_f);

		dirname = std::string(watches_path) + dirname; //creating dir for appication
		process_d = d_op.create(dirname);
		
		f_op.remove(lock_f);
	}catch(const std::runtime_error &e){
		send_log(e.what(), API_CALL::LOG_send_error);
		throw_ex(e.what());
	}
}

void watchlib::init_ipc(){
	const int max_cli = 10;
	const int proc_sleep = 1000;
	const std::string p_lis_path = std::string(watches_path) + 
		std::to_string(app_pid) + "/" + p_lis_name;
	try{
		p_lis = std::make_unique<packet_listener>(p_lis_path, max_cli, proc_sleep);
		p_lis->start();
		p_send = std::make_unique<packet_sender>();
	}catch(const std::runtime_error &e){
		send_log(e.what(), API_CALL::LOG_send_error);
		throw_ex(e.what());
	}
}

void watchlib::init(){
	try{
		init_dir();
		init_ipc();
		p_lis->add_callback(API_CALL::tell_info, &watchlib::cb_ask_info, this);
	}catch(const std::runtime_error &e){
		send_log(e.what(), API_CALL::LOG_send_error);
		throw_ex(e.what());
	}
	init_status = true;
}

void watchlib::end(){
	send_log("ending", API_CALL::LOG_send_info);
	p_lis->stop();
	p_lis.reset(nullptr);
	p_send.reset(nullptr);

	dir_op d_op;
	file_op f_op;

	const std::string dirpath = std::string(watches_path) +
		std::to_string(this->app_pid);
	send_log(std::string("recursively removing app's dir:")+dirpath,
		API_CALL::LOG_send_info);
	try{
		sptr<dir> d = d_op.open(dirpath);
		d_op.remove(d);
		//check if it's the last app
		d = d_op.open(watches_path);
		int files = d_op.count_files(d)-2;
		if(files == 1 && f_op.check(lastpid_path)){
			send_log("cleaning watches dir",
				API_CALL::LOG_send_info);
			d_op.remove(d);
		}
	}catch(const std::runtime_error &e){
		send_log(e.what(), API_CALL::LOG_send_error);
		throw_ex(e.what());
	}
	init_status = false;
}

void watchlib::set_form(sptr<binform> appform){
	this->appform = appform;
}

sptr<binform> watchlib::get_form()const{
	return appform;
}
void watchlib::send(const int pid, API_CALL code, const std::vector<std::string> &args){
	const std::string path = watches_path + std::to_string(pid) + "/" + p_lis_name;
	if(!p_send->is_conn_by_path(path))
		p_send->connect(path);
	p_send->send_by_path(path, {code, app_pid, unix_pid, name, std::move(args)});
}
void watchlib::send(const std::string &name, API_CALL code,
	const std::vector<std::string> &args)
{
	//1.ask every app for info;
	//2.packet_listener will get answer and call cb_tell_info in other thread;
	//3.cb_tell_info will add path and name to "found_names" vector;
	//4.this metod will iterate through "found_names" vector and connect
	//	to each path with desired name;
	//5.add path-name association to p_send;
	if(!p_send->is_conn_by_name(name)){
		broadcast(API_CALL::ask_info, {});
		std::this_thread::sleep_for(std::chrono::milliseconds(50));//wait for apps
		auto it = apps_info.begin();
		while(it != apps_info.end()){
			if(it->second == name){
				if(p_send->is_conn_by_path(it->first)){
					//delete if p_send allready connected to this path
					it = apps_info.erase(it);
					continue;
				}
				p_send->connect(it->first);
				p_send->associate(it->first, it->second);
				p_send->send_by_path(it->first,
					construct_packet(code,std::move(args)));
				it = apps_info.erase(it);
				continue;
			}
			if(p_send->is_conn_by_path(it->first)){
				//delete if p_send allready connected to this path
				it = apps_info.erase(it);
				continue;
			}
			it++;
		}
	}else{
		p_send->send_by_name(name, construct_packet(code,std::move(args)));
	}
}
void watchlib::broadcast(API_CALL code, const std::vector<std::string> &args){
	dir_op d_op;
	const auto dir = d_op.open(watches_path);
	const auto files = d_op.list(dir); //read all files in watches dir
	for(const auto &file : files){
		auto cast = std::dynamic_pointer_cast<IO::dir>(file);
		if(!cast) //file is not dir
			continue;
		const auto pid_dir = d_op.open(cast->get_path());
		const auto pid_dir_files = d_op.list(pid_dir);
		for(const auto &pid_dir_file : pid_dir_files){//get listener socket
			std::string name;
			{//get name from path
				name = pid_dir_file->get_path();
				int pos = name.find_last_of('/');
				name = name.substr(pos);
			}
			if(name != p_lis_name)//it's not p_lis socket
				continue;
			const std::string path = pid_dir_file->get_path();
			if(!p_send->is_conn_by_path(path))
				p_send->connect(path);
			const packet p = construct_packet(code, args);
			p_send->send_by_path(path, p);
			p_send->disconn_by_path(path);
		}
	}
}
