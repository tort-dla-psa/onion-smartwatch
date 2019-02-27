#ifndef debugger_h
#define debugger_h

#include <memory>
#include <string>

template <typename T>
using sptr = std::shared_ptr<T>;

class debugger{
	int verb;
	debugger();
	std::string get_timestamp()const;
public:
	debugger(const debugger &d) = delete;
	void operator = (const debugger &d) = delete;
	static sptr<debugger> get_instance();
	int get_verbosity()const;
	void set_verbosity(const int level);
	void increase_verbosity(int delta);
	void decrease_verbosity(int delta);
	void output(const std::string &name,
		const std::string &data,
		int level)const;
};
#endif
