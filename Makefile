incdir := include
libdir := lib
srcdir := src
blddir := build
bindir := bin
appsdir := apps
submodsdir := submodules

extra = 
cxx := g++
cxxflags := -ffast-math -std=c++17 $(extra)

#IO submodule
IO:= $(blddir)/IO.flag

#debugger
dbgr := debugger
dbgr_flags := -c -fPIC
dbgr_s := $(srcdir)/$(dbgr)/*.cpp
dbgr_h := $(incdir)/$(dbgr)
dbgr_inc := $(dbgr_h)
dbgr_files := $(dbgr_s) $(dbgr_h)/*.h
dbgr_trg := $(blddir)/$(dbgr).o
dbgr_dep := 

#libbinforms
libbinforms := binforms
libbinforms_flags := --shared -fPIC
libbinforms_s := $(srcdir)/$(libbinforms)/*.cpp
libbinforms_h := $(incdir)/$(libbinforms)
libbinforms_files := $(libbinforms_s) $(libbinforms_h)/*.h
libbinforms_inc := -I$(libbinforms_h) \
		-I$(dbgr_h)
libbinforms_trg := $(libdir)/$(libbinforms).so
libbinforms_dep :=  $(dbgr_trg)

#libwatches
libwatches := watchlib
libwatches_flags := --shared -fPIC -lpthread
libwatches_s := $(srcdir)/$(libwatches)/*.cpp
libwatches_h := $(incdir)/$(libwatches)
libwatches_files := $(libwatches_s) $(libwatches_h)/*.h
libwatches_inc := -I$(libwatches_h) \
		-I$(libbinforms_h) \
		-I$(dbgr_h) \
		-I$(submodsdir)/UnixIO-cpp/include/
libwatches_trg := $(libdir)/$(libwatches).so
libwatches_dep := $(libbinforms_trg) $(dbgr_trg) $(IO)

#interface
app_ui := interface
app_ui_flags := -Wl,-rpath=./$(libdir) $(libbinforms_trg) $(libwatches_trg)
app_ui_s := $(appsdir)/$(app_ui)/*.cpp
app_ui_h := $(appsdir)/$(app_ui) 
app_ui_files := $(app_ui_s) #$(appsdir)/$(app_ui)/*.h
app_ui_inc := -I$(app_ui_h) $(libwatches_inc)
app_ui_trg := $(bindir)/$(app_ui)/$(app_ui)
app_ui_dep := $(libbinforms_trg) $(libwatches_trg)

#hello-world
app_hello := hello-world
app_hello_flags := -Wl,-rpath=$(libdir) $(libbinforms_trg) $(libwatches_trg)
app_hello_s := $(appsdir)/$(app_hello)/*.cpp
app_hello_h := $(appsdir)/$(app_hello) 
app_hello_inc := -I$(app_hello_h) $(libwatches_inc) 
app_hello_files := $(app_hello_s) #$(appsdir)/$(app_ui)/*.h
app_hello_trg := $(bindir)/$(app_hello)/$(app_hello)
app_hello_dep := $(libbinforms_trg) $(libwatches_trg)

prepare:
	mkdir -p $(blddir)/IO
	mkdir -p $(libdir)
	mkdir -p $(bindir)

$(IO):
	$(MAKE) -C $(submodsdir)/UnixIO-cpp/ extra="$(extra)" all
	touch $(@)

$(dbgr_trg): $(dbgr_files) $(dbgr_dep)
	$(cxx) $(cxxflags) $(dbgr_s) -I$(dbgr_inc) $(dbgr_flags) -o $@

$(libbinforms_trg): $(libbinforms_files) $(libbinforms_dep)
	$(cxx) $(cxxflags) $(libbinforms_s) $(libbinforms_inc) $(libbinforms_flags) -o $@

$(libwatches_trg): $(libwatches_files) $(libwatches_dep)
	$(cxx) $(cxxflags) $(libwatches_s) $(libwatches_inc) $(libwatches_flags) -o $@

$(app_ui_trg): $(app_ui_files)
	mkdir -p $(bindir)/$(app_ui)
	$(cxx) $(cxxflags) $(app_ui_s) $(app_ui_inc) \
		$(submodsdir)/UnixIO-cpp/build/* \
		$(dbgr_trg) \
		$(app_ui_flags) -o $@

$(app_hello_trg): $(app_hello_target)
	mkdir -p $(bindir)/$(app_hello)
	$(cxx) $(cxxflags) $(app_hello_s) $(app_hello_inc) \
		$(submodsdir)/UnixIO-cpp/build/* \
		$(dbgr_trg) \
	       	$(app_hello_flags) -o $(app_hello_trg)

$(libbinforms): $(libbinforms_trg)
$(libwatches): $(libwatches_trg)
$(app_ui): $(app_ui_trg)
$(app_hello): $(app_hello_trg)

all: prepare $(libbinforms) $(libwatches) $(app_ui) $(app_hello) $(IO)

clean:
	rm -rf $(blddir) $(libdir) $(bindir)
	$(MAKE) -C $(submodsdir)/UnixIO-cpp/ clean
