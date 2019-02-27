incdir := include
libdir := lib
srcdir := src
blddir := build
bindir := bin
appsdir := apps
submodsdir := submodules

extra:=
cxx:= g++
cxxflags:= -ffast-math -g -DDEBUG $(extra)

common_inc:=-I../include/watchlib/ -I../submodules/UnixIO-cpp/include/ -I../include/debugger

packets_cli:=packets-cli
packets_cli_s:=$(packets_cli)/*.cpp
packets_cli_srcs:=../submodules/UnixIO-cpp/build/* ../build/*.o
packets_cli_flags:= -L../lib -l:../lib/watchlib.so
packets_cli_trg:= $(packets_cli)/a.out

packets_srv:=packets-srv
packets_srv_s:=$(packets_srv)/*.cpp
packets_srv_srcs:=../submodules/UnixIO-cpp/build/* ../build/*.o
packets_srv_flags:= -L../lib -l:../lib/watchlib.so -lpthread
packets_srv_trg:= $(packets_srv)/a.out

$(packets_cli_trg): $(packets_cli_s)
	$(cxx) $(cxxflags) $(packets_cli_srcs) $(packets_cli_s) $(common_inc) $(packets_cli_flags) -o $@

$(packets_srv_trg): $(packets_srv_s)
	$(cxx) $(cxxflags) $(packets_srv_srcs) $(packets_srv_s) $(common_inc) $(packets_srv_flags) -o $@

all: $(packets_cli_trg) $(packets_srv_trg)
clean: 
	rm **/a.out
