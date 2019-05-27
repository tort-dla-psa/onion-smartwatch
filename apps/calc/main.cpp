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
	std::string* current;
	std::shared_ptr<label> lbl;
	std::function<void(double &a, double b)> f;

	void update_label(const std::string &text){
		lbl->set_text(text);
		lbl->update();
		update();
	}

	std::shared_ptr<button> mk_btn(std::vector<std::shared_ptr<button>> &vec,
		const std::string &lbl)
	{
		return vec.emplace_back(new button(lbl));
	}

	void do_math(){
		if(!f){
			return;
		}
		if(current == &first){
			current = &second;
			update_label(second);
			return;
		}
		double real_first = std::stod(first);
		double real_second = std::stod(second);
		f(real_first, real_second);
		first = std::to_string(real_first);
		update_label(first);
		second = "0";
	}
public:
	myform():binform(w,h){
		f = nullptr;
		first = "0";
		second = "0";
		current = &first;
		auto l = std::make_shared<container>(w,h);
		std::vector<std::shared_ptr<button>> buttons, actions;
		buttons.reserve(15);

		lbl = std::make_shared<label>(first);
		auto b9 = mk_btn(buttons, "9");
		auto b8 = mk_btn(buttons, "8");
		auto b7 = mk_btn(buttons, "7");
		auto b6 = mk_btn(buttons, "6");
		auto b5 = mk_btn(buttons, "5");
		auto b4 = mk_btn(buttons, "4");
		auto b3 = mk_btn(buttons, "3");
		auto b2 = mk_btn(buttons, "2");
		auto b1 = mk_btn(buttons, "1");
		auto b0 = mk_btn(buttons, "0");
		auto b_dot = 	mk_btn(buttons, ".");
		auto b_plus = 	mk_btn(actions, "+");
		auto b_minus = 	mk_btn(actions, "-");
		auto b_mul = 	mk_btn(actions, "*");
		auto b_div = 	mk_btn(actions, "/");
		auto b_eq = 	mk_btn(actions, "=");

		lbl->move(0,0);
		auto buttons_cols = std::make_shared<h_stack>();
		auto buttons_rows = std::make_shared<v_stack>();
		buttons_cols->set_elements({b9, b8, b7});
		buttons_rows->add_element(std::move(buttons_cols));

		buttons_cols = std::make_shared<h_stack>();
		buttons_cols->set_elements({b6, b5, b4});
		buttons_rows->add_element(std::move(buttons_cols));

		buttons_cols = std::make_shared<h_stack>();
		buttons_cols->set_elements({b3, b2, b1});
		buttons_rows->add_element(std::move(buttons_cols));

		buttons_cols = std::make_shared<h_stack>();
		buttons_cols->set_elements({b0, b_dot, b_eq});
		buttons_rows->add_element(std::move(buttons_cols));

		auto main_st = std::make_shared<h_stack>();
		auto st_funcs = std::make_shared<v_stack>();
		st_funcs->set_elements({b_plus,b_minus,b_mul,b_div});
		main_st->set_elements({std::move(buttons_rows), std::move(st_funcs)});
		main_st->move(0, lbl->get_h() + 1);
		main_st->update();
		l->add_element(std::move(main_st));

		l->add_element(lbl);
		for(auto &b:buttons){
			auto f = std::bind(&myform::enter_digit, this,
				(b->get_label())->get_text()[0]);
			b->bind(f);
		}

		b_plus->bind([this](){
			f = [](double &a, double b){ a += b; };
			do_math();
		});
		b_minus->bind([this](){
			f = [](double &a, double b){ a -= b; };
			do_math();
		});
		b_mul->bind([this](){
			f = [](double &a, double b){ a *= b; };
			do_math();
		});
		b_div->bind([this](){
			f = [](double &a, double b){ a /= b; };
			do_math();
		});
		b_eq->bind([this](){
			do_math();
		});

		add_layer(l);
		update();

#ifdef DBG_CALC
		enter_digit('9');
		enter_digit('9');
		b_plus->on_press_e(std::make_shared<event>(0,0));
		enter_digit('1');
		b_eq->on_press_e(std::make_shared<event>(0,0));
#endif
	}

	void enter_digit(char digit){
		if(*current == "0"){
			current->clear();
			current->operator=(digit);
		}else{
			current->push_back(digit);
		}
		update_label(*current);
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
