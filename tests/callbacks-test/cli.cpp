#include <iostream>
#include "watchlib.h"

using namespace watches;

int main(){
	watchlib wlib("cli");
	wlib.init();
	while(true){
		std::string mes;
		std::cin>>mes;
		wlib.send(0, API_CALL::LOG_send_info, {mes});
	}
}
