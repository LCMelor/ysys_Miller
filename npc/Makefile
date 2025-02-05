# Basic Setup and Check

ifeq ($(MAKECMDGOALS),)
  MAKECMDGOALS  = npc
endif

# 1.General Compilation Target Settings

TOPNAME = core

# Create a build directory
WORK_DIR = $(shell pwd)
DST_DIR  = $(WORK_DIR)/build/$(TOPNAME)
$(shell mkdir -p $(DST_DIR))

# Compilation targets
OBJ_DIR = $(DST_DIR)/obj_dir
NVBOARD_TARGET = $(DST_DIR)/$(TOPNAME)

# Source files
NXDC_FILES = constr/top.nxdc

SRC_AUTO_BIND = $(abspath $(DST_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

VERILATOR_VSRC = $(shell find $(abspath ./vsrc) -name "*.v")
VERILATOR_VSRC += $(shell find $(abspath ./vsrc) -name "*.sv")
VERILATOR_CSRC = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
#VERILATOR_CSRC += $(SRC_AUTO_BIND)

IMG ?=

# 2.Compliation Flags

VERILATOR = verilator

INC_PATH ?=
INC_PATH += $(abspath csrc/include)

VERILATOR_FLAGS += -MMD --build -cc \
								 -O3 --x-assign fast --x-initial fast --noassert \
							 -j	 --trace

# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
LDFLAGS += -lreadline


# 3. Include the rules for NVBoard and git

# rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk
# rules for git
include ../Makefile

# 4. Simulation

# NPC with no NVBOARD
npc: $(VERILATOR_VSRC) $(VERILATOR_CSRC)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
	 --Mdir $(OBJ_DIR) --exe -o $(abspath $(NVBOARD_TARGET))
	$(NVBOARD_TARGET) $(IMG)


# simulation with NVBOARD
$(NVBOARD_TARGET): $(VERILATOR_VSRC) $(VERILATOR_CSRC) $(NVBOARD_ARCHIVE)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_FLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
	 --Mdir $(OBJ_DIR) --exe -o $(abspath $(NVBOARD_TARGET)) 

run: $(NVBOARD_TARGET)
	@$^

# 5. Miscellaneous

.PHONY: clean

clean:
	rm -rf $(WORK_DIR)/build
