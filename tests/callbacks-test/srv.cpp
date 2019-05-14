#include <iostream>
#include "watchlib.h"

using namespace watches;

void cb_pure(const packet &p){
	std::cout<<p.get_args()[0]<<"\n";
}

int main(){
	watchlib wlib("srv");
	wlib.init();
	wlib.add_callback(API_CALL::LOG_send_info, cb_pure);
	wlib.start();
}

