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
#binforms submodule
binforms:= $(blddir)/binforms.flag
#i2c driver
i2c := $(blddir)/i2c.flag

#logger
lggr := logger
lggr_flags := -c -fPIC
lggr_s := $(srcdir)/$(lggr)/*.cpp
lggr_h := $(incdir)/$(lggr)
lggr_inc := -I$(lggr_h)
lggr_files := $(lggr_s) $(lggr_h)/*.h
lggr_trg := $(blddir)/$(lggr).o
lggr_dep := 

#libwatches
libwatches := watchlib
libwatches_flags := --shared -fPIC -lpthread
libwatches_s := $(srcdir)/$(libwatches)/*.cpp
libwatches_h := $(incdir)/$(libwatches)
libwatches_files := $(libwatches_s) $(libwatches_h)/*.h
libwatches_inc := -I$(libwatches_h) -I$(lggr_h) \
		-I$(submodsdir)/UnixIO-cpp/include/ \
		-I$(submodsdir)/binforms/include/
libwatches_trg := $(libdir)/libonionwatch.so
libwatches_dep := $(lggr_trg) $(IO) $(binforms)

#interface
app_ui := interface
app_ui_flags := -Llib -lbinforms -lonionwatch -lunixiocpp -lpthread
app_ui_s := $(appsdir)/$(app_ui)/*.cpp
app_ui_h := $(appsdir)/$(app_ui) 
app_ui_files := $(app_ui_s) #$(appsdir)/$(app_ui)/*.h
app_ui_inc := -I$(app_ui_h) $(libwatches_inc) -I$(submodsdir)/i2c-exp-driver/include
app_ui_trg := $(bindir)/$(app_ui)/$(app_ui)
app_ui_dep := $(libwatches_trg) $(i2c)

#companion-server
companion_serv := companion-server
companion_serv_flags := -Llib -lonionwatch -lunixiocpp
companion_serv_s := $(appsdir)/$(companion_serv)/*.cpp
companion_serv_h := $(appsdir)/$(companion_serv)
companion_serv_files := $(companion_serv_s)
companion_serv_inc := -I$(companion_serv_h) $(libwatches_inc)
companion_serv_trg := $(bindir)/$(companion_serv)/$(companion_serv)
companion_serv_dep := $(libwatches_trg)

#client-mock
cli_mock := interface-client
cli_mock_flags := -Llib -lunixiocpp
cli_mock_s := $(appsdir)/$(cli_mock)/*.cpp
cli_mock_h := $(appsdir)/$(cli_mock) 
cli_mock_files := $(cli_mock_s) #$(appsdir)/$(app_ui)/*.h
cli_mock_inc := -I$(cli_mock_h) $(libwatches_inc)
cli_mock_trg := $(bindir)/$(cli_mock)/$(cli_mock)
cli_mock_dep := $(IO)

#hello-world
app_hello := hello-world
#app_hello_flags := -Wl,-rpath=$(libdir) $(libbinforms_trg) $(libwatches_trg)
app_hello_s := $(appsdir)/$(app_hello)/*.cpp
app_hello_h := $(appsdir)/$(app_hello) 
app_hello_inc := -I$(app_hello_h) $(libwatches_inc) 
app_hello_files := $(app_hello_s) #$(appsdir)/$(app_ui)/*.h
app_hello_trg := $(bindir)/$(app_hello)/$(app_hello)
app_hello_dep := $(libwatches_trg)

.DEFAULT_GOAL = all

prepare:
	mkdir -p $(blddir)
	mkdir -p $(libdir)
	mkdir -p $(bindir)

$(IO):
	$(MAKE) -C $(submodsdir)/UnixIO-cpp/ extra="$(extra)" all
	cp $(submodsdir)/UnixIO-cpp/lib/* $(libdir)/libunixiocpp.so
	touch $(@)

$(binforms):
	$(MAKE) -C $(submodsdir)/binforms/ extra="$(extra)" all
	cp $(submodsdir)/binforms/lib/* $(libdir)/libbinforms.so
	touch $(@)

$(i2c):
	$(MAKE) -C $(submodsdir)/i2c-exp-driver extra="$(extra)" all
	touch $(@)

$(lggr_trg): $(lggr_files) $(lggr_dep)
	$(cxx) $(cxxflags) $(lggr_s) $(lggr_inc) $(lggr_flags) -o $@

$(libwatches_trg): $(libwatches_files) $(libwatches_dep)
	$(cxx) $(cxxflags) $(libwatches_s) $(libwatches_inc) $(libwatches_flags) -o $@

$(app_ui_trg): $(app_ui_files) $(app_ui_dep)
	mkdir -p $(bindir)/$(app_ui)
	$(cxx) $(cxxflags) $(app_ui_s) $(app_ui_inc) \
		$(submodsdir)/i2c-exp-driver/build/*.o \
		$(submodsdir)/i2c-exp-driver/build/lib/*.o \
		$(lggr_trg) $(app_ui_flags) -o $@

$(companion_serv_trg): $(companion_serv_files) $(companion_serv_dep)
	mkdir -p $(bindir)/$(companion_serv)
	$(cxx) $(cxxflags) $(companion_serv_s) $(companion_serv_inc) \
		$(lggr_trg) $(companion_serv_flags) -o $@

$(app_hello_trg): $(app_hello_files) $(app_hello_dep)
	mkdir -p $(bindir)/$(app_hello)
	$(cxx) $(cxxflags) $(app_hello_s) $(app_hello_inc) \
		$(lggr_trg) $(app_hello_flags) -o $(app_hello_trg)

$(cli_mock_trg): $(cli_mock_files) $(cli_mock_dep)
	mkdir -p $(bindir)/$(cli_mock)
	$(cxx) $(cxxflags) $(cli_mock_s) $(cli_mock_inc) \
		$(srcdir)/$(libwatches)/packet.cpp \
	       	$(cli_mock_flags) -o $(cli_mock_trg)

$(libwatches): $(libwatches_trg)
$(app_ui): $(app_ui_trg)
# $(app_hello): $(app_hello_trg)
$(cli_mock): $(cli_mock_trg)
$(companion_serv): $(companion_serv_trg)

all: prepare $(app_ui) $(cli_mock) $(companion_serv) $(app_hello) 

clean:
	rm -rf $(blddir) $(libdir) $(bindir)
	$(MAKE) -C $(submodsdir)/UnixIO-cpp/ clean
	$(MAKE) -C $(submodsdir)/binforms/ clean
	$(MAKE) -C $(submodsdir)/i2c-exp-driver/ clean
