VERILATOR_FLAGS = -cc --exe --build --trace-fst
VERILATOR_VSRC = $(wildcard, vsrc/*.v) 
VERILATOR_CSRC = $(wildcard, csrc/*.c)
VERILATOR_TAR = $(patsubst %.v,%,$(VERILATOR_VSRC))

VERILATOR = verilator
all:
	@echo "Write this Makefile by your self."

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by your self."
sim: $(VERILATOR_CSRC) $(VERILATOR_VSRC)
	$(VERILATOR) $(VERILATOR_FLAGS) -f $(VERILATOR_CSRC) $(VERILATOR_VSRC)
	./obj_dir/$(VERILATOR_TAR)


include ../Makefile
