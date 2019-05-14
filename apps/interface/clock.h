#include <thread>
#include <atomic>
#include <mutex>
#include <cmath>
#include "imagebox.h"

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
