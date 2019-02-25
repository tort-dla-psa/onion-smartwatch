#ifndef LABEL_H
#define LABEL_H
#include <string>
#include "element.h"
#include "imovable.h"

class label: public element,
		public imovable
{
	std::string text;
public:
	label();
	label(const std::string &text);
	label(const char *text);
	void set_text(const std::string &text);
	std::string get_text()const;
	void move(const unsigned int x,
			  const unsigned int y)override;
	void update()override;
};

#endif // LABEL_H
