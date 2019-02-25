#ifndef packet_listener_h
#define packet_listener_h

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

#include "packet.h"
#include "file.h"
#include "socket_op.h"

template<typename T>
using sptr = std::shared_ptr<T>;

using namespace IO;

namespace watches{

class packet_listener{
	socket_op s_op;
	sptr<IO::socket> sock;
	typedef void(*func)(const packet);
	std::map<const packet, func>mp;
	std::vector<sptr<IO::socket>> clients;
	std::mutex mt;
	void accept_func();
	void recv_func(sptr<IO::socket> cli);
	void try_recv(sptr<IO::socket> cli, const int time);
	void throw_ex(const std::string &header);
public:
	packet_listener(const std::string &path, const int max_clients);
	~packet_listener();
	void add_callback(const packet p,func f);
	void try_accepting(const int time);
	void spin(const int recv_time);
};

}
#endif
