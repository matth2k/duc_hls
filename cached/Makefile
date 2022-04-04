# source verilog files
XILINX_HLS?=/opt/Xilinx/Vitis_HLS/2021.1
SETTINGS=$(XILINX_HLS)/settings64.sh

#The C functions that make up top modules
TOP_MODULES=stream2mem mem2stream
PACKAGES = $(patsubst %,%.zip,$(TOP_MODULES))
# All CPP source files
CPP_SOURCES=$(wildcard ./src/*.cpp)
CPP_SOURCES+=$(wildcard ./src/*.h)

#Part
PART=xczu3eg-sbva484-1-i
#clk PERIOD
PERIOD=10


all: clean $(PACKAGES)

.PHONY: clean
clean:
	-rm -rf build
	

%.zip: clean build %_script.tcl
	source $(SETTINGS); vitis_hls -f $*_script.tcl
	+cp -r $*_prj/solution1/syn/verilog/* build/verilog
	cp -r src/*.v build/verilog
	-rm -rf *_prj *.log

build:
	mkdir $@
	mkdir $@/iprepo
	mkdir $@/verilog



%_script.tcl:
	@echo "open_project $*_prj" > $@
	@echo "set_top $*" >> $@
	@echo "add_files \"$(CPP_SOURCES)\"" >> $@
	@echo "open_solution "solution1" -flow_target vivado" >> $@
	@echo "set_part {$(PART)}" >> $@
	@echo "create_clock -period $(PERIOD) -name default" >> $@
	@echo "config_export -format ip_catalog -output ./build/iprepo/$*.zip -rtl verilog" >> $@
	@echo "csynth_design" >> $@
	@echo "export_design -rtl verilog -format ip_catalog -output ./build/iprepo/$*.zip" >> $@
	@echo "close_project" >> $@
	@echo "quit" >> $@
	







