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
#include "file_op.h"
#include "file.h"
#include "binform.h"
#include "graphics.h"
#include "stack.h"
#include "label.h"
#include "event.h"
#include "imagebox.h"
#include "watchlib.h"

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
	if(pid == 0){
		int fd = open("/dev/null", O_WRONLY);
		dup2(fd, 1);
		dup2(fd, 2);
		char** args = new char*[1];
		args[0] = nullptr;
		::execv(path.c_str(), args);
	}else if(pid > 0){
		return;
	}else{
		throw_ex("can't fork");
	}
}

void launch(std::string path){
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


class myform:public binform{
	std::vector<event> events;
	struct cursor{
		int x=0, y=0;
		int prev_x=0, prev_y=0;
		int w=5, h=5;
		bool changed = true;
		bit_image img = bit_image(w,h);
	}c;

	std::vector<sptr<event>> events_vec;
	std::mutex events_mutex;
	std::atomic_bool end_requested;
public:
	myform(const unsigned int w,
		const unsigned int h)
		:binform(w,h)
	{
		end_requested = false;
		drawer d;
		d.draw_line(0,c.h/2,c.w-1,c.h/2,&c.img);
		d.draw_line(c.h/2,0,c.h/2,c.w-1,&c.img);
	}
	~myform(){
	}
	void draw()const{
		draw_img(this->img.get());
	}
	void loop(){
		drawer d;
		bit_image clear_cursor_img = bit_image(c.w, c.h);
		d.fill_image(false, &clear_cursor_img);
		sptr<bit_image> img = this->img;
		while(!end_requested){
			events_mutex.lock();
			for(const auto &form_el:binform::elements){
				const int f_el_x = form_el->get_x();
				const int f_el_y = form_el->get_y();
				const int f_el_x2 = f_el_x + form_el->get_w();
				const int f_el_y2 = f_el_y + form_el->get_h();
				if(f_el_x > c.x + c.w ||
					f_el_y > c.y + c.h ||
					f_el_x2 < c.x ||
					f_el_y2 < c.y)
				{
					if(form_el->get_changed()){
						form_el->update();
						d.draw_image(f_el_x, f_el_y,
							form_el->get_image(), img);
					}
					form_el->set_changed(false);
					continue;
				}
				if(form_el->get_changed() || c.changed){
					form_el->update();
					d.draw_image(f_el_x, f_el_y,
						form_el->get_image(), img);
					form_el->set_changed(false);
				}
			}
			if(c.changed){
				d.draw_image(c.prev_x - c.w/2,
					c.prev_y - c.h/2,
					&clear_cursor_img, img.get());
				d.draw_image(c.x - c.w/2, c.y- c.h/2,
					&c.img, img.get());
				c.changed = false;
			}
			events_mutex.unlock();
			draw_img(this->img.get());
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	void move_cursor_dx(int dx){
		events_mutex.lock();
		c.prev_x = c.x;
		c.x += dx;
		c.changed = true;
		events_mutex.unlock();
	}
	void move_cursor_dy(int dy){
		events_mutex.lock();
		c.prev_y = c.y;
		c.y += dy;
		c.changed = true;
		events_mutex.unlock();
	}
	void request_end(){
		end_requested = true;
	}
};

sptr<myform> form;

void cb_key_press(const packet p){
	const char c = p.get_args()[0][0];
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
	}
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
		delete H, M, S;
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

int main(){
	form = sptr<myform>(new myform(app_w, app_h));
	watchlib lib_obj;
	lib_obj.init();
	lib_obj.set_form(form);

	sptr<form_clock> clk(new form_clock(20, 20));
	form->add_element(clk);
	clk->move(0, 0);
	sptr<label> lbl(new label("hi"));
	form->add_element(lbl);
	lbl->move(clk->get_x() + clk->get_w() + 1, 0);
	lib_obj.add_callback(API_CALL::UI_key_pressed, cb_key_press);
	launch(watches_path+"bin/companion-server/companion-server");
	form->loop();
}
