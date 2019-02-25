#include <string.h>
#include "graphics.h"


using namespace graphics;

drawer::drawer():drawer(nullptr){}
drawer::drawer(const sptr<bit_image> &img)
	:img(img),color(true)
{}

void drawer::set_image(const sptr<bit_image> &img){
	this->img = img;
}
sptr<bit_image> drawer::get_image()const{
	return img;
}

void drawer::set_color(const bool color){
	this->color = color;
}
bool drawer::get_color()const{
	return color;
}

void drawer::draw_line(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2)
{ draw_line(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_line(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		bit_image* img)
{ draw_line(x1,y1,x2,y2,color,img); }

void drawer::draw_line(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const sptr<bit_image> &img)
{ draw_line(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_line(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color)
{ draw_line(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_line(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color,bit_image* img)
{
	{
		const unsigned int w = img->get_w();
		const unsigned int h = img->get_h();
		if (x1 >= w || x2 >= w ||
				y1 >= h||y2 >= h)
		{
			//TODO:trow exception
			return;
		}
	}

	unsigned int _x1 = x1;
	unsigned int _y1 = y1;

	const int dx=std::max(_x1,x2)-std::min(_x1,x2),
			dy=std::max(_y1,y2)-std::min(_y1,y2),
			sx=(x2 > _x1)?1:-1,
			sy=(y2 > _y1)?1:-1;

	int err = (dx > dy ? dx : -dy) / 2, e2;
	for (;;) {
		//img(x0,y0) = color;
		img->set_pixel(_x1,_y1,color);
		if (_x1 == x2 && _y1 == y2)	break;
		e2 = err;
		if (e2 > -dx) { err -= dy; _x1 += sx; }
		if (e2 < dy) { err += dx; _y1 += sy; }
	}
}

void drawer::draw_line(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color,const sptr<bit_image> &img)
{ draw_line(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_line(const dot &dt1,const dot &dt2)
{ draw_line(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_line(const dot &dt1,const dot &dt2,
		bit_image* img)
{ draw_line(dt1.x,dt1.y,dt2.x,dt2.y,color,img); }

void drawer::draw_line(const dot &dt1,const dot &dt2,
		const sptr<bit_image> &img)
{ draw_line(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_line(const dot &dt1,const dot &dt2,
		const bool color)
{ draw_line(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_line(const dot &dt1,const dot &dt2,
		const bool color,bit_image* img)
{ draw_line(dt1.x,dt1.y,dt2.x,dt2.y,color,img); }

void drawer::draw_line(const dot &dt1,const dot &dt2,
		const bool color,const sptr<bit_image> &img)
{ draw_line(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }


void drawer::draw_rect(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2)
{ draw_rect(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_rect(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		bit_image* img)
{ draw_rect(x1,y1,x2,y2,color,img); }

void drawer::draw_rect(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const sptr<bit_image> &img)
{ draw_rect(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_rect(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color)
{ draw_rect(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_rect(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color,bit_image* img)
{
	const unsigned int w = img->get_w();
	if(x2<x1||y2<y1){
		//TODO:throw exception
	}
	const unsigned int rect_w = x2-x1+1;
	std::vector<bool> raw_img = img->get_pixels();
	std::fill_n(raw_img.begin()+x1+y1*w,rect_w,color);
	std::fill_n(raw_img.begin()+x1+y2*w,rect_w,color);
	const unsigned int start = y1+1,
			end = y2;
	for(unsigned int y=start; y<end; y++){
		raw_img[x1+y*(rect_w)] = color;
		raw_img[x2+y*(rect_w)] = color;
	}
	img->set_pixels(raw_img);
}

void drawer::draw_rect(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color,const sptr<bit_image> &img)
{ draw_rect(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_rect(const dot &dt1,const dot &dt2)
{ draw_rect(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_rect(const dot &dt1,const dot &dt2,
					   bit_image* img)
{ draw_rect(dt1.x,dt1.y,dt2.x,dt2.y,color,img); }

void drawer::draw_rect(const dot &dt1,const dot &dt2,
		const sptr<bit_image> &img)
{ draw_rect(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_rect(const dot &dt1,const dot &dt2,
		const bool color)
{ draw_rect(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_rect(const dot &dt1,const dot &dt2,
		const bool color,bit_image* img)
{ draw_rect(dt1.x,dt1.y,dt2.x,dt2.y,color,img); }

void drawer::draw_rect(const dot &dt1,const dot &dt2,
		const bool color,const sptr<bit_image> &img)
{ draw_rect(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }


void drawer::draw_rect_filled(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2)
{ draw_rect_filled(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_rect_filled(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		bit_image* img)
{ draw_rect_filled(x1,y1,x2,y2,color,img); }

void drawer::draw_rect_filled(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const sptr<bit_image> &img)
{ draw_rect_filled(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_rect_filled(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color)
{ draw_rect_filled(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_rect_filled(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color,bit_image* img)
{
	const unsigned int w = img->get_w();
	if(x2<x1||y2<y1){
		//TODO:throw exception
	}
	const unsigned int rect_w = x2-x1;
	std::vector<bool> raw_img = img->get_pixels();
	for(unsigned int y=y1; y<y2; y++){
		std::fill_n(raw_img.begin()+x1+y*w,rect_w,color);
	}
	img->set_pixels(raw_img);
}

void drawer::draw_rect_filled(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color,const sptr<bit_image> &img)
{ draw_rect_filled(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_rect_filled(const dot &dt1,const dot &dt2)
{ draw_rect_filled(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_rect_filled(const dot &dt1,const dot &dt2,
		bit_image* img)
{ draw_rect_filled(dt1.x,dt1.y,dt2.x,dt2.y,color,img); }

void drawer::draw_rect_filled(const dot &dt1,const dot &dt2,
		const sptr<bit_image> &img)
{ draw_rect_filled(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_rect_filled(const dot &dt1,const dot &dt2,
		const bool color)
{ draw_rect_filled(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_rect_filled(const dot &dt1,const dot &dt2,
		const bool color,bit_image* img)
{ draw_rect_filled(dt1.x,dt1.y,dt2.x,dt2.y,color,img); }

void drawer::draw_rect_filled(const dot &dt1,const dot &dt2,
		const bool color,const sptr<bit_image> &img)
{ draw_rect_filled(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_ellipse(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2)
{ draw_ellipse(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_ellipse(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		bit_image* img)
{ draw_ellipse(x1,y1,x2,y2,color,img); }

void drawer::draw_ellipse(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const sptr<bit_image> &img)
{ draw_ellipse(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_ellipse(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color)
{ draw_ellipse(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_ellipse(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color,bit_image* img)
{ }

void drawer::draw_ellipse(const unsigned int x1,const unsigned int y1,
		const unsigned int x2,const unsigned int y2,
		const bool color,const sptr<bit_image> &img)
{ draw_ellipse(x1,y1,x2,y2,color,img.get()); }

void drawer::draw_ellipse(const dot &dt1,const dot &dt2)
{ draw_ellipse(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_ellipse(const dot &dt1,const dot &dt2,
		bit_image* img)
{ draw_ellipse(dt1.x,dt1.y,dt2.x,dt2.y,color,img); }

void drawer::draw_ellipse(const dot &dt1,const dot &dt2,
		const sptr<bit_image> &img)
{ draw_ellipse(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_ellipse(const dot &dt1,const dot &dt2,
		const bool color)
{ draw_ellipse(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_ellipse(const dot &dt1,const dot &dt2,
		const bool color,bit_image* img)
{ draw_ellipse(dt1.x,dt1.y,dt2.x,dt2.y,color,img); }

void drawer::draw_ellipse(const dot &dt1,const dot &dt2,
		const bool color,const sptr<bit_image> &img)
{ draw_ellipse(dt1.x,dt1.y,dt2.x,dt2.y,color,img.get()); }

void drawer::draw_image(const unsigned int x1,const unsigned int y1,
		bit_image* img)
{
	std::vector<bool> src = img->get_pixels();
	const unsigned int srcw = img->get_w(),
			srch = img->get_h();
	std::vector<bool> dst = this->img->get_pixels();
	const unsigned int dstw = this->img->get_w();
	const unsigned int dst_start = x1 + y1*dstw;
	for(unsigned int i=0; i<srch; i++){
		std::copy(src.begin()+i*srcw,
				  src.begin()+(i+1)*srcw,
				  dst.begin()+dst_start+i*dstw);
	}
	this->img->set_pixels(dst);
}
void drawer::draw_image(const unsigned int x1,const unsigned int y1,
		const sptr<bit_image> &img)
{ draw_image(x1,y1,img.get()); }

void drawer::draw_image(const dot &dt1, bit_image* img)
{ draw_image(dt1.x,dt1.y,img); }

void drawer::draw_image(const dot &dt1,const sptr<bit_image> &img)
{ draw_image(dt1.x,dt1.y,img.get()); }


void drawer::fill_image(const bool color)
{ fill_image(color, img.get()); }

void drawer::fill_image(const bool color,bit_image* img){
	std::vector<bool> src = img->get_pixels();
	std::fill_n(src.begin(), src.size(), color);
	img->set_pixels(src);
}

void drawer::fill_image(const bool color,sptr<bit_image> &img)
{ fill_image(color, img.get()); }
