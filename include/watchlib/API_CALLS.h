#ifndef API_CALL_H
#define API_CALL_H

namespace watches{

enum class API_CALL{
	ask_info = 0,
	tell_info,
	UI_key_pressed,
	UI_key_released,
	UI_cursor_moved,
	LOG_send_info,
	LOG_send_debug,
	LOG_send_error
};

}
#endif
