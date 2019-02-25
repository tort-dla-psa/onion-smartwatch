#ifndef BUTTON_H
#define BUTTON_H
#include "iresizable.h"
#include "aliases.h"
#include "label.h"
#include "element.h"
#include "graphics.h"

using namespace graphics;

class button: public element,
		public iresizable,
		public imovable
{
protected:
	uptr<drawer> drwr;
	sptr<label> lbl;
public:
	button();
	button(const unsigned int w,
		   const unsigned int h);
	button(const label &lbl);
	button(const unsigned int w,
		   const unsigned int h,
		   const label &lbl);
	button(const sptr<label> &lbl);
	button(const unsigned int w,
		   const unsigned int h,
		   const sptr<label> &lbl);
	button(const std::string &text);
	button(const unsigned int w,
		   const unsigned int h,
		   const std::string &text);
	virtual ~button();

	virtual void set_label(const std::string &text);
	virtual void set_label(const sptr<label> &lbl);
	sptr<label> get_label()const;

	virtual void move(const unsigned int x,
					  const unsigned int y)override;
	virtual void resize(const unsigned int w,
						const unsigned int h)override;

	virtual void update()override;
};

#endif // BUTTON_H
