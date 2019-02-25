#ifndef ELEMENT_H
#define ELEMENT_H
#include "aliases.h"

class bit_image;
class element{
private:
	bool changed;
protected:
	unsigned int x,y;
	sptr<bit_image> img;
public:
	element(const unsigned int w,
		const unsigned int h);
	virtual ~element();
	unsigned int get_w()const;
	unsigned int get_h()const;
	void set_changed(const bool status);
	virtual bool get_changed()const;
	virtual void update()=0;
	sptr<bit_image> get_image()const;
	virtual void move(const unsigned int x, const unsigned int y);
	virtual unsigned int get_x()const;
	virtual unsigned int get_y()const;

	virtual void press_e(){}
	virtual void tap_e(){}
	virtual void release_e(){}
	virtual void focus_enter_e(){}
	virtual void focus_leave_e(){}
	virtual void swipe_left_e(){}
	virtual void swipe_right_e(){}
	virtual void swipe_up_e(){}
	virtual void swipe_down_e(){}
};

#endif // ELEMENT_H
