TOPNAME = top
NXDC_FILES = constr/top.nxdc
INC_PATH ?=

VERILATOR_FLAGS += -MMD --build -cc \
								 -O3 --x-assign fast --x-initial fast --noassert	

VERILATOR = verilator

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir 
NVBOARD_TARGET = $(BUILD_DIR)/$(TOPNAME) 


# ---------------------------------------------------------- 
#  default
default: build $(NVBOARD_TARGET)

# constrain file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

# source file
VERILATOR_VSRC = $(shell find $(abspath ./vsrc) -name "*.v")
VERILATOR_CSRC = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
VERILATOR_CSRC += $(SRC_AUTO_BIND)


build:
	mkdir -p $(BUILD_DIR)

# rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk
# rules for git
include ../Makefile

# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

# simulation
$(NVBOARD_TARGET): $(VERILATOR_VSRC) $(VERILATOR_CSRC) $(NVBOARD_ARCHIVE)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
	 --Mdir $(OBJ_DIR) --exe -o $(abspath $(NVBOARD_TARGET)) 

run: $(NVBOARD_TARGET)
	@$^

.PHONY: clean	

clean:
	rm -rf $(BUILD_DIR)


