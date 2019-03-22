#ifndef packet_sender_h
#define packet_sender_h

#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <string>
#include "packet.h"
#include "file.h"
#include "socket_op.h"

template<typename T>
using sptr = std::shared_ptr<T>;

using namespace IO;

namespace watches{
class packet_sender{
	socket_op s_op;
	std::vector<sptr<IO::socket>> listeners;
	void throw_ex(const std::string &header);
	std::map<std::string, std::string> name_path_pair;
public:
	packet_sender();
	~packet_sender();
	void connect(const std::string &path);
	void send_by_path(const std::string &path, const packet &p);
	void send_by_name(const std::string &name, const packet &p);
	void disconn_by_path(const std::string &path);
	void disconn_by_name(const std::string &name);
	bool is_conn(const sptr<IO::socket> &listener)const;
	bool is_conn_by_path(const std::string &path)const;
	bool is_conn_by_name(const std::string &name)const;
	void associate(const std::string &path, const std::string &name);
};
}
#endif
