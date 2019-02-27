#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "util.h"

#define testdir		"tst"
#define lockname	".lock"
#define lastname	".last"
#define lockpath	testdir "/" lockname
#define lastpath	testdir "/" lastname

bool lock(){
	bool status = util::create_file(lockpath);
	return status;
}

bool unlock(){
	bool status = util::remove_file(lockpath);
	return status;
}

bool check_lock(){
	return util::check_file(lockpath);
}

int read_last(){
	char* last = util::read_file(lastpath);
	if(last == nullptr){
		return -1;
	}
	int int_last = std::atoi(last);
	delete[] last;
	return int_last;
}

void print_last(){
	std::cout<<"Last dir name: "<<read_last()<<"\n";
}

bool autocreate(){
	if(!check_lock()){
		lock();
	}else{
		while(!check_lock()){
			sleep(1);
			lock();
		}
	}
	bool status = false;
	int last = read_last();
	status = (last != -1);
	if(!status){ unlock(); return false; }
	std::string dirname = std::to_string(last+1);
	status = util::create_dir(dirname.c_str());
	if(!status){ unlock(); return false; }
	status = util::clear_file(lastpath);
	if(!status){ unlock(); return false; }
	status = util::write_file(lastpath, dirname.c_str());
	if(!status){ unlock(); return false; }
	unlock();
	return true;
}

bool init(){
	bool status = util::create_dir(testdir);
	status &= util::create_file(lastpath);
	status &= util::write_file(lastpath, "0");
	return status;
}

void clean(){
	util::remove_dir_rec(testdir);
}

int main(){
	if(!init()){
		std::cout<<"can't init\n";
		return -1;
	}
	autocreate();
	print_last();
	autocreate();
	autocreate();
	print_last();
	clean();
}
