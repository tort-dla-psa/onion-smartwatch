#ifndef IHOLDING_H
#define IHOLDING_H
#include "aliases.h"

class element;

class iholding_single{
public:
	virtual ~iholding_single(){}
	virtual sptr<element> get_element()=0;
	virtual void set_element(const sptr<element> &el)=0;
};

class iholding_multiple{
public:
	virtual vec_s<element> get_elements()const=0;
	virtual sptr<element> get_element(const unsigned int place)const=0;
	virtual void set_elements(const vec_s<element> &elements)=0;
	virtual void set_element(const unsigned int place,
				const sptr<element> &el)=0;
	virtual void add_element(const sptr<element> &el)=0;
};

#endif // IHOLDING_H
