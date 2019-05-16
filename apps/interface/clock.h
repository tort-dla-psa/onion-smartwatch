#include <thread>
#include <atomic>
#include <mutex>
#include <cmath>
#include <algorithm>
#include "imagebox.h"


float percent(int var, int percent){
	return double(var)/100.0*percent;
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
			const float pi = 3.1415;
			graphics::dot d1 = {(int)(x + w/2*std::cos(rad-pi/4)),
					(int)(y + w/2*std::sin(rad-pi/4))},
				d2 = {(int)(x + w/2*std::cos(rad+pi/4)),
					(int)(y + w/2*std::sin(rad+pi/4))},
				d3 = {(int)(d1.x + h*std::cos(rad)),
					(int)(d1.y + h*std::sin(rad))},
				d4 = {(int)(d2.x + h*std::cos(rad)),
					(int)(d2.y + h*std::sin(rad))};
			dr.draw_line(d1, d2, img);
			dr.draw_line(d2, d4, img);
			dr.draw_line(d1, d3, img);
			dr.draw_line(d3, d4, img);
		}
	};
	arrow *H,*M,*S;
	sptr<std::thread> update_thr;
	std::atomic_bool end_requested;
	std::mutex draw_mt;
public:
	form_clock(unsigned int w, unsigned int h):imagebox(w,h){
		H = new arrow(w/2, h/2, 
			std::max((int)percent(h,10), 3),
			(int)percent(h/2,50));
		M = new arrow(w/2, h/2, 
			std::max((int)percent(h,8), 2),
			(int)percent(h/2,70));
		S = new arrow(w/2, h/2, 
			std::max((int)percent(h,5), 1),
			(int)percent(h/2,80));
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
			//set_changed(true);
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
