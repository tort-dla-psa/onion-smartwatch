#pragma once

#include <memory>
#include <queue>
#include <string>
#include "API_CALLS.h"
#include "packet_listener.h"
#include "packet_sender.h"
#include "callback_invoker.h"
#include "packet.h"

#ifdef UI_BINFORMS
namespace binforms{
class binform;
}
#include "ui_event_manager.h"
#endif

namespace watches{

enum send_policy{
	once = 0,
	repeatedly,
	default_pol = once
};

class watchlib{
protected:
	std::unique_ptr<packet_listener> p_lis;
	std::unique_ptr<packet_sender> p_send;
	std::unique_ptr<callback_invoker> invkr;
	int app_pid, unix_pid;
	const std::string name;
	bool init_status;

	void init_dir();
	void init_ipc();
	void throw_ex(const std::string &mes);
#ifdef UI_BINFORMS
	std::unique_ptr<ui_event_manager> ui_ev_man;
	std::shared_ptr<binforms::binform> form;
	void cb_UI_cursor_press(const packet &p);
	void cb_UI_ask_image(const packet &p);
#endif
	void cb_ask_info(const packet &p);
	void cb_tell_info(const packet &p);
	packet construct_packet(API_CALL code, const std::vector<std::string> &args);
	std::map<std::string, std::queue<packet>> delayed_packets;
	//std::map<std::string, send_policy> policies;
	std::map<std::string, int> statistics;
public:
	watchlib(const std::string name);
	~watchlib();

	void init();
	void end();

#ifdef UI_BINFORMS
	void set_form(std::shared_ptr<binforms::binform> form);
#endif

	void add_callback(API_CALL code, std::function<void(const packet &)> cb){
		if(!init_status)
			throw_ex("library was not initialized");
		invkr->add_callback(code, cb);
	}

	void set_policy(const std::string name, send_policy policy);
	send_policy get_policy(const std::string &name);
	void send(const int pid, API_CALL code);
	void send(const int pid, API_CALL code, const std::vector<std::string> &args);
	void send(const int pid, API_CALL code, const std::vector<int> &args);
	void send(const std::string &name, API_CALL code);
	void send(const std::string &name, API_CALL code,
		const std::vector<std::string> &args);
	void send(const std::string &name, API_CALL code, const std::vector<int> &args);
	void send_log(const std::string &mes, API_CALL LOG_api_call);
	void broadcast(API_CALL code, const std::vector<std::string> &args);
	void start();
};

};

