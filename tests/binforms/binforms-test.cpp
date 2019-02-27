//#include <QCoreApplication>
#include <iostream>
#include <ctime>
#include "binform.h"
#include "label.h"
#include "button.h"
#include "binfont.h"
#include "imagebox.h"

class mybutton:public button{
	sptr<drawer> inner_drwr;
public:
	mybutton(const sptr<drawer> &newdrwr)
		:button()
	{
		this->inner_drwr = newdrwr;
	}
	~mybutton(){}
	void press_e()override{
		std::cout<<"pressed\n";
		this->resize(get_w()*1.5,
					 get_h()*1.3);
		const unsigned int img_w = inner_drwr->get_image()->get_w(),
				img_h = inner_drwr->get_image()->get_h();
		inner_drwr->draw_line(0,0,img_w-1,img_h-1);
		inner_drwr->draw_rect(0,0,img_w-1,img_h-1);
	}
};

struct HexCharStruct
{
	unsigned char c;
	HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
	return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
	return HexCharStruct(_c);
}

void draw(image* img){
	const unsigned int w = img->get_w(),
			h = img->get_h();
	for (int i=0; i<h; i++) {
		for (int j=0; j<w; j++) {
			std::cout<<(img->get_pixel(j,i)?'I':'_');
		}
		std::cout<<'\n';
	}
}

int main(int argc, char *argv[]){
	const unsigned int w=100,h=30;
	binform bf(w,h);
	sptr<label> lbl(new label("lbl"));
	draw(lbl->get_image().get());
	sptr<label> lbl2(new label("btn"));
	sptr<button> btn(new button(lbl2));

	const unsigned int img_w = h/3*2, img_h = h/3*2;
	sptr<imagebox> imgbox(new imagebox(img_w,img_h));
	sptr<bit_image> img(new bit_image(img_w,img_h));
	imgbox->set_image(img);
	sptr<drawer> drwr(new drawer(imgbox->get_inner_img()));
	sptr<mybutton> mybtn(new mybutton(drwr));

	bf.add_element(lbl);
	lbl->move(0,0);
	bf.add_element(btn);
	btn->move(0,lbl->get_y()+lbl->get_h());
	bf.add_element(mybtn);
	mybtn->move(lbl->get_x()+lbl->get_w(),
				0);
	bf.add_element(imgbox);
	imgbox->move(w-img_w,h-img_h);

	bf.update();
	draw(bf.get_image().get());
	mybtn->press_e();
	imgbox->update();
	bf.update();
	//draw(bf.get_image().get());
	{
		byte_image* byte_img = binfont::bit_img_to_byte_img(bf.get_image().get());
		draw(byte_img);
		bit_image* bit_img = binfont::byte_img_to_bit_img(byte_img);
		draw(bit_img);
		delete byte_img;
		byte_img = binfont::bit_img_to_byte_img(bit_img);
		draw(byte_img);
		delete bit_img;
		bit_img = binfont::byte_img_to_bit_img(byte_img);
		draw(bit_img);
		delete byte_img, bit_img;
	}
	return 0;
}
