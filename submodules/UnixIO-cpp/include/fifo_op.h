#ifndef fifo_op_h
#define fifo_op_h

#include<string>
#include<memory>
#include"file.h"

template<typename T>
using sptr = std::shared_ptr<T>;

namespace IO{

class fifo_op{
	int errno_bak;
public:
	fifo_op();
	~fifo_op();
	sptr<fifo> create(const std::string &path, int perms);
	sptr<fifo> open(const std::string &path, int flags);
	sptr<fifo> open(sptr<fifo> f, int flags);
	bool close(sptr<fifo> f);
	bool write(sptr<fifo> f, const std::string &data);
	std::string read(sptr<fifo> f);
	int get_errno()const;
	std::string get_errmes()const;
};
}
#endif
