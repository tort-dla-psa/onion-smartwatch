#include <iostream>
#include "binform.h"
#include "stack.h"
#include "label.h"
#include "imagebox.h"
#include "watchlib.h"

#define app_w 70
#define app_h 10

using namespace watches;

inline void draw_img(image* img){
	const unsigned int w = img->get_w();
	const unsigned int h = img->get_h();

	for(unsigned int i=0; i<h; i++){
		for(unsigned int j=0; j<w; j++){
			std::cout<<img->get_pixel(j,i)? '#':'_';
		}
		std::cout<<'\n';
	}
}

class myform:public binform{
public:
	myform(const unsigned int w,
		const unsigned int h)
		:binform(w,h)
	{}
	void draw()const{
		draw_img(this->img.get());
	}
};

int main(){
	sptr<myform> form(new myform(app_w, app_h));
	watchlib lib_obj;
	lib_obj.init();
	lib_obj.set_form(form);
	//simulating some work
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
