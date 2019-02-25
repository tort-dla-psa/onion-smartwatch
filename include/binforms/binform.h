#ifndef BINFORM_H
#define BINFORM_H

#include "aliases.h"
#include "element.h"
#include "iholding.h"

class binform:public element,
		public iholding_multiple
{
	vec_s<element> elements;
public:
	binform(const unsigned int w, const unsigned int h);
	unsigned int get_w()const;
	unsigned int get_h()const;
	bool get_changed()const override;
	void update()override;
	virtual vec_s<element> get_elements()const;
	virtual sptr<element> get_element(const unsigned int place)const override;
	virtual void set_elements(const vec_s<element> &elements)override;
	virtual void set_element(const unsigned int place,
				const sptr<element> &el)override;
	virtual void add_element(const sptr<element> &el)override;
};

#endif // BINFORM_H
