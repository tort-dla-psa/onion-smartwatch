#ifndef watchlib_h
#define watchlib_h

#include <memory>
#include "API_CALLS.h"
#include "binform.h"
#include "packet_listener.h"
#include "packet_sender.h"
#include "packet.h"

template<typename T> using sptr = std::shared_ptr<T>;
template<typename T> using uptr = std::unique_ptr<T>;

namespace watches{

class watchlib{
protected:
	sptr<binform> appform;
	uptr<packet_listener> p_lis;
	uptr<packet_sender> p_send;
	int app_pid, unix_pid;
	const std::string name;
	bool init_status;

	void init_dir();
	void init_ipc();
	void throw_ex(const std::string &mes);
	packet construct_packet(API_CALL code, const std::vector<std::string> &args);
	void cb_ask_info(const packet &p);
	void cb_tell_info(const packet &p);
	std::vector<std::pair<std::string, std::string>> apps_info;
public:
	watchlib(const std::string &name);
	~watchlib();

	void init();
	void end();

	void set_form(sptr<binform> form);
	sptr<binform> get_form()const;
	void add_callback(API_CALL code, void(*cb)(const packet&)){
		if(!init_status)
			throw_ex("library was not initialized");
		p_lis->add_callback(code, cb);
	}
	template<class obj>
	void add_callback(API_CALL code, void(obj::*cb)(const packet&), obj* o){
		if(!init_status)
			throw_ex("library was not initialized");
		p_lis->add_callback(code, cb, o);
	}
	template<class obj>
	void add_callback(API_CALL code, void(obj::*cb)(const packet&), sptr<obj> o){
		if(!init_status)
			throw_ex("library was not initialized");
		p_lis->add_callback(code, cb, o);
	}
	void send(const int pid, API_CALL code, const std::vector<std::string> &args);
	void send(const std::string &name, API_CALL code,
			const std::vector<std::string> &args);
	void send_log(const std::string &mes, API_CALL LOG_api_call);
	void broadcast(API_CALL code, const std::vector<std::string> &args);
};

};

#endif
