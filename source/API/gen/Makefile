#
# Gnu make file to build the Ivory System general library
#

# --- Target Architecture & Toolchain Selection ---
HOST_MACHINE = $(shell uname -m)
HOST_FAMILY = $(if $(filter arm% aarch64 arm64,$(HOST_MACHINE)),arm,x86)
TARGET_FAMILY ?= $(if $(PROC),$(PROC),$(HOST_FAMILY))

ifdef ARM_ELF
TOOL_CHAIN = d:/cygwin/arm2_gcc3.4.0
UTIL_DIR   = $(TOOL_CHAIN)/bin
PROC       = arm
TYPE       = elf
PROC_TYPE  = arm-elf
INCPATH    = $(TOOL_CHAIN)/arm-elf/include
INCGENERIC = -I. -I$(INCPATH) -I$(TOOL_CHAIN)/lib/gcc/arm-elf/3.4.0/include

ifneq ($(filter default undefined,$(origin CXX)),)
CXX = $(TOOL_CHAIN)/bin/$(PROC)-$(TYPE)-g++
endif
ifneq ($(filter default undefined,$(origin CC)),)
CC = $(TOOL_CHAIN)/bin/$(PROC)-$(TYPE)-gcc
endif
ifneq ($(filter default undefined,$(origin AR)),)
AR = $(TOOL_CHAIN)/bin/$(PROC)-$(TYPE)-ar
endif

else
ifneq ($(filter arm arm-linux aarch64 arm64,$(TARGET_FAMILY)),)
ifeq ($(ARCH), 32)
PROC_TYPE = arm-linux
ARCH_FLAG = -marm
ifneq ($(filter default undefined,$(origin CXX)),)
CXX = arm-linux-gnueabihf-g++
endif
ifneq ($(filter default undefined,$(origin CC)),)
CC = arm-linux-gnueabihf-gcc
endif
else
ifneq ($(filter arm%,$(HOST_MACHINE)),)
PROC_TYPE = arm-linux
ARCH_FLAG = -marm
else
PROC_TYPE = aarch64-linux
ARCH_FLAG =
endif
ifneq ($(filter default undefined,$(origin CXX)),)
CXX = g++
endif
ifneq ($(filter default undefined,$(origin CC)),)
CC = gcc
endif
endif
else
ifeq ($(ARCH), 32)
PROC_TYPE = x86-linux
ARCH_FLAG = -m32
else
PROC_TYPE = x86_64-linux
ARCH_FLAG =
endif
ifneq ($(filter default undefined,$(origin CXX)),)
CXX = g++
endif
ifneq ($(filter default undefined,$(origin CC)),)
CC = gcc
endif
endif
endif

# --- Flag Declarations ---
DEP_FLAGS = -MMD -MP
DEPFLAGS  = $(DEP_FLAGS)

ifndef CFLAGS
ifdef ARM_ELF
CFLAGS = -c -Os $(DEP_FLAGS) -mcpu=arm7tdmi -marm -fsigned-char -ffixed-lr -mlong-calls -nostdinc $(INCGENERIC) -fno-builtin
else
ifdef DEBUG
CFLAGS = -c $(ARCH_FLAG) -g $(DEP_FLAGS) -Wno-conversion-null -Wno-logical-op-parentheses -fsigned-char
else
CFLAGS = -c $(ARCH_FLAG) $(DEP_FLAGS) -Ofast -Wno-conversion-null -Wno-logical-op-parentheses -fsigned-char
endif
endif
endif

AR_FLAGS = rc

includes = -I../../header/

objectDir = ../../../build/gcc/$(PROC_TYPE)/obj/API/gen/

libraryDir = ../../../build/gcc/$(PROC_TYPE)/lib/API/

ifdef ARM_ELF
DEFINES =  -DIVORY -DNO_STDIO -DMINIMAL_IOSTREAM 
else
DEFINES = -DIVORY
endif

sources = $(wildcard *.cpp)
objects :=

all:	$(libraryDir)libgen.a

define makeObject =
source_file := $(1)
obj_file := $(objectDir)/$(1:.cpp=.o)
objects += $$(obj_file)
dep_file := $(objectDir)/$(1:.cpp=.d)
$$(obj_file): $$(source_file) | $(objectDir)
	$(CXX) $(DEPFLAGS) $(CFLAGS) $(DEFINES) $(includes) $$< -o $$@
-include $$(dep_file)
endef

$(foreach source,$(sources),$(eval $(call makeObject,$(source))))

$(objectDir) :
	mkdir -p $(objectDir)

$(libraryDir) :
	mkdir -p $(libraryDir)

$(libraryDir)libgen.a: $(libraryDir) $(objectDir) $(objects) 
	rm -rf $(libraryDir)libgen.a
	$(AR) $(AR_FLAGS) $(libraryDir)libgen.a $(objects)

.PHONY: clean
clean:
	rm -rf $(objectDir)
	rm -rf $(libraryDir)libgen.a

.PHONY: cleandep
cleandep:
	rm -f $(dependencies)
