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

#ifdef DEBUG
watchlib::watchlib():watchlib(logger::get_instance()){}
#else
watchlib::watchlib():watchlib(nullptr){}
#endif

watchlib::watchlib(sptr<logger> dbg){
	this->dbg = dbg;
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
void watchlib::print(const std::string &mes){
	print(mes, 0);
}
void watchlib::print(const std::string &mes, int verb){
	if(dbg) dbg->output("watchlib",mes, verb);
}

void watchlib::init_dir(){
	file_op f_op;
	dir_op d_op;

	sptr<file> lock_f = nullptr;
	sptr<file> lastpid_f = nullptr;
	sptr<dir> process_d = nullptr;
	std::string dirname;

	{//check and create watches dir
		print(std::string("creating watches dir: ")+watches_path);
		if(!d_op.check(watches_path))
			d_op.create(watches_path);
	}

	{//TODO:change to subscription
		bool lock_present = f_op.check(lock_path);
		if(lock_present)
			print(std::string("lock file exists at: ")+lock_path+", waiting");
		while(f_op.check(lock_path)) //check lock
			usleep(100000);
	}

	{//try to create lock file
		print(std::string("creating lock file at: ")+lock_path);
		lock_f = f_op.create(lock_path, O_RDONLY, 0644);
		f_op.close(lock_f);
	}

	{//open or try to create lastpid file
		if(f_op.check(lastpid_path)){
			print(std::string("opening lastpid file at: ")+lastpid_path);
			lastpid_f = f_op.open(lastpid_path, O_RDWR);
		}else{
			print(std::string("creating lastpid file at: ")+lastpid_path);
			lastpid_f = f_op.create(lastpid_path, O_RDWR, 0644);
		}
	}

	{//read last pid and calculate new pid
		int last = -1;
		std::string str_last;
		print(std::string("reading last pid from: ")+lastpid_path);
		f_op.read(lastpid_f, str_last);
		if(!str_last.empty())
			last = std::atoi(str_last.c_str());
		this->app_pid = last+1;
		dirname = std::to_string(app_pid);
	}

	f_op.clear(lastpid_f);
       	f_op.write(lastpid_f, dirname);
	f_op.close(lastpid_f);

	//creating dir for appication
	dirname = std::string(watches_path) + dirname;
	print(std::string("creating dir at: ")+dirname);
	process_d = d_op.create(dirname);
	
	print("unlocking pids dir");
	f_op.remove(lock_f);
}

void watchlib::init_ipc(){
	const int max_cli = 10;
	const int proc_sleep = 1000;
	const std::string p_lis_path = std::string(watches_path) + 
		std::to_string(app_pid) + "/" + p_lis_name;
	p_lis = std::make_unique<packet_listener>
		(p_lis_path, max_cli, proc_sleep);
	p_lis->start();
	p_send = std::make_unique<packet_sender>();
}

void watchlib::init(){
	try{
		init_dir();
		init_ipc();
	}catch(const std::runtime_error &e){
		std::cerr<<e.what();
		return;
	}
	init_status = true;
}

void watchlib::end(){
	print("ending");
	print("stopping IPC");
	p_lis->stop();
	p_lis.reset(nullptr);
	p_send.reset(nullptr);

	dir_op d_op;
	file_op f_op;

	const std::string dirpath = std::string(watches_path) +
		std::to_string(this->app_pid);
	print(std::string("recursively removing app's dir:")+dirpath);
	sptr<dir> d = d_op.open(dirpath);
	d_op.remove(d);

	d = d_op.open(watches_path);
	int files = d_op.count_files(d)-2;
	if(files == 1 && f_op.check(lastpid_path)){
		print("cleaning watches dir");
		d_op.remove(d);
	}
	init_status = false;
}

void watchlib::set_form(sptr<binform> appform){
	this->appform = appform;
}

sptr<binform> watchlib::get_form()const{
	return appform;
}
void watchlib::add_callback(API_CALL code, func f){
	if(!init_status)
		throw_ex("library was not initialized");
	p_lis->add_callback(code, f);
}
void watchlib::send(const int pid, API_CALL code, const std::vector<std::string> &args){
	const std::string path = watches_path + std::to_string(pid) + "/" + p_lis_name;
	if(!p_send->is_connected(path))
		p_send->connect(path);
	const packet p(code, app_pid, unix_pid, "name", std::move(args));
	p_send->send(path, std::move(p));
}
void watchlib::send(const std::string &name, API_CALL code,
		const std::vector<std::string> &args)
{
}
