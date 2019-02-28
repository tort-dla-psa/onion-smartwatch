#ifndef packet_sender_h
#define packet_sender_h

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <string>
#include "packet.h"
#include "file.h"
#include "socket_op.h"
#include "debugger.h"

template<typename T>
using sptr = std::shared_ptr<T>;

using namespace IO;

namespace watches{
class packet_sender{
	socket_op s_op;
	std::vector<sptr<IO::socket>> listeners;
	void throw_ex(const std::string &header);
	sptr<debugger> dbg;
	void print_dbg(const std::string &info, int verb);
public:
	packet_sender();
	~packet_sender();
	void connect(const std::string &path);
	void send(const std::string &path, const packet p);
	void disconnect(const std::string &path);
};
}
#endif
