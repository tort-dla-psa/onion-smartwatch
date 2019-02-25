#ifndef IMOVABLE_H
#define IMOVABLE_H

class imovable{
public:
	virtual void move(const unsigned int x,
					  const unsigned int y)=0;
};

#endif // IMOVABLE_H
