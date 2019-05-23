#pragma once
#include <memory>

namespace binforms{
class binform;
};

namespace watches{

class ui_event_manager{
	std::shared_ptr<binforms::binform> form;
public:
	ui_event_manager(std::shared_ptr<binforms::binform> form);
	void process_event(const int x, const int y, int ev_id);
};

}
