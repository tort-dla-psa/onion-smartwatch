#include <iostream>
#include <string>
#include <memory>
#include <fcntl.h>
#include "fifo_op.h"

using namespace IO;

int main(){
	const std::string path="/tmp/fifo1";
	fifo_op ff_op;
	sptr<fifo> ff = nullptr;
	if(!ff_op.check(path)){
		std::cout<<"creating FIFO\n";
		ff = ff_op.create(path, 0666);
	}
	ff = ff_op.open(path, O_WRONLY);
	if(!ff){
		std::cout<<"error on opening/creating FIFO\n";
	}
	if(!ff_op.write(ff, "some 1337 d@tA")){
		std::cout<<"error on writing data\n";
		goto return_error;
	}
	if(!ff_op.close(ff)){
		std::cout<<"error on closing FIFO\n";
		goto return_error;
	}
	ff = ff_op.open(path, O_RDONLY);
	if(!ff){
		std::cout<<"error on reopening fifo\n";
		goto return_error;
	}
	{
		std::cout << "reading data...\n";
		std::string data = ff_op.read(ff);
		std::cout << data << '\n';
	}
	if(!ff_op.close(ff)){
		std::cout<<"error on closing FIFO\n";
		goto return_error;
	}
	if(!ff_op.remove(ff)){
		std::cout<<"error on removing FIFO\n";
		goto return_error;
	}
	return 0;
return_error:
	std::cout<<ff_op.get_errmes()<<'\n';
	return -1;
}
