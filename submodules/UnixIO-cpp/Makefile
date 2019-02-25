blddir := build
incdir := include
srcdir := src
extra = 
cxx := g++
cxxflags := -ffast-math -std=c++17 $(extra)

#file
file_name := file
file_flags := -c -fPIC
file_src := $(srcdir)/$(file_name).cpp
file_files := $(incdir)/$(file_name).h $(file_src)
file_trg := $(blddir)/$(file_name).o
file_dep :=

#file_op
file_op_name := file_op
file_op_flags := -c -fPIC
file_op_src := $(srcdir)/$(file_op_name).cpp
file_op_files := $(incdir)/$(file_op_name).h $(file_src)
file_op_trg := $(blddir)/file_op.o
file_op_dep := $(file_trg)

#fifo_op
fifo_op_name := fifo_op
fifo_op_flags := -c -fPIC
fifo_op_src := $(srcdir)/$(fifo_op_name).cpp
fifo_op_files := $(incdir)/$(fifo_op_name).h $(fifo_op_src)
fifo_op_trg := $(blddir)/fifo_op.o
fifo_op_dep := $(file_trg) $(file_op_trg)

#stat_op
stat_op_name := stat_op
stat_op_flags := -c -fPIC
stat_op_src := $(srcdir)/$(stat_op_name).cpp
stat_op_files := $(incdir)/$(stat_op_name).h $(stat_op_src)
stat_op_trg := $(blddir)/stat_op.o
stat_op_dep := $(file_trg)

#dir_op
dir_op_name := dir_op
dir_op_flags := -c -fPIC
dir_op_src := $(srcdir)/$(dir_op_name).cpp
dir_op_files := $(incdir)/$(dir_op_name).h $(dir_op_src)
dir_op_trg := $(blddir)/dir_op.o
dir_op_dep := $(file_trg) $(file_op_trg)

#socket_op
socket_op_name := socket_op
socket_op_flags := -c -fPIC
socket_op_src := $(srcdir)/$(socket_op_name).cpp
socket_op_files := $(incdir)/$(socket_op_name).h
socket_op_trg := $(blddir)/socket_op.o
socket_op_dep := $(file_trg) $(file_op_trg)

prepare:
	mkdir -p $(blddir)

$(file_trg): $(file_files) $(file_dep)
	$(cxx) $(cxxflags) $(file_src) -I$(incdir) $(file_flags) -o $@

$(file_op_trg): $(file_op_files) $(file_op_dep)
	$(cxx) $(cxxflags) $(file_op_src) -I$(incdir) $(file_op_flags) -o $@

$(fifo_op_trg): $(fifo_op_files) $(fifo_op_dep)
	$(cxx) $(cxxflags) $(fifo_op_src) -I$(incdir) $(fifo_op_flags) -o $@

$(stat_op_trg): $(stat_op_files) $(stat_op_dep)
	$(cxx) $(cxxflags) $(stat_op_src) -I$(incdir) $(stat_op_flags) -o $@

$(socket_op_trg): $(socket_op_files) $(socket_op_dep)
	$(cxx) $(cxxflags) $(socket_op_src) -I$(incdir) $(socket_op_flags) -o $@

$(dir_op_trg): $(dir_op_files) $(dir_op_dep)
	$(cxx) $(cxxflags) $(dir_op_src) -I$(incdir) $(dir_op_flags) -o $@

all: prepare \
	$(file_trg) \
	$(file_op_trg) \
	$(fifo_op_trg) \
	$(stat_op_trg) \
	$(socket_op_trg) \
	$(dir_op_trg)

clean:
	rm -rf $(blddir)
