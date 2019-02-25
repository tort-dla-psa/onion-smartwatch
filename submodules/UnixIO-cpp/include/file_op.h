#ifndef file_operator_h
#define file_operator_h

#include <string>
#include <memory>

template<typename T>
using sptr = std::shared_ptr<T>;

namespace IO{

class file;
class file_op{
	int errno_bak;
public:
	file_op();
	~file_op();
	
	bool check(sptr<file> f);
	bool check(const std::string &path);
	sptr<file> create(const std::string &path, int flags, int perms);
	sptr<file> open(const std::string &path, int flags);
	bool close(sptr<file> f);
	bool remove(sptr<file> f);
	bool rename(sptr<file> f, const std::string &name);
	bool move(sptr<file> f, const std::string &path);
	bool copy(sptr<file> f, const std::string &path);
	bool clear(sptr<file> f);
	bool write(sptr<file> f, const std::string &data);
	std::string read(sptr<file> f);
	int get_errno()const;
	std::string get_errmes()const;
};
}
#endif
