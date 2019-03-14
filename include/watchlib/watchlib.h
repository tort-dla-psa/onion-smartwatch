#ifndef watchlib_h
#define watchlib_h

#include <memory>
#include "API_CALLS.h"
#include "binform.h"
#include "packet_listener.h"
#include "packet_sender.h"
#include "packet.h"
#include "logger.h"

template<typename T>
using sptr = std::shared_ptr<T>;
template<typename T>
using uptr = std::unique_ptr<T>;

namespace watches{

class watchlib{
protected:
	sptr<binform> appform;
	sptr<logger> dbg;
	uptr<packet_listener> p_lis;
	uptr<packet_sender> p_send;
	int app_pid, unix_pid;
	bool init_status;
	typedef void(*func)(const packet);

	void init_dir();
	void init_ipc();
	void throw_ex(const std::string &mes);
	void print(const std::string &mes, int verb);
	void print(const std::string &mes);
	watchlib(sptr<logger> dbg);
public:
	watchlib();
	~watchlib();

	void init();
	void end();

	void set_form(sptr<binform> form);
	sptr<binform> get_form()const;
	void add_callback(API_CALL code, func f);
	void send(const int pid, API_CALL code, const std::vector<std::string> &args);
	void send(const std::string &name, API_CALL code,
			const std::vector<std::string> &args);
};

};

#endif
