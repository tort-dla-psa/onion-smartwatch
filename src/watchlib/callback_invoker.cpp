#include "callback_invoker.h"

using namespace watches;

callback_invoker::callback_invoker(const sptr<types::concurrent_queue<packet>> &packets_queue,
		std::chrono::nanoseconds process_sleep)
	:packets_queue(packets_queue),
	process_sleep(process_sleep)
{ }

void callback_invoker::start_processing(){
	while(true){
		packet p(API_CALL::ask_info,0,0,"null",{}); // temp packet
		if(!packets_queue->wait_pop(p)){
			std::this_thread::sleep_for(process_sleep);
			continue;
		}
		std::map<API_CALL, cb>::iterator it = mp.find(p.get_val());
		if(it != mp.end()){
			(it->second)(p);
		}
	}
}
