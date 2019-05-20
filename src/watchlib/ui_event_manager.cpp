#ifdef UI_BINFORMS
#include "binform.h"
#include "event.h" //binforms events
#include "ui_event_manager.h"

using namespace watches;
using namespace binforms;

ui_event_manager::ui_event_manager(sptr<binform> form){
	this->form = form;
}

void ui_event_manager::process_event(const int x, const int y, int ev_id){
	if(!form){
		return;
	}
	auto el = form->get_element(x,y);
	const auto ev = static_cast<BINFORM_EVENTS>(ev_id);
	const std::shared_ptr<event> e(new event(x,y));
	if(ev == BINFORM_EVENTS::focus_enter){
		el->on_focus_enter_e(e);
	}else if(ev == BINFORM_EVENTS::focus_leave){
		el->on_focus_leave_e(e);
	}else if(ev == BINFORM_EVENTS::press){
		el->on_press_e(e);
	}else if(ev == BINFORM_EVENTS::release){
		el->on_release_e(e);
	}else if(ev == BINFORM_EVENTS::tap){
		el->on_tap_e(e);
	}
}
#endif
