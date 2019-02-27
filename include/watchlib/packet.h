#ifndef packet_h
#define packet_h

#include <string>
#include <array>
#include <vector>

namespace watches{

const int packet_header_size = 3;
const int packet_pid_place = 0;
const int packet_upid_place = 1;
const int packet_name_place = 2;

class packet{
	int val;
	std::array<std::string, packet_header_size> header;
	std::vector<std::string> args;
public:
	packet(int val, int pid, int upid, 
		const std::string &name,
		const std::vector<std::string> &args);
	packet(const std::string &data);
	packet(int val, const std::array<std::string, packet_header_size> &header);
	void add_arg(const std::string &arg);
	void add_arg(const std::vector<std::string> &args);
	void set_arg(const std::string &arg);
	void set_arg(const std::vector<std::string> &args);
	int get_val()const;
	int get_pid()const;
	int get_upid()const;
	std::string get_name()const;
	std::vector<std::string> get_args()const;
	std::string serialize()const;
	bool operator == (const packet &rhs) const;
	bool operator < (const packet &rhs) const;
};
}
#endif
