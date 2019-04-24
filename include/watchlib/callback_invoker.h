#pragma once

#include <memory>
#include <functional>
#include <map>
#include <thread>
#include <chrono>
#include "concurrent_queue.h"
#include "packet.h"

template<typename T>
using sptr = std::shared_ptr<T>;

namespace watches{
class callback_invoker{
	sptr<types::concurrent_queue<packet>> packets_queue;
	typedef std::function<void(const packet&)> cb; //callback allias
	std::map<API_CALL, cb> mp; 	//<==== callbacks
	std::chrono::nanoseconds process_sleep;
public:
	callback_invoker(const sptr<types::concurrent_queue<packet>> &packets_queue,
			std::chrono::nanoseconds process_sleep);
	void add_callback(API_CALL code, void(*cb)(const packet&)){
		mp[code] = std::bind(cb, std::placeholders::_1);
	}
	template<class obj>
	void add_callback(API_CALL code, void(obj::*cb)(const packet&), sptr<obj> o){
		mp[code] = std::bind(cb, o, std::placeholders::_1);
	}
	void delete_callback(API_CALL code, void(*cb)(const packet&)) = delete;
	template<class obj>
	void delete_callback(API_CALL code, void(obj::*cb)(const packet&), sptr<obj> o) = delete;
	void start_processing();
};
};
