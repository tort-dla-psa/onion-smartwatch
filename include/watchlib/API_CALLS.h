#pragma once

namespace watches{

enum class API_CALL{
	ask_info = 0,
	tell_info,
	request_end,
	UI_key_pressed,
	UI_key_released,
	UI_cursor_moved,
	UI_cursor_pressed,
	UI_ask_image,
	UI_send_image,
	LOG_send_info,
	LOG_send_debug,
	LOG_send_error
};

}
