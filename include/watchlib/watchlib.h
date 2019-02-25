#ifndef watchlib_h
#define watchlib_h

#include <memory>
#include "binform.h"

template<typename T>
using sptr = std::shared_ptr<T>;

namespace watches{

class watchlib{
private:
	sptr<binform> appform;
	int app_pid;
	bool init_status;

	bool init_dir();
public:
	watchlib();
	~watchlib();

	bool init();
	bool end();

	void set_form(const sptr<binform> &form);
	sptr<binform> get_form()const;
};

};

#endif
