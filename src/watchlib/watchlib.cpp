#include <iostream>
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
#define sl_name "sl"
#define ss_name "ss"


using namespace watches;
using namespace IO;

void print(const std::string &str){
	std::cout<<str<<'\n';
}
watchlib::watchlib(){
	init_status = false;
	app_pid = -1;
	appform = nullptr;
}

watchlib::~watchlib(){
	if(init_status){
		end();
	}
}

bool watchlib::init_dir(){
	file_op f_op;
	dir_op d_op;

	sptr<file> lock_f = nullptr;
	sptr<file> lastpid_f = nullptr;
	sptr<dir> process_d = nullptr;
	bool status = true;
	std::string dirname;

	{//check and create watches dir
		status = d_op.check(watches_path);
		if(!status)
			status = (d_op.create(watches_path) != nullptr);
		if(!status)
			return status = false;
	}

	while(f_op.check(lock_path)) //check lock
		usleep(100000);

	{//try to create lock file
		lock_f = f_op.create(lock_path, O_RDONLY, 0644);
		status &= (lock_f != nullptr);
		status &= f_op.close(lock_f);
		if(!status)
			goto error_file;
	}

	{//open or try to create lastpid file
		lastpid_f = f_op.open(lastpid_path, O_RDWR);
		if(!lastpid_f){
			lastpid_f = f_op.create(lastpid_path, O_RDWR, 0644);
			status &= (lastpid_f != nullptr);
			if(!status)
				goto error_file;
		}
	}

	{//read last pid
		int last = 0;
		std::string ch_last = f_op.read(lastpid_f);
		last = (ch_last != "")? std::atoi(ch_last.c_str()) : 0;
		dirname = std::to_string(last+1);
		this->app_pid = last+1;
	}

	status &= f_op.clear(lastpid_f);
       	status &= f_op.write(lastpid_f, dirname);
	status &= f_op.close(lastpid_f);
	if(!status)
		goto error_file;

	dirname = std::string(watches_path) + dirname;
	process_d = d_op.create(dirname);
	if(!process_d){ return status = false; }

	status &= f_op.remove(lock_f);
	if(!status)
		goto error_file;
	return true;
error_file:
	print(f_op.get_errmes());
	return false;
}
bool watchlib::init(){
	init_status = true;
	init_status &= init_dir();
	if(!init_status)
		return false;
	return true;
}

bool watchlib::end(){
	//TODO:delete sl and ss

	dir_op d_op;
	file_op f_op;

	const std::string dirpath = std::string(watches_path) +
		std::to_string(this->app_pid);
	sptr<dir> d = d_op.open(dirpath);
	if(!d_op.remove(d))
		return false;

	init_status = false;

	d = d_op.open(watches_path);
	int files = d_op.count_files(d)-2;
	if(files == 1 && f_op.check(lastpid_path)){
		d_op.remove(d);
	}
	return true;
}

void watchlib::set_form(const sptr<binform> &appform){
	this->appform = appform;
}

sptr<binform> watchlib::get_form()const{
	return appform;
}
