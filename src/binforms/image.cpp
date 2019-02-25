#include <algorithm>
#include "image.h"

image::image(const unsigned int w, const unsigned int h)
	:w(w),h(h)
{}
image::~image(){}
unsigned int image::get_w()const{
	return w;
}
unsigned int image::get_h()const{
	return h;
}


bit_image::bit_image(const unsigned int w, const unsigned int h)
	:image(w,h)
{
	img.resize(w*h,false);
}
bit_image::bit_image(const unsigned int w, const unsigned int h,
		const bool* img)
	:bit_image(w,h)
{
	const unsigned int size = w*h;
	for(unsigned int i=0; i<size; i++){
		this->img[i] = img[i];
	}
}
bit_image::~bit_image(){}
bool bit_image::get_pixel(const unsigned int x, const unsigned int y)const{
	return img[x+y*w];
}
std::vector<bool> bit_image::get_pixels() const{
	return img;
}
void bit_image::set_pixel(const unsigned int x, const unsigned int y,
		const bool pixel)
{
	this->img[x+y*w] = pixel;
}
void bit_image::set_pixels(const std::vector<bool> &img){
	this->img = img;
}
void bit_image::resize(const unsigned int w, const unsigned int h){
	std::vector<bool> new_img_pixels;
	new_img_pixels.resize(w*h, false);
	const unsigned int h_size = (h < this->h)? h : this->h;
	const unsigned int w_size = (w < this->w)? w : this->w;
	for(unsigned int y=0; y<h_size; y++){
		std::copy(this->img.begin() + y * this->w,
				this->img.begin() + (y + 1) * w_size,
				new_img_pixels.begin() + y * w);
	}
	this->w = w;
	this->h = h;
	this->img = new_img_pixels;
}


byte_image::byte_image(const unsigned int w, const unsigned int h)
	:image(w,h)
{
	const unsigned int size = w*h/sizeof(char);
	img.reserve(size);
	std::fill_n(img.begin(),size,0x00);
}
byte_image::byte_image(const unsigned int w, const unsigned int h,
		const char* img)
	:image(w,h)
{
	const unsigned int size = w*h/8;
	this->img.reserve(size);
	for(unsigned int i=0; i<size; i++){
		this->img[i] = img[i];
	}
}
byte_image::~byte_image(){}
bool byte_image::get_pixel(const unsigned int x, const unsigned int y)const{
	const int selector = 1<<y%8;
	return img[x+y/8*w]&selector;
}
std::vector<char> byte_image::get_pixels()const{
	return img;
}
void byte_image::set_pixel(const unsigned int x, const unsigned int y,
		const bool pixel)
{
	const int selector = 1<<(y%sizeof(char));
	const unsigned int coord = x+y/sizeof(char)*w;
	if(pixel){
		img[coord] |= selector;
	}else{
		img[coord] &= ~selector;
	}
}
void byte_image::set_pixels(const std::vector<char> &img){
	this->img = img;
}
void byte_image::resize(const unsigned int w, const unsigned int h){
	//TODO:implement
}
