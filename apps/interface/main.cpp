#include <iostream>
#include "binform.h"
#include "stack.h"
#include "label.h"
#include "imagebox.h"
#include "watchlib.h"

#define app_w 128
#define app_h 64

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
	myform form(app_w, app_h);
/*
	sptr<v_stack> st1(new v_stack());
	sptr<label> lbl1(new label("lbl1"));
	sptr<label> lbl2(new label("lbl2"));
	sptr<label> lbl3(new label("label3"));
	st1->add_element(lbl1);
	st1->add_element(lbl2);
	st1->add_element(lbl3);
	st1->update();
	//draw_img(st1->get_image().get());
	form.add_element(st1);
	form.update();
	form.draw();
*/
//	sptr<imagebox>
}
