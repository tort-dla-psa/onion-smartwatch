#ifndef data_protocol_h
#define data_protocol_h

#include <string>
#include "file.h"
#include "socket_op.h"

using namespace IO;

namespace watches{
namespace data_protocol{
	inline void recv(sptr<socket_op> &s_op, sptr<IO::socket> sock,
		std::string &data)
	{
		int length;
		if(!data.empty())
			data.clear();
		s_op->recv(sock, length);
		data.reserve(length);
		s_op->recv(sock, data, length);
	}
	inline void recv(socket_op &s_op, sptr<IO::socket> sock,
		std::string &data)
	{
		int length;
		if(!data.empty())
			data.clear();
		s_op.recv(sock, length);
		data.reserve(length);
		s_op.recv(sock, data, length);
	}
	inline void send(sptr<socket_op> &s_op, sptr<IO::socket> sock, 
		const std::string &data)
	{
		s_op->send(sock, (int)data.size());
		s_op->send(sock, data);
	}
	inline void send(socket_op &s_op, sptr<IO::socket> sock, 
		const std::string &data)
	{
		s_op.send(sock, (int)data.size());
		s_op.send(sock, data);
	}
};
};
#endif
