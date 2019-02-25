#ifndef dir_op_h
#define dir_op_h

#include <vector>
#include <string>
#include <memory>

template<typename T>
using sptr = std::shared_ptr<T>;

namespace IO{

class dir_op{
public:
	dir_op();
	~dir_op();

	bool check(sptr<dir> d);
	bool check(const std::string &path);
	sptr<dir> create(const std::string &path, int flags);
	sptr<dir> create(const std::string &path);
	sptr<dir> open(const std::string &path);
	bool copy(sptr<dir> src, sptr<dir> dst);
	bool move(sptr<dir> src, sptr<dir> dst);
	bool rename(sptr<dir> src, const std::string &dst);
	bool remove(sptr<dir> d);
	std::vector<sptr<unix_file>> list(sptr<dir> d);
	int count_files(sptr<dir> d);
};
}
#endif
