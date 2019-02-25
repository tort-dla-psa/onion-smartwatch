#include "label.h"
#include "binfont.h"

#define label_def_txt "label"

label::label()
	:label(label_def_txt)
{}

label::label(const std::string &text)
	:element (binfont::get_pixel_width(text),
			  binfont::get_pixel_height(text)),
	  text(text)
{
	bit_image* temp = binfont::text_to_bit_img(text);
	img = std::shared_ptr<bit_image>(temp);
}

label::label(const char *text)
	:label(std::string(text))
{}

void label::set_text(const std::string &text){
	this->text = text;
	set_changed(true);
}

std::string label::get_text()const{
	return text;
}

void label::move(const unsigned int x,
				 const unsigned int y)
{
	this->x = x;
	this->y = y;
	set_changed(true);
}

void label::update(){
	bit_image* temp = binfont::text_to_bit_img(text);
	img = std::shared_ptr<bit_image>(temp);
}
