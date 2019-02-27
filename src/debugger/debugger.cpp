#include <iostream>
#include <ctime>
#include "debugger.h"

debugger::debugger(){
	this->verb = 0;
}

sptr<debugger> debugger::get_instance(){
	static sptr<debugger> instance(new debugger());
	return instance;
}
std::string debugger::get_timestamp()const{
	std::time_t res = std::time(nullptr);
	std::string str = std::asctime(std::localtime(&res));
	return str.substr(0,str.size()-1);
}
int debugger::get_verbosity()const{
	return verb;
}
void debugger::set_verbosity(const int level){
	this->verb = level;
}
void debugger::increase_verbosity(int delta){
	if(delta>0){
		verb += delta;
	}
}
void debugger::decrease_verbosity(int delta){
	if(delta>0){
		verb -= delta;
	}
}
void debugger::output(const std::string &name,
	const std::string &data,
	int level)const
{
	if(verb < level){
		return;
	}
	std::cout<<"["<<get_timestamp()<<"]"<<name<<":\t"<<data<<'\n';
}
