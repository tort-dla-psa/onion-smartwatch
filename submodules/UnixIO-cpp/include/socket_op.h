#ifndef socket_op_h
#define socket_op_h

#include <string>
#include <memory>

template <typename T>
using sptr = std::shared_ptr<T>;

namespace IO{
class socket;
class socket_op{
	int errno_bak;
	bool check_error(int code);
public:
	socket_op();
	~socket_op();
	sptr<IO::socket> create(const std::string &path,
		int domain,
		int type,
		int protocol);
	sptr<IO::socket> create(int domain,
		int type,
		int protocol);
	sptr<IO::socket> create(const std::string &path,
		int domain,
		int type);
	bool bind(const sptr<IO::socket> &f);
	bool bind(const sptr<IO::socket> &f, int port);
	bool listen(const sptr<IO::socket> &f);
	bool listen(const sptr<IO::socket> &f, const int max_clients);
	sptr<IO::socket> accept(const sptr<IO::socket> &f);
	bool connect(const sptr<IO::socket> &f);
	bool connect(const sptr<IO::socket> &f, const std::string &ip, int port);
	bool close(sptr<IO::socket> f);
	bool send(const sptr<IO::socket> &f, const std::string &data);
	bool send(const sptr<IO::socket> &f, const int number);
	bool send(const sptr<IO::socket> &f, const char number);
	bool write(const sptr<IO::socket> &f, const std::string &data);
	bool write(const sptr<IO::socket> &f, const int number);
	bool recv(const sptr<IO::socket> &f, std::string &str, int size);
	bool recv(const sptr<IO::socket> &f, std::string &str, int size, int flags);
	bool recv(const sptr<IO::socket> &f, int &number);
	bool recv(const sptr<IO::socket> &f, int &number, int flags);
	bool read(const sptr<IO::socket> &f, std::string &str, int size);
	bool read(const sptr<IO::socket> &f, int &number);
	int get_errno()const;
	std::string get_errmes()const;
};
}

#endif
