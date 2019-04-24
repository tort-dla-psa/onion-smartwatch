#pragma once

#include <memory>
#include "API_CALLS.h"
#include "binform.h"
#include "packet_listener.h"
#include "packet_sender.h"
#include "callback_invoker.h"
#include "packet.h"

template<typename T> using sptr = std::shared_ptr<T>;
template<typename T> using uptr = std::unique_ptr<T>;

namespace watches{

class watchlib: public std::enable_shared_from_this<watchlib>{
protected:
	sptr<binform> appform;
	uptr<packet_listener> p_lis;
	uptr<packet_sender> p_send;
	uptr<callback_invoker> invkr;
	int app_pid, unix_pid;
	const std::string name;
	bool init_status;

	void init_dir();
	void init_ipc();
	void throw_ex(const std::string &mes);
	void cb_ask_info(const packet &p);
	void cb_tell_info(const packet &p);
	std::vector<std::pair<std::string, std::string>> apps_info;
	packet construct_packet(API_CALL code, const std::vector<std::string> &args);
	sptr<watchlib> this_ptr;
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
		invkr->add_callback(code, cb);
	}

	template<class obj>
	void add_callback(API_CALL code, void(obj::*cb)(const packet&), sptr<obj> o){
		if(!init_status)
			throw_ex("library was not initialized");
		invkr->add_callback(code, cb, o);
	}

	void send(const int pid, API_CALL code, const std::vector<std::string> &args);
	void send(const std::string &name, API_CALL code,
			const std::vector<std::string> &args);
	void send_log(const std::string &mes, API_CALL LOG_api_call);
	void broadcast(API_CALL code, const std::vector<std::string> &args);
};

};

