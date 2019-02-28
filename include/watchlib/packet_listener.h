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
#include "debugger.h"

template<typename T>
using sptr = std::shared_ptr<T>;

using namespace IO;

namespace watches{

class packet_listener_client{
	sptr<std::thread> thr;
	std::mutex mt;
	sptr<IO::socket> sock;
	sptr<debugger> dbg;
	socket_op s_op;
	bool connected;
	int id;
	void throw_ex(const std::string &header);
	std::vector<packet> querry;
	bool got_packets_flag;
	void process_packets();
	void print_dbg(const std::string &info, int verb);
public:
	packet_listener_client(sptr<IO::socket> sock, int id);
	~packet_listener_client();
	std::vector<packet> get_querry();
	bool got_packets();
	int get_id()const;
	void disconnect();
	bool get_connected();
};

class packet_listener{
	std::mutex mt;
	int max_clients;
	bool quit_requested;
	socket_op s_op;
	sptr<IO::socket> sock;
	sptr<debugger> dbg;
	typedef void(*func)(const packet);
	std::map<int, func>mp;
	std::vector<sptr<packet_listener_client>> clients;
	sptr<std::thread> accept_thread, reaction_thread;
	void print_dbg(const std::string &info, int verb);
protected:
	void reaction_func();
	void throw_ex(const std::string &header);
	void accept_client();
public:
	packet_listener(const std::string &path, const int max_clients);
	~packet_listener();
	void add_callback(int code, func f);
	void start();
};

}
#endif
