#include "imagebox.h"
#include "graphics.h"

using namespace graphics;

imagebox::imagebox(const unsigned int w,
				   const unsigned int h)
	:imagebox (w,h,this->img)
{}

imagebox::imagebox(const unsigned int w,
				   const unsigned int h,
				   const sptr<bit_image> &img)
	:element (w,h),
	  imovable ()
{
	this->inner_img = img;
	drwr = std::unique_ptr<drawer>(new drawer(this->img));
}

imagebox::~imagebox(){}

void imagebox::set_image(const sptr<bit_image> &img){
	this->inner_img = img;
	set_changed(true);
}
sptr<bit_image> imagebox::get_inner_img(){
	return inner_img;
}

void imagebox::update(){
	drwr->draw_image(0,0,inner_img);
	set_changed(true);
}

void imagebox::move(const unsigned int x,
					const unsigned int y)
{
	this->x = x;
	this->y = y;
	set_changed(true);
}
