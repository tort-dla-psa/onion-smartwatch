#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "image.h"

namespace graphics{
struct dot{
	unsigned int x,y;
};

class drawer{
	sptr<bit_image> img;
	bool color;
public:
	drawer();
	drawer(const sptr<bit_image> &img);

	void set_image(const sptr<bit_image> &img);
	sptr<bit_image> get_image()const;

	void set_color(const bool color);
	bool get_color()const;

	void draw_line(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2);
	void draw_line(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			bit_image* img);
	void draw_line(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const sptr<bit_image> &img);
	void draw_line(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color);
	void draw_line(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color,bit_image* img);
	void draw_line(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color,const sptr<bit_image> &img);
	void draw_line(const dot &dt1,const dot &dt2);
	void draw_line(const dot &dt1,const dot &dt2,
			bit_image* img);
	void draw_line(const dot &dt1,const dot &dt2,
			const sptr<bit_image> &img);
	void draw_line(const dot &dt1,const dot &dt2,
			const bool color);
	void draw_line(const dot &dt1,const dot &dt2,
			const bool color,bit_image* img);
	void draw_line(const dot &dt1,const dot &dt2,
			const bool color,const sptr<bit_image> &img);

	void draw_rect(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2);
	void draw_rect(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			bit_image* img);
	void draw_rect(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const sptr<bit_image> &img);
	void draw_rect(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color);
	void draw_rect(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color,bit_image* img);
	void draw_rect(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color,const sptr<bit_image> &img);
	void draw_rect(const dot &dt1,const dot &dt2);
	void draw_rect(const dot &dt1,const dot &dt2,
			bit_image* img);
	void draw_rect(const dot &dt1,const dot &dt2,
			const sptr<bit_image> &img);
	void draw_rect(const dot &dt1,const dot &dt2,
			const bool color);
	void draw_rect(const dot &dt1,const dot &dt2,
			const bool color,bit_image* img);
	void draw_rect(const dot &dt1,const dot &dt2,
			const bool color,const sptr<bit_image> &img);

	void draw_rect_filled(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2);
	void draw_rect_filled(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			bit_image* img);
	void draw_rect_filled(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const sptr<bit_image> &img);
	void draw_rect_filled(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color);
	void draw_rect_filled(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color,bit_image* img);
	void draw_rect_filled(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color,const sptr<bit_image> &img);
	void draw_rect_filled(const dot &dt1,const dot &dt2);
	void draw_rect_filled(const dot &dt1,const dot &dt2,
			bit_image* img);
	void draw_rect_filled(const dot &dt1,const dot &dt2,
			const sptr<bit_image> &img);
	void draw_rect_filled(const dot &dt1,const dot &dt2,
			const bool color);
	void draw_rect_filled(const dot &dt1,const dot &dt2,
			const bool color,bit_image* img);
	void draw_rect_filled(const dot &dt1,const dot &dt2,const bool color,
			const sptr<bit_image> &img);

	void draw_ellipse(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2);
	void draw_ellipse(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			bit_image* img);
	void draw_ellipse(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const sptr<bit_image> &img);
	void draw_ellipse(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color);
	void draw_ellipse(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color,bit_image* img);
	void draw_ellipse(const unsigned int x1,const unsigned int y1,
			const unsigned int x2,const unsigned int y2,
			const bool color,const sptr<bit_image> &img);
	void draw_ellipse(const dot &dt1,const dot &dt2);
	void draw_ellipse(const dot &dt1,const dot &dt2,
			bit_image* img);
	void draw_ellipse(const dot &dt1,const dot &dt2,
			const sptr<bit_image> &img);
	void draw_ellipse(const dot &dt1,const dot &dt2,
			const bool color);
	void draw_ellipse(const dot &dt1,const dot &dt2,
			const bool color,bit_image* img);
	void draw_ellipse(const dot &dt1,const dot &dt2,
			const bool color,const sptr<bit_image> &img);

	void draw_image(const unsigned int x1,const unsigned int y1,
			bit_image* img);
	void draw_image(const dot &dt1, bit_image* img);
	void draw_image(const unsigned int x1,const unsigned int y1,
			const sptr<bit_image> &img);
	void draw_image(const dot &dt1, const sptr<bit_image> &img);

	void fill_image(const bool color);
	void fill_image(const bool color,bit_image* img);
	void fill_image(const bool color,sptr<bit_image> &img);
};
}
#endif // GRAPHICS_H
