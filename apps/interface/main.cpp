#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdexcept>
#include <ctime>
#include <cmath>
#include "API_CALLS.h"
#include "data_protocol.h"
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

#define app_w 70
#define app_h 20

using namespace watches;
using namespace IO;
const std::string watches_path("/home/tort/gits/onion-smartwatch/");

std::vector<sptr<std::thread>> children;

void throw_ex(const std::string &mes){
	const std::string fullmes = "interface: exception: " + mes + "\n";
	throw std::runtime_error(fullmes);
}

inline void draw_img(image* img){
	const unsigned int w = img->get_w();
	const unsigned int h = img->get_h();

	for(unsigned int i=0; i<h; i++){
		for(unsigned int j=0; j<w; j++){
			std::cout<<(img->get_pixel(j,i)? '#':'_');
		}
		std::cout<<'\n';
	}
}

void exec_func_args(const std::string &path, const std::vector<std::string> &args){
	pid_t pid = fork();
	if(pid == 0){
		char** char_args = new char*[args.size()+1];
		{
			int selector = 0;
			for(const auto &str:args){
				char_args[selector++] = (char*)(str.c_str());
			}
			char_args = nullptr;
		}
		::execv(path.c_str(), char_args);
	}else if(pid > 0){
		return;
	}else{
		throw_ex("can't fork");
	}
}
void exec_func(const std::string &path){
	pid_t pid = fork();
	if(pid == 0){ //child
		//no output to stdout
		int fd = open("/dev/null", O_WRONLY);
		dup2(fd, 1);
		dup2(fd, 2);
		char** args = new char*[1];
		args[0] = nullptr;
		::execv(path.c_str(), args);
	}else if(pid > 0){ //main thread
		return;
	}else{
		throw_ex("can't fork");
	}
}

void launch(const std::string &path){
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
	exec_func(path);
}

class form_clock:public imagebox{
	struct arrow{
		int h, w;
		int x, y;
		float rad = 0;
		void update(int cur, int max){
			const float pi = 3.1415;
			rad = pi/-2.0 + 2.0*pi / max * cur;
		}
		arrow(int x, int y, int w, int h):x(x),y(y),w(w),h(h){}
		void draw(graphics::drawer &dr, sptr<bit_image> &img){
			dr.draw_line(x, y, 
				x + h * std::cos(rad),
				y + h * std::sin(rad),
				img);
		}
	};
	arrow *H,*M,*S;
	sptr<std::thread> update_thr;
	std::atomic_bool end_requested;
	std::mutex draw_mt;
public:
	form_clock(unsigned int w, unsigned int h):imagebox(w,h){
		H = new arrow((int)w/2, (int)h/2, 3, 6);
		M = new arrow((int)w/2, (int)h/2, 2, 7);
		S = new arrow((int)w/2, (int)h/2, 1, 8);
		end_requested = false;
		update_thr = std::make_shared<std::thread>(&form_clock::update_time, this);
	}
	~form_clock(){
		end_requested = true;
		if(update_thr->joinable())
			update_thr->join();
		delete H;
		delete M;
		delete S;
	}
	void update_time(){
		graphics::drawer dr;
		sptr<bit_image> temp_img = 
			std::static_pointer_cast<bit_image>(get_inner_img());
		while(!end_requested){
			time_t now = time(0);
			tm *ltm = localtime(&now);
			H->update(ltm->tm_hour%12, 12);
			M->update(ltm->tm_min, 60);
			S->update(ltm->tm_sec, 60);
			draw_mt.lock();
			dr.fill_image(false, temp_img);
			S->draw(dr, temp_img);
			M->draw(dr, temp_img);
			H->draw(dr, temp_img);
			set_changed(true);
			draw_mt.unlock();
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	void update(){
		draw_mt.lock();
		imagebox::update();
		draw_mt.unlock();
	}
};

class cursor:public imagebox{
	int prev_x, prev_y;
	sptr<bit_image> clear_img;
public:
	cursor(int x, int y, unsigned int w, unsigned int h)
		:imagebox(w, h)
	{
		move(x, y);
		drawer d;
		sptr<bit_image> temp = std::static_pointer_cast<bit_image>(get_inner_img());
		d.draw_line(0,h/2,w-1,h/2,temp);
		d.draw_line(h/2,0,h/2,w-1,temp);
		clear_img = std::make_shared<bit_image>(w,h);
	}
	~cursor(){}
	void move_dx(int delta){
		prev_x = x;
		x += delta;
		move(x, y);
		set_changed(true);
	}
	void move_dy(int delta){
		prev_y = y;
		y += delta;
		move(x, y);
		set_changed(true);
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
			bool redraw = false;
			events_mutex.lock();
			const int c_x = c->get_x(),
			      c_y = c->get_y(),
			      c_x2 = c->get_x() + c->get_w(),
			      c_y2 = c->get_y() + c->get_h();
			if(c->get_changed()){
				clear_cursor();
			}
			for(const auto &form_el:binform::elements){
				const int f_el_x = form_el->get_x();
				const int f_el_y = form_el->get_y();
				const int f_el_x2 = f_el_x + form_el->get_w();
				const int f_el_y2 = f_el_y + form_el->get_h();
				if(f_el_x > c_x2 ||
					f_el_y > c_y2 ||
					f_el_x2 < c_x ||
					f_el_y2 < c_y)
				{
					if(form_el->get_changed()){
						form_el->update();
						d.draw_image(f_el_x, f_el_y,
							form_el->get_image(), img);
						redraw = true;
					}
					form_el->set_changed(false);
					continue;
				}
				if(form_el->get_changed() | c->get_changed()){
					form_el->update();
					d.draw_image(f_el_x, f_el_y,
						form_el->get_image(), img);
					redraw = true;
					form_el->set_changed(false);
				}
			}
			if(c->get_changed()){
				draw_cursor();
				redraw = true;
				c->set_changed(false);
			}
			events_mutex.unlock();
			if(redraw){
				draw_img(this->img.get());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
public:
	myform(const unsigned int w,
		const unsigned int h)
		:binform(w,h)
	{
		end_requested = false;
		drawer d;
		c = std::make_shared<cursor>(0,0,5,5);
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
		byte_image* byte_img = binfont::bit_img_to_byte_img(this->img.get());
		const std::vector<char> bytes = byte_img->get_pixels();
		char* begin = (char*)&bytes[0];
		drv.draw((uint8_t*)begin, bytes.size());
		delete byte_img;
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
	void cb_key_press(const packet &p){
		std::cout<<"cb called\n";
		events_mutex.lock();
		const char c = p.get_args()[0][0];
		if(c == 'a'){
			move_cursor_dx(-1);
		}else if(c == 'd'){
			move_cursor_dx(1);
		}else if(c == 'w'){
			move_cursor_dy(-1);
		}else if(c == 's'){
			move_cursor_dy(1);
		}else if(c == 'q'){
			request_end();
		}
		events_mutex.unlock();
	}
};

int main(){
	sptr<myform> form = sptr<myform>(new myform(app_w, app_h));
	//lib_obj.set_form(form);

	sptr<form_clock> clk(new form_clock(20, 20));
	form->add_element(clk);
	clk->move(0, 0);
	sptr<label> lbl(new label("hi"));
	form->add_element(lbl);
	lbl->move(clk->get_x() + clk->get_w() + 1, 0);
	//launch(watches_path+"bin/companion-server/companion-server");
	//form->loop();

	watchlib lib_obj("interface");
	lib_obj.init();
	lib_obj.send_log("init succseed", API_CALL::LOG_send_info);
	lib_obj.add_callback(API_CALL::UI_key_pressed, &myform::cb_key_press, form);
	lib_obj.start();
}
