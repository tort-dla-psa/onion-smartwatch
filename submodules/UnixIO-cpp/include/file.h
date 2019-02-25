#ifndef file_class_h
#define file_class_h

#include <string>

namespace IO{

class unix_file{
	std::string path;
public:
	unix_file(const std::string &path);
	virtual ~unix_file();
	std::string get_path()const;
};

class file:public unix_file{
	int fd;
protected:
	friend class file_op;
	int get_fd()const;
	void set_fd(const int fd);
public:
	file(int fd, const std::string &path);
	file(const std::string &path);
	~file();
};

class dir:public unix_file{
public:
	dir(const std::string &path);
	~dir();
};

class symlink:public file{
public:
	symlink(const std::string &path);
	~symlink();
};

class socket:public file{
	//bool binded;
	//bool listening;
protected:
	friend class socket_op;
	int get_fd()const;
	void set_fd(const int fd);
public:
	socket(const std::string &path);
	socket();
	~socket();
	bool get_bind()const;
	bool get_listen()const;
};

class fifo:public file{
	int fd;
protected:
	friend class fifo_op;
	int get_fd()const;
	void set_fd(const int fd);
public:
	fifo(int fd, const std::string &path);
	fifo(const std::string &path);
	~fifo();
};

class ch_dev:public unix_file{
public:
	ch_dev(const std::string &path);
	~ch_dev();
};

class blk_dev:public unix_file{
public:
	blk_dev(const std::string &path);
	~blk_dev();
};

}
#endif
