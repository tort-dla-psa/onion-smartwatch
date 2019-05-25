#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include "button.h"
#include "watchlib.h"
#include "binform.h"
#include "image.h"
#include "stack.h"

const int w = 100, h = 64;
using namespace binforms;
using namespace watches;

class myform:public binform{
protected:
	std::string first, second;
	bool enter_second;
	std::shared_ptr<label> lbl;
	std::function<void(void)> f;
	void update_label(){
		lbl->set_text(first);
		lbl->update();
		update();
	}
	std::shared_ptr<button> mk_btn(std::vector<std::shared_ptr<button>> &vec,
		const std::string &lbl)
	{
		return vec.emplace_back(new button(lbl));
	}
public:
	myform():binform(w,h){
		f = nullptr;
		first = "0";
		second = "0";
		enter_second = false;
		auto l = std::make_shared<container>(w,h);
		std::vector<std::shared_ptr<button>> buttons, actions;
		buttons.reserve(15);

		lbl = std::make_shared<label>(first);
		auto b9 = mk_btn(buttons,("9"));
		auto b8 = mk_btn(buttons,"8");
		auto b7 = mk_btn(buttons,"7");
		auto b6 = mk_btn(buttons,"6");
		auto b5 = mk_btn(buttons,"5");
		auto b4 = mk_btn(buttons,"4");
		auto b3 = mk_btn(buttons,"3");
		auto b2 = mk_btn(buttons,"2");
		auto b1 = mk_btn(buttons,"1");
		auto b0 = mk_btn(buttons,"0");
		auto b_dot = mk_btn(buttons,".");
		auto b_plus = mk_btn(actions,"+");
		auto b_minus = mk_btn(actions,"-");
		auto b_mul = mk_btn(actions,"*");
		auto b_div = mk_btn(actions,"/");
		auto b_eq = mk_btn(actions,"=");

		lbl->move(0,0);
		b9->move(0, lbl->get_h());
		auto st_v = std::make_shared<v_stack>();
		auto st_h = std::make_shared<h_stack>();
		st_h->set_elements({b9, b8, b7});
		st_h->update();
		st_v->add_element(std::move(st_h));
		st_v->update();

		st_h = std::make_shared<h_stack>();
		st_h->set_elements({b6, b5, b4});
		st_h->update();
		st_v->add_element(std::move(st_h));
		st_v->update();

		st_h = std::make_shared<h_stack>();
		st_h->set_elements({b3, b2, b1});
		st_h->update();
		st_v->add_element(std::move(st_h));
		st_v->update();

		st_h = std::make_shared<h_stack>();
		st_h->set_elements({b0, b_dot, b_eq});
		st_h->update();
		st_v->add_element(std::move(st_h));
		st_v->update();

		st_h = std::make_shared<h_stack>();
		auto st_funcs = std::make_shared<v_stack>();
		st_funcs->set_elements({b_plus,b_minus,b_mul,b_div});
		st_funcs->update();
		st_h->set_elements({st_v, st_funcs});
		st_h->update();

		l->add_element(lbl);
		for(auto &b:buttons){
			auto f = std::bind(&myform::enter_digit, this,
				(b->get_label())->get_text()[0]);
			b->bind(f);
		}
		l->add_element(st_h);
		st_h->move(0, lbl->get_h() + 1);

		b_plus->bind(std::bind(&myform::add, this));
		b_minus->bind(std::bind(&myform::sub, this));
		b_mul->bind(std::bind(&myform::mul, this));
		b_div->bind(std::bind(&myform::div, this));
		b_div->bind(std::bind(&myform::eq, this));
		add_layer(l);
		update();
	}
	void enter_digit(char digit){
		if(f){
			second += digit;
		}else{
			first += digit;
		}
		update_label();
	}
	void add(){
		eq();
		f = [this](){
			double real_first = std::stod(first);
			double real_second = std::stod(second);
			real_first+=real_second;
			first = std::to_string(real_first);
			second = "0";
		};
	}
	void sub(){
		eq();
		f = [this](){
			double real_first = std::stod(first);
			double real_second = std::stod(second);
			real_first-=real_second;
			first = std::to_string(real_first);
			second = "0";
		};
	}
	void mul(){
		eq();
		f = [this](){
			double real_first = std::stod(first);
			double real_second = std::stod(second);
			real_first*=real_second;
			first = std::to_string(real_first);
			second = "0";
		};
	}
	void div(){
		eq();
		f = [this](){
			double real_first = std::stod(first);
			double real_second = std::stod(second);
			real_first/=real_second;
			first = std::to_string(real_first);
			second = "0";
		};
	}
	void eq(){
		if(f){
			return;
		}
		f();
		update_label();
	}
};

inline void draw_img(std::shared_ptr<element> el){
	const auto img = el->get_image();
	const uint w = img->get_w();
	const uint h = img->get_h();

	for(uint i=0; i<h; i++){
		for(uint j=0; j<w; j++){
			std::cout<<(img->get_pixel(j,i)? '#':'_');
		}
		std::cout<<'\n';
	}
}

int main(){
	auto f = std::make_shared<myform>();
	f->update();
	draw_img(f);
	watchlib lib("calc");
	lib.init();
	lib.set_form(f);
	lib.start();
};
