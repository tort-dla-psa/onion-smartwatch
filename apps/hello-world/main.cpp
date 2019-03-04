#include <iostream>
#include <memory>
#include <errno.h>
#include <string.h>
#include "watchlib.h"
#include "binform.h"
#include "label.h"

using namespace watches;

int main(){
	sptr<binform> form(new binform(100,10));
	sptr<label> lbl1(new label("hello world"));

	watchlib lib_obj;
	lib_obj.init();
	lib_obj.set_form(form);
}
