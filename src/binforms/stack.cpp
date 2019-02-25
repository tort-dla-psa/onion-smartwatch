#include <stack.h>

stack::stack()
	:element(0,0),
	iholding_multiple()
{
	drwr = uptr<drawer>(new drawer(this->img));
}

stack::~stack(){}

unsigned int stack::get_size()const{
	return elements.size();
}
unsigned int stack::get_place(const sptr<element> &el)const{
	const unsigned int end = elements.size();
	for(unsigned int i=0; i<end; i++){
		if(elements[i] == el){
			return i;
		}
	}
	return -1;
}
sptr<element> stack::get_next_of(const sptr<element> &el)const{
	const unsigned int place = get_place(el);
	return get_element(place+1);
}

sptr<element> stack::get_prev_of(const sptr<element> &el)const{
	const unsigned int place = get_place(el);
	return get_element(place-1);
}

vec_s<element> stack::get_elements()const{
	return elements;
}
sptr<element> stack::get_element(const unsigned int place)const{
	return elements[place];
}



v_stack::v_stack():stack(){}

void v_stack::align_items(){
	unsigned int prev_h=0;
	for(auto &el: elements){
		el->move(0, prev_h);
		prev_h += el->get_h();
	}
	set_changed(true);
}
void v_stack::set_elements(const vec_s<element> &elements){
	this->elements = elements;
	align_items();
	update_size();
}
void v_stack::set_element(const unsigned int place,
			const sptr<element> &el)
{
	this->elements[place] = el;
	align_items();
	update_size();
}
void v_stack::add_element(const sptr<element> &el){
	elements.emplace_back(el);
	align_items();
	update_size();
}
void v_stack::update_size(){
	unsigned int new_w = 0, new_h = 0;
	for(auto &el: elements){
		new_h += el->get_h();
		new_w = std::max(new_w, el->get_w());
	}
	this->img->resize(new_w, new_h);
	drwr->fill_image(false, this->img);
	set_changed(true);
}
void v_stack::update(){
	for(auto &el:elements){
		if(el->get_changed()){
			drwr->draw_image(el->get_x(),
					el->get_y(),
					el->get_image());
			el->set_changed(false);
		}
	}
	set_changed(true);
}


h_stack::h_stack():stack(){}
void h_stack::align_items(){
	unsigned int prev_w=0;
	for(auto &el: elements){
		el->move(prev_w, 0);
		prev_w += el->get_w();
	}
	set_changed(true);
}
void h_stack::set_elements(const vec_s<element> &elements){
	this->elements = elements;
	align_items();
	update_size();
}
void h_stack::set_element(const unsigned int place,
			const sptr<element> &el)
{
	this->elements[place] = el;
	align_items();
	update_size();
}
void h_stack::add_element(const sptr<element> &el){
	elements.emplace_back(el);
	align_items();
	update_size();
}
void h_stack::update_size(){
	unsigned int new_w = 0, new_h = 0;
	for(auto &el: elements){
		new_h = std::max(new_h, el->get_h());
		new_w += el->get_w();
	}
	this->img->resize(new_w, new_h);
	drwr->fill_image(false, this->img);
	set_changed(true);
}
void h_stack::update(){
	for(auto &el:elements){
		if(el->get_changed()){
			drwr->draw_image(el->get_x(),
					el->get_y(),
					el->get_image());
			el->set_changed(false);
		}
	}
	set_changed(true);
}
