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

inline void draw_img(image* img){
	const uint w = img->get_w();
	const uint h = img->get_h();

	for(uint i=0; i<h; i++){
		for(uint j=0; j<w; j++){
			std::cout<<(img->get_pixel(j,i)? '#':'_');
		}
		std::cout<<'\n';
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
		dup2(fd, 1);
		dup2(fd, 2);
		char** char_args = new char*[args.size()+1];
		{
			int selector = 0;
			for(const auto &str:args){
				char_args[selector++] = (char*)(str.c_str());
			}
			char_args = nullptr;
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
	int prev_x, prev_y;
	sptr<bit_image> clear_img;
public:
	cursor(int x, int y, uint w, uint h)
		:imagebox(w, h)
	{
		move(x, y);
		drawer d;
		sptr<bit_image> temp = std::static_pointer_cast<bit_image>(get_inner_img());
		d.draw_line(0, h/2, w-1, h/2, color::white, temp);
		d.draw_line(h/2, 0, h/2, w-1, color::white, temp);
		clear_img = std::make_shared<bit_image>(w,h);
	}
	~cursor(){}
	void move_dx(int delta){
		prev_x = x;
		x += delta;
		move(x, y);
		//set_changed(true);
	}
	void move_dy(int delta){
		prev_y = y;
		y += delta;
		move(x, y);
		//set_changed(true);
	}
	int get_prev_x(){
		return prev_x;
	}
	int get_prev_y(){
		return prev_y;
	}
	sptr<bit_image> get_clear_img(){
		return clear_img;
	}
};


class myform:public binform{
	std::vector<event> events;

	std::vector<sptr<event>> events_vec;
	std::mutex events_mutex;
	std::atomic_bool end_requested;
	fastOledDriver drv;
	drawer d;
	sptr<cursor> c;
	void clear_cursor(){
		d.draw_image(c->get_prev_x() - c->get_w()/2,
			c->get_prev_y() - c->get_h()/2,
			c->get_clear_img(), img);
	}
	void draw_cursor(){
		d.draw_image(c->get_x() - c->get_w()/2,
			c->get_y() - c->get_h()/2,
			c->get_image(), img);
	}
	std::thread loop_thr;
	void loop_func(){
		sptr<bit_image> img = this->img;
		while(!end_requested){
			events_mutex.lock();
			for(auto &l:reverse_wrapper(layers)){
				l->update();
				const auto elements = l->get_elements();
				for(const auto &el:elements){
					d.draw_image(el->get_x(), el->get_y(),
						el->get_image(), img);
				}
			}
			events_mutex.unlock();
			draw_img(this->img.get());
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
public:
	myform(const uint w, const uint h)
		:binform(w,h)
	{
		end_requested = false;
		c = std::make_shared<cursor>(0,0,5,5);

		sptr<layer> cursor_layer(new layer(w, h));
		cursor_layer->add_element(c);
		add_layer(cursor_layer);

		sptr<layer> ui_layer0(new layer(w,h));
		sptr<form_clock> clk(new form_clock(app_h, app_h));
		clk->move(0, 0);
		ui_layer0->add_element(clk);

		sptr<label> lbl(new label("hi"));
		ui_layer0->add_element(lbl);
		lbl->move(w - lbl->get_w(), 0);
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
		draw_img(this->img.get());
		const auto byte_img = binfont::to_byte_img(this->img);
		const auto bytes = byte_img->get_pixels();
		drv.draw((uint8_t*)&bytes[0], bytes.size());
	}
	void loop(){
		loop_thr = std::thread(&myform::loop_func, this);
	}
	void move_cursor_dx(int dx){
		clear_cursor();
		c->move_dx(dx);
		draw_cursor();
	}
	void move_cursor_dy(int dy){
		clear_cursor();
		c->move_dy(dy);
		draw_cursor();
	}
	void request_end(){
		end_requested = true;
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
	}
}

int main(){
	form = sptr<myform>(new myform(app_w, app_h));

	//launch(watches_path+"bin/companion-server/companion-server");
	form->loop();

	lib_obj = std::make_shared<watchlib>("interface");
	lib_obj->init();
	lib_obj->send_log("init succseed", API_CALL::LOG_send_info);
	lib_obj->add_callback(API_CALL::UI_key_pressed, cb_key_press);
	lib_obj->start();
}
