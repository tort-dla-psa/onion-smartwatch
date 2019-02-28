#include <sstream>
#include "packet.h"

using namespace watches;

const std::string reg_delimiter = "|";

packet::packet(int val, int pid, int upid,
	const std::string &name,
	const std::vector<std::string> &args)
{
	this->val = val;
	header = std::array<std::string, packet_header_size>();
	header[packet_pid_place] = std::to_string(pid);
	header[packet_upid_place] = std::to_string(upid);
	header[packet_name_place] = name;
	this->args = args;
}

packet::packet(const std::string &data){
	std::istringstream iss(data);
	std::string token;
	int count = 0;
	header = std::array<std::string, packet_header_size>();
	while (std::getline(iss, token, '|')){
		if(count == 0){
			val = std::stoi(token);
		}else if(count < packet_header_size + 1){
			header[count - 1] = token;
		}else{
			args.emplace_back(token);
		}
		count++;
	}
}

packet::packet(int val, const std::array<std::string, packet_header_size> &header){
	this->val = val;
	this->header = header;
}

void packet::add_arg(const std::string &arg){
	args.emplace_back(arg);
}

void packet::add_arg(const std::vector<std::string> &args){
	this->args.reserve(args.size());
	this->args.insert(this->args.end(), args.begin(), args.end());
}

void packet::set_arg(const std::string &arg){
	this->args.clear();
	args.emplace_back(arg);
}

void packet::set_arg(const std::vector<std::string> &args){
	this->args = args;
}

int packet::get_val()const{
	return val;
}
int packet::get_pid()const{
	return std::stoi(header[packet_pid_place]);
}
int packet::get_upid()const{
	return std::stoi(header[packet_upid_place]);
}
std::string packet::get_name()const{
	return header[packet_name_place];
}
std::vector<std::string> packet::get_args()const{
	return args;
}

std::string packet::serialize()const{
	std::string data = std::to_string(val);
	data.append(reg_delimiter);
	for(const std::string &tmp_str:header){
		data.append(tmp_str);
		data.append(reg_delimiter);
	}
	for(const std::string &arg:args){
		data.append(arg);
		data.append(reg_delimiter);
	}
	return data;
}

bool packet::operator == (const packet &rhs) const {
	return (val == rhs.val);
}

bool packet::operator < (const packet &rhs) const {
	return (val < rhs.val);
}

