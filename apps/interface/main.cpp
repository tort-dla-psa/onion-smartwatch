#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <unistd.h>
#include "stat_op.h"
#include "binform.h"
#include "graphics.h"
#include "stack.h"
#include "label.h"
#include "event.h"
#include "button.h"
#include "imagebox.h"
#include "watchlib.h"
#include "oled-exp.h"
#include "binfont.h"
#include "clock.h"
#include "tools.h"

using namespace watches;
using namespace IO;
using namespace binforms;

using uint = unsigned int;
const std::string watches_path("/home/tort/gits/onion-smartwatch/");
const uint app_w = 100, app_h = 64;

std::vector<sptr<std::thread>> children;

void throw_ex(const std::string &mes){
	const std::string fullmes = "interface: exception: " + mes + "\n";
	throw std::runtime_error(fullmes);
}

inline void draw_img(std::shared_ptr<image> img){
	const uint w = img->get_w();
	const uint h = img->get_h();

	for(uint i=0; i<h; i++){
		std::string line="";
		line.reserve(w);
		for(uint j=0; j<w; j++){
			line += (img->get_pixel(j,i)? '#':'_');
		}
		std::cout<<line<<'\n';
	}
}

void launch(const std::string &path, const std::vector<std::string> &args){
	stat_op st_op;
	if(!st_op.set_path(path)){
		throw std::runtime_error("stat error on " + path + 
				"\n"+strerror(errno));
	}
	if(!st_op.is_file()){
		throw std::runtime_error(path + " is not file or does not exist"+
				"\n"+strerror(errno));
	}
	if(!st_op.check_exec_user()){
		throw std::runtime_error("this user can't launch " + path+
				"\n"+strerror(errno));
	}

	pid_t pid = fork();
	if(pid == 0){ //child
		//no output to stdout
		int fd = open("/dev/null", O_WRONLY);
		//dup2(fd, 1);
		//dup2(fd, 2);
		char* char_args[args.size()+1];
		{
			int selector = 0;
			for(const auto &str:args){
				char_args[selector++] = (char*)(str.c_str());
			}
			char_args[selector] = nullptr;
		}
		::execv(path.c_str(), char_args);
	}else if(pid > 0){ //main thread
		return;
	}else{
		throw_ex("can't fork");
	}
}

void launch(const std::string &path){
	launch(path, {});
}

class cursor:public imagebox{
public:
	cursor(int x, int y, uint w, uint h)
		:imagebox(w, h)
	{
		move(x, y);
		drawer d;
		sptr<bit_image> temp = std::static_pointer_cast<bit_image>(get_inner_img());
		d.draw_line(0, h/2, w-1, h/2, color::white, temp);
		d.draw_line(h/2, 0, h/2, w-1, color::white, temp);
	}
	~cursor(){}
	void move_dx(int delta){
		x += delta;
		move(x, y);
		//set_changed(true);
	}
	void move_dy(int delta){
		y += delta;
		move(x, y);
		//set_changed(true);
	}
};


class myform:public binform{
protected:
	std::vector<event> events;

	std::vector<sptr<event>> events_vec;
	std::mutex events_mutex;
	std::atomic_bool end_requested;
	fastOledDriver drv;
	drawer d;
	sptr<cursor> c;
	std::thread loop_thr;
	void draw_layer(const std::shared_ptr<layer> &l){
		const auto elements = l->get_elements();
		for(const auto &el:elements){
			d.draw_image(el->get_x(), el->get_y(),
				el->get_image(), img);
		}
	}
	void loop_func(){
		sptr<bit_image> img = this->img;
		while(!end_requested){
			events_mutex.lock();
			if(!app_launched){
				for(auto &l:reverse_wrapper(layers)){
					l->update();
					draw_layer(l);
				}
			}else{
				lib_obj->send(app_name, API_CALL::UI_ask_image);
				if(app_img){
					d.draw_image(0, 0, app_img, img);
				}
				cursor_layer->update();
				draw_layer(cursor_layer);
			}
			events_mutex.unlock();
			draw_img(img);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	std::shared_ptr<binforms::bit_image> app_img;
	std::shared_ptr<watchlib> lib_obj;
	std::shared_ptr<layer> cursor_layer;
//TODO:incapsulate
	std::atomic_bool app_launched;
	std::string app_name;
public:
	void set_app_name(std::string name){
		app_name = name;
	}

	myform(std::shared_ptr<watchlib> lib_obj, const uint w, const uint h)
		:binform(w,h)
	{
		end_requested = false;
		app_img = nullptr;
		app_launched = false;
		this->lib_obj = lib_obj;
		c = std::make_shared<cursor>(0,0,5,5);

		cursor_layer = std::make_shared<layer>(w, h);
		cursor_layer->add_element(c);
		add_layer(cursor_layer);

		auto ui_layer0 = std::make_shared<layer>(w,h);
		auto btn = std::make_shared<button>("calc");
		btn->move(0,0);
		lib_obj->set_policy("calc", send_policy::repeatedly);
		btn->bind([&](){
			events_mutex.lock();
			set_app_name("calc");
			//launch(watches_path+"/bin/"+app_name+"/"+app_name);
			app_launched = true;
			events_mutex.unlock();
		});
		ui_layer0->add_element(btn);

		auto clk = std::make_shared<form_clock>(app_h, app_h);
		clk->move(0, 0);
		ui_layer0->add_element(clk);

		add_layer(ui_layer0);

		drv.init();
	}
	~myform(){
		end_requested = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		if(loop_thr.joinable()){
			loop_thr.join();
		}
	}
	void draw(){
		//TODO: fix this monstrosity
		draw_img(img);
		const auto byte_img = binfont::to_byte_img(img);
		const auto bytes = byte_img->get_pixels();
		drv.draw((uint8_t*)&bytes[0], bytes.size());
	}
	void loop(){
		loop_thr = std::thread(&myform::loop_func, this);
	}
	void move_cursor_dx(int dx){
		c->move_dx(dx);
	}
	void move_cursor_dy(int dy){
		c->move_dy(dy);
	}
	void send_cursor_press_event(){
		const int cursor_x = c->get_x(),
		      cursor_y = c->get_y();

		if(app_launched){
			lib_obj->send(app_name, API_CALL::UI_cursor_pressed,
				{cursor_x, cursor_y, int(BINFORM_EVENTS::press)});
		}else{
			auto el = get_element(cursor_x, cursor_y);
			if(!el){
				return;
			}
			auto e = std::make_shared<event>(cursor_x, cursor_y);
			el->on_press_e(e);
		}
	}
	void request_end(){
		end_requested = true;
	}
	void set_app_img(const packet &p){
		if(!app_launched){
			return;
		}
		if(p.get_name() != app_name){
			return;
		}
		app_img = std::make_shared<bit_image>(app_w, app_h);
		std::vector<bool> pixels;
		pixels.reserve(app_w * app_h);
		std::string serialized_pixels = p.get_args()[0];
		for(const auto &p:serialized_pixels){
			pixels.emplace_back((p=='1')?true:false);
		}
		app_img->set_pixels(pixels);
	}
	std::shared_ptr<element> get_element(const int x, const int y)const override{
		for(int i=1; i < layers.size(); i++){
			const auto el = layers[i]->get_element(x,y);
			if(el){
				return el;
			}
		}
		return nullptr;
	}
};

sptr<myform> form;
sptr<watchlib> lib_obj;

void cb_key_press(const packet &p){
	const char c = p.get_args()[0][0];
	std::cout<<"cb called:"<<c<<"\n";
	if(c == 'a'){
		form->move_cursor_dx(-1);
	}else if(c == 'd'){
		form->move_cursor_dx(1);
	}else if(c == 'w'){
		form->move_cursor_dy(-1);
	}else if(c == 's'){
		form->move_cursor_dy(1);
	}else if(c == 'q'){
		form->request_end();
		lib_obj->end();
	}else if(c == 'x'){
		form->send_cursor_press_event();
	}
}

void cb_send_image(const packet &p){
	form->set_app_img(std::move(p));
}

int main(){
	lib_obj = std::make_shared<watchlib>("interface");

	form = std::make_shared<myform>(lib_obj, app_w, app_h);
	form->loop();

	lib_obj->init();
	//lib_obj->set_form(form);
	lib_obj->send_log("init succseed", API_CALL::LOG_send_info);
	lib_obj->add_callback(API_CALL::UI_key_pressed, cb_key_press);
	lib_obj->add_callback(API_CALL::UI_send_image, cb_send_image);
	lib_obj->start();
}
