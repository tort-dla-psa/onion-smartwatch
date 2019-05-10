#!/bin/bash

g++ cli.cpp -g -I ../../include/watchlib \
		-I ../../submodules/binforms/include \
		-I ../../submodules/UnixIO-cpp/include \
		-I ../../include/types/ \
		-L ../../lib/ -lunixiocpp -lonionwatch -o cli \
&& \
g++ srv.cpp -g -I ../../include/watchlib \
		-I ../../submodules/binforms/include \
		-I ../../submodules/UnixIO-cpp/include \
		-I ../../include/types/ \
		-L ../../lib/ -lunixiocpp -lonionwatch -o srv

