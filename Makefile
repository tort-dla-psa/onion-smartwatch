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

#libbinforms
libbinforms := binforms
libbinforms_flags := --shared -fPIC
libbinforms_s := $(srcdir)/$(libbinforms)/*.cpp
libbinforms_h := $(incdir)/$(libbinforms)
libbinforms_files := $(libbinforms_s) $(libbinforms_h)/*.h
libbinforms_trg := $(libdir)/$(libbinforms).so
libbinforms_dep := 

#libwatches
libwatches := watchlib
libwatches_flags := --shared -fPIC -lpthread
libwatches_s := $(srcdir)/$(libwatches)/*.cpp
libwatches_h := $(incdir)/$(libwatches)
libwatches_files := $(libwatches_s) $(libwatches_h)/*.h
libwatches_trg := $(libdir)/$(libwatches).so
libwatches_dep := $(libbinforms_trg) $(IO)

#interface
app_ui := interface
app_ui_flags := -Wl,-rpath=./$(libdir) $(libbinforms_trg) $(libwatches_trg)
app_ui_s := $(appsdir)/$(app_ui)/*.cpp
app_ui_h := $(appsdir)/$(app_ui) 
app_ui_files := $(app_ui_s) #$(appsdir)/$(app_ui)/*.h
app_ui_trg := $(bindir)/$(app_ui)/$(app_ui)
app_ui_dep := $(libbinforms_trg) $(libwatches_trg)

#hello-world
app_hello := hello-world
app_hello_flags := -Wl,-rpath=$(libdir) $(libbinforms_trg) $(libwatches_trg)
app_hello_s := $(appsdir)/$(app_hello)/*.cpp
app_hello_h := $(appsdir)/$(app_hello) 
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

$(libbinforms_trg): $(libbinforms_files) $(libbinforms_dep)
	$(cxx) $(cxxflags) $(libbinforms_s) -I$(libbinforms_h) $(libbinforms_flags) -o $@

$(libwatches_trg): $(libwatches_files) $(libwatches_dep)
	$(cxx) $(cxxflags) $(libwatches_s) \
		-I$(libwatches_h) -I$(libbinforms_h) \
		-I$(submodsdir)/UnixIO-cpp/include/ $(libwatches_flags) -o $@

$(app_ui_trg): $(app_ui_files)
	mkdir -p $(bindir)/$(app_ui)
	$(cxx) $(cxxflags) $(app_ui_s) -I$(app_ui_h) \
		-I$(libbinforms_h) -I$(libwatches_h) \
		-I$(submodsdir)/UnixIO-cpp/include/ \
		$(submodsdir)/UnixIO-cpp/build/* \
		$(app_ui_flags) -o $@

$(app_hello_trg): $(app_hello_target)
	mkdir -p $(bindir)/$(app_hello)
	$(cxx) $(cxxflags) $(app_hello_s) \
		-I$(libbinforms_h) -I$(libwatches_h) \
		-I$(submodsdir)/UnixIO-cpp/include/ \
		$(submodsdir)/UnixIO-cpp/build/* \
	       	$(app_hello_flags) -o $(app_hello_trg)

$(libbinforms): $(libbinforms_trg)
$(libwatches): $(libwatches_trg)
$(app_ui): $(app_ui_trg)
$(app_hello): $(app_hello_trg)

all: prepare $(libbinforms) $(libwatches) $(app_ui) $(app_hello) $(IO)

clean:
	rm -rf $(blddir) $(libdir) $(bindir)
	$(MAKE) -C $(submodsdir)/UnixIO-cpp/ clean
