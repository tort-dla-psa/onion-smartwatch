#include "button.h"
#include "binfont.h"

#define button_def_text "btn"

button::button()
	:button (std::string(button_def_text))
{}
button::button(const unsigned int w,
			   const unsigned int h)
	:button (w,h,
			 std::string(button_def_text))
{}
button::button(const unsigned int w,
			   const unsigned int h,
			   const label &lbl)
	:button (w,h,lbl.get_text())
{}
button::button(const label &lbl)
	:button(lbl.get_text())
{}
button::button(const unsigned int w,
			   const unsigned int h,
			   const sptr<label> &lbl)
	:button (w,h,lbl->get_text())
{}
button::button(const sptr<label> &lbl)
	:button(lbl->get_text())
{}
button::button(const unsigned int w,
			   const unsigned int h,
			   const std::string &text)
	:element (w,h),imovable (),iresizable ()
{
	drwr = std::unique_ptr<drawer>(new drawer(img));
	set_label(text);
	update();
}
button::button(const std::string &text)
	:button(binfont::get_pixel_width(text),
			binfont::get_pixel_height(text),
			text)
{}
button::~button(){};

void button::set_label(const std::string &text){
	lbl = std::make_shared<label>(text);
	resize(img->get_w(),img->get_h());
	set_changed(true);
}
void button::set_label(const sptr<label> &lbl){
	set_label(lbl->get_text());
}

void button::move(const unsigned int x,
				  const unsigned int y)
{
	this->x = x;
	this->y = y;
	set_changed(true);
}
void button::resize(const unsigned int w,
					const unsigned int h)
{
	img = std::make_shared<bit_image>(w,h);
	drwr->set_image(img);
	const std::string txt = lbl->get_text();
	const unsigned int text_w = binfont::get_pixel_width(txt),
			text_h = binfont::get_pixel_height(txt);

	const int x = (text_w < w)? (w - text_w)/2 : 0,
			y = (text_h < h)? (h - text_h)/2 :0;
	lbl->move(x,y);
	set_changed(true);
}

void button::update(){
	const unsigned int w = get_w();
	const unsigned int h = get_h();
	drwr->draw_image(lbl->get_x(),lbl->get_y(),
					 lbl->get_image());
	drwr->draw_rect(0,0,w-1,h-1);
}
