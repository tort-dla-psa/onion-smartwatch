#ifndef data_protocol_h
#define data_protocol_h

#include <string>
#include "file.h"
#include "socket_op.h"

using namespace IO;

namespace watches{
namespace data_protocol{
	inline bool recv(sptr<socket_op> &s_op, sptr<IO::socket> sock,
		std::string &data)
	{
		int length;
		if(!data.empty())
			data.clear();
		bool status = s_op->recv(sock, length);
		if(!status)
			return false;
		data.reserve(length);
		status = s_op->recv(sock, data, length);
		if(!status)
			return false;
		return true;
	}
	inline bool recv(socket_op &s_op, sptr<IO::socket> sock,
		std::string &data)
	{
		int length;
		if(!data.empty())
			data.clear();
		bool status = s_op.recv(sock, length);
		data.reserve(length);
		if(!status)
			return false;
		status = s_op.recv(sock, data, length);
		if(!status)
			return false;
		return true;
	}
	inline bool send(sptr<socket_op> &s_op, sptr<IO::socket> sock, 
		const std::string &data)
	{
		bool status = s_op->send(sock, (int)data.size());
		if(!status)
			return false;
		status = s_op->send(sock, data);
		if(!status)
			return false;
		return true;
	}
	inline bool send(socket_op &s_op, sptr<IO::socket> sock, 
		const std::string &data)
	{
		bool status = s_op.send(sock, (int)data.size());
		if(!status)
			return false;
		status = s_op.send(sock, data);
		if(!status)
			return false;
		return true;
	}
};
};
#endif
