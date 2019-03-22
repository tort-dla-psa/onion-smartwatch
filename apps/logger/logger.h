#ifndef logger_h
#define logger_h

#include <memory>
#include <string>

template <typename T>
using sptr = std::shared_ptr<T>;

class logger{
	int verb;
	logger();
	std::string get_timestamp()const;
public:
	logger(const logger &d) = delete;
	void operator = (const logger &d) = delete;
	static sptr<logger> get_instance();
	int get_verbosity()const;
	void set_verbosity(const int level);
	void increase_verbosity(int delta);
	void decrease_verbosity(int delta);
	void output(const std::string &name,
		const std::string &data,
		int level)const;
};
#endif
