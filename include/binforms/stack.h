#ifndef STACK_H
#define STACK_H
#include "element.h"
#include "iholding.h"
#include "graphics.h"
#include "aliases.h"

using namespace graphics;

class stack:public element,
		public iholding_multiple
{
protected:
	uptr<drawer> drwr;
	stack();
	vec_s<element> elements;
	virtual void align_items()=0;
public:
	virtual ~stack();
	unsigned int get_size()const;
	sptr<element> get_next_of(const sptr<element> &el)const;
	sptr<element> get_prev_of(const sptr<element> &el)const;
	virtual void update_size()=0;
	vec_s<element> get_elements()const override;
	sptr<element> get_element(const unsigned int place)const override;
	unsigned int get_place(const sptr<element> &el)const;
	virtual void add_element(const sptr<element> &el)override =0;
	virtual void set_elements(const vec_s<element> &elements)override =0;
	virtual void set_element(const unsigned int place,
			const sptr<element> &el)override =0;
};

class v_stack:public stack{
	void align_items()override;
public:
	v_stack();
	void update()override;
	void update_size()override;
	void add_element(const sptr<element> &el)override;
	void set_elements(const vec_s<element> &elements)override;
	void set_element(const unsigned int place,
			const sptr<element> &el)override;
};

class h_stack:public stack{
	void align_items()override;
public:
	h_stack();
	void update()override;
	void update_size()override;
	void add_element(const sptr<element> &el)override;
	void set_elements(const vec_s<element> &elements)override;
	void set_element(const unsigned int place,
			const sptr<element> &el)override;
};

#endif // STACK_H
