#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include "button.h"
#include "watchlib.h"
#include "binform.h"

const int w = 128, h = 64;
template<typename T>
using sptr = std::shared_ptr<T>;

class myform:public binform{
	std::string first, second;
	bool enter_second;
	void move_after_right(const sptr<button> &el, sptr<button> el2){
		el2->move(el->get_x()+el->get_w(), el->get_y());
	}
	void move_after_down(const sptr<button> &el, sptr<button> el2){
		el2->move(el->get_x(), el->get_y()+el->get_h());
	}
	sptr<label> lbl;
	sptr<std::function<void(void)>> f;
	void update_label(){
		lbl->
public:
	myform():binform(w,h){
		f = nullptr;
		first = "0";
		second = "0";
		enter_second = false;
		sptr<layer> l(new layer(w,h));
		std::vector<sptr<button>> buttons, actions;
		buttons.reserve(15);

		lbl = std::make_shared<label>(first);
		sptr<button> b9 = buttons.emplace_back(new button("9"));
		sptr<button> b8 = buttons.emplace_back(new button("8"));
		sptr<button> b7 = buttons.emplace_back(new button("7"));
		sptr<button> b6 = buttons.emplace_back(new button("6"));
		sptr<button> b5 = buttons.emplace_back(new button("5"));
		sptr<button> b4 = buttons.emplace_back(new button("4"));
		sptr<button> b3 = buttons.emplace_back(new button("3"));
		sptr<button> b2 = buttons.emplace_back(new button("2"));
		sptr<button> b1 = buttons.emplace_back(new button("1"));
		sptr<button> b0 = buttons.emplace_back(new button("0"));
		sptr<button> b_dot = buttons.emplace_back(new button("."));
		sptr<button> b_plus = actions.emplace_back(new button("+"));
		sptr<button> b_minus = actions.emplace_back(new button("-"));
		sptr<button> b_mul = actions.emplace_back(new button("*"));
		sptr<button> b_div = actions.emplace_back(new button("/"));
		sptr<button> b_eq = actions.emplace_back(new button("="));

		lbl->move(0,0);
		b9->move(0, lbl->get_h());
		move_after_right(b9, b8);
		move_after_right(b8, b7);
		move_after_down(b9, b6);
		move_after_right(b6, b5);
		move_after_right(b5, b4);
		move_after_down(b6, b3);
		move_after_right(b3, b2);
		move_after_right(b2, b1);
		move_after_down(b3, b0);
		move_after_right(b0, b_dot);
		move_after_right(b_dot, b_eq);
		move_after_right(b7, b_plus);
		move_after_down(b_plus, b_minus);
		move_after_down(b_minus, b_mul);
		move_after_down(b_mul, b_div);

		l->add_element(lbl);
		for(auto &b:buttons){
			l->add_element(b);
			b->bind([this, &b](){
				enter_digit((b->get_label())->get_text()[0]);
			});
		}
		for(auto &b:actions){
			l->add_element(b);
		}
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
			second+=digit;
		}else{
			first+=digit;
		}
	}
	void add(){
		eq();
		f = std::make_shared<std::function<void()>>([this](){
			double real_first = std::stod(first);
			double real_second = std::stod(second);
			real_first+=real_second;
			first = std::to_string(real_first);
			second = "0";
		});
	}
	void sub(){
		eq();
		f = std::make_shared<std::function<void()>>([this](){
			double real_first = std::stod(first);
			double real_second = std::stod(second);
			real_first-=real_second;
			first = std::to_string(real_first);
			second = "0";
		});
	}
	void mul(){
		eq();
		f = std::make_shared<std::function<void()>>([this](){
			double real_first = std::stod(first);
			double real_second = std::stod(second);
			real_first*=real_second;
			first = std::to_string(real_first);
			second = "0";
		});
	}
	void div(){
		eq();
		f = std::make_shared<std::function<void()>>([this](){
			double real_first = std::stod(first);
			double real_second = std::stod(second);
			real_first/=real_second;
			first = std::to_string(real_first);
			second = "0";
		});
	}
	void eq(){
		if(f){
			(*f)();
		}
	}
};

inline void draw_img(sptr<element> el){
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
	sptr<myform> f(new myform());
	f->update();
	draw_img(f);
	watchlib lib("calc");
	lib.init();
	lib.set_form(f);
};
