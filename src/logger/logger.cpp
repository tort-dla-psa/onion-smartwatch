#include <iostream>
#include <ctime>
#include "logger.h"

logger::logger(){
	this->verb = 0;
}

sptr<logger> logger::get_instance(){
	static sptr<logger> instance(new logger());
	return instance;
}
std::string logger::get_timestamp()const{
	std::time_t res = std::time(nullptr);
	std::string str = std::asctime(std::localtime(&res));
	return str.substr(0,str.size()-1);
}
int logger::get_verbosity()const{
	return verb;
}
void logger::set_verbosity(const int level){
	this->verb = level;
}
void logger::increase_verbosity(int delta){
	if(delta>0){
		verb += delta;
	}
}
void logger::decrease_verbosity(int delta){
	if(delta>0){
		verb -= delta;
	}
}
void logger::output(const std::string &name,
	const std::string &data,
	int level)const
{
	if(verb < level){
		return;
	}
	std::cout<<"["<<get_timestamp()<<"]"<<name<<":\t"<<data<<'\n';
}
