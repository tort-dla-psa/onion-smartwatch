#pragma once
#include <iostream>

template <typename C>
struct reverse_wrapper {
	C &c_;
	reverse_wrapper(C &c):c_(c){}
	typename C::reverse_iterator begin(){
		return c_.rbegin();
	}
	typename C::reverse_iterator end(){
		return c_.rend();
	}
};

template <typename C, size_t N>
struct reverse_wrapper<C[N]>{
	C (&c_) [N];
	reverse_wrapper(C(&c) [N]):c_(c){}
	typename std::reverse_iterator<const C *> begin(){
		return std::rbegin(c_);
	}
	typename std::reverse_iterator<const C *> end(){
		return std::rend(c_);
	}
};

template <typename C>
reverse_wrapper<C> r_wrap(C &c) {
	return reverse_wrapper<C>(c);
}
