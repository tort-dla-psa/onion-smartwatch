#include <iostream>
#include "callback_invoker.h"

using namespace watches;

callback_invoker::callback_invoker(const sptr<types::concurrent_queue<packet>> &packets_queue,
		std::chrono::nanoseconds process_sleep)
	:packets_queue(packets_queue),
	process_sleep(process_sleep)
{ }

void callback_invoker::start_processing(){
	end_requested = false;
	while(!end_requested){
		packet p(API_CALL::ask_info,0,0,"null",{}); // temp packet
		if(!packets_queue->wait_pop(p)){
			if(packets_queue->exit_requested()){
				break;
			}else{
				std::this_thread::sleep_for(process_sleep);
				continue;
			}
		}
#ifdef DEBUG
		std::cout<<"CI:got packet from:"<<p.get_name()
			<<",call:"<<p.get_pid()<<",args:\n";
		for(const auto &arg:p.get_args()){
			std::cout<<"CI:arg:"<<arg<<"\n";
		}
#endif
		auto it = mp.find(p.get_val());
		if(it != mp.end()){
			try{
				(it->second)(p);
			}catch(const std::runtime_error &e){
				std::cerr<<"error in callback:"<<e.what()<<"\n";
			}
		}
	}
}

void callback_invoker::end(){
	end_requested = true;
	packets_queue->clear();
}
