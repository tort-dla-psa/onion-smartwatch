#include "element.h"
#include "image.h"

element::element(const unsigned int w, const unsigned int h)
	:img(new bit_image(w,h))
{
	move(0,0);
}

element::~element(){}

void element::set_changed(const bool status){
	this->changed = status;
}
bool element::get_changed() const{
	return changed;
}
void element::move(const unsigned int x, const unsigned int y){
	this->x = x;
	this->y = y;
	set_changed(true);
}
unsigned int element::get_x()const{
	return x;
}
unsigned int element::get_y()const{
	return y;
}
unsigned int element::get_w()const{
	return img->get_w();
}
unsigned int element::get_h()const{
	return img->get_h();
}
sptr<bit_image> element::get_image() const{
	return img;
}
