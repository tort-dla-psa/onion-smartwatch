#ifndef IMAGE_H
#define IMAGE_H
#include <vector>
#include "aliases.h"

class image{
protected:
	unsigned int w,h;
public:
	image(const unsigned int w,const unsigned int h);
	virtual ~image();
	virtual bool get_pixel(const unsigned int x,const unsigned int y)const=0;
	virtual void set_pixel(const unsigned int x,const unsigned int y,
			const bool pixel)=0;
	unsigned int get_w()const;
	unsigned int get_h()const;
	virtual void resize(const unsigned int w,const unsigned int h)=0;
};

class bit_image:public image{
	std::vector<bool> img;
public:
	bit_image(const unsigned int w,const unsigned int h);
	bit_image(const unsigned int w,const unsigned int h,
			const bool* img);
	~bit_image();
	bool get_pixel(const unsigned int x,const unsigned int y)const override;
	std::vector<bool> get_pixels()const;
	void set_pixel(const unsigned int x,const unsigned int y,
			const bool pixel)override;
	void set_pixels(const std::vector<bool> &img);
	void resize(const unsigned int w,const unsigned int h)override;
};

class byte_image:public image{
	std::vector<char> img;
public:
	byte_image(const unsigned int w,const unsigned int h);
	byte_image(const unsigned int w,const unsigned int h,
			const char* img);
	~byte_image();
	bool get_pixel(const unsigned int x,const unsigned int y)const override;
	std::vector<char> get_pixels()const;
	void set_pixel(const unsigned int x,const unsigned int y,
			const bool pixel)override;
	void set_pixels(const std::vector<char> &img);
	void resize(const unsigned int w,const unsigned int h)override;
};

#endif // IMAGE_H
