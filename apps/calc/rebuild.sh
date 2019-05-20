g++ main.cpp -I ../../submodules/binforms/include/ \
	-I ../../include/watchlib/ \
	-I ../../submodules/UnixIO-cpp/include/ \
	-I ../../include/types/ \
	../../lib/libbinforms.so \
	../../lib/libonionwatch.so \
	../../lib/libunixiocpp.so \
	-std=c++17 -g
