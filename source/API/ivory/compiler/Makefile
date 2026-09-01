#
# Gnu make file to build the Ivory System compiler library
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

includes = -I../../../header/

objectDir = ../../../../build/gcc/$(PROC_TYPE)/obj/API/ivory/compiler/

libraryDir = ../../../../build/gcc/$(PROC_TYPE)/lib/API/ivory/

ifdef ARM_ELF
DEFINES =  -DIVORY -DENABLE_ASSERT -DNO_STDIO -DMINIMAL_IOSTREAM 
else
DEFINES = -DIVORY -DGLOBAL_VM=1 -DRUN_TIME_TYPE_CHECKS=0 -DNO_INTERP_EXCEPTIONS -DFRAME_DESCRIPTORS=1 -DCLOSURE_UPDATE=1 -DGARBAGE_COLLECTION=1 -DSERIALISATION=1 -DINTERPRETER=1
endif

define run-gcc
$(CXX) $(DEPFLAGS) $(CFLAGS) $(DEFINES) $(includes) $^ -o $@
endef

sources = $(wildcard *.cpp)
sources += $(wildcard parser/*.cpp)
objects :=

all:	$(libraryDir)libcompiler.a

define makeObject =
$source = $(1)
object = $(objectDir)$(source:.cpp=.o)
objects := $(objects) $$(object)
dependencyRule=$(objectDir)$(source:.cpp=.d)
dependencies += $(dependencyRule)
$$(object): $$(source)
	$(CXX) $(DEPFLAGS) $(CFLAGS) $(DEFINES) $(includes) $$< -o $$@
-include $(dependencyRule)
endef

$(foreach source,$(sources),$(eval $(call makeObject,$(source))))

$(objectDir) :
	mkdir -p $(objectDir)

$(objectDir)/parser :
	mkdir -p $(objectDir)/parser

$(libraryDir) :
	mkdir -p $(libraryDir)

$(libraryDir)libcompiler.a: $(libraryDir) $(objectDir) $(objectDir)/parser $(objects) 
	rm -rf $(libraryDir)libcompiler.a
	$(AR) $(AR_FLAGS) $(libraryDir)libcompiler.a $(objects)

.PHONY: clean
clean:
	rm -rf $(objectDir)
	rm -rf $(libraryDir)libcompiler.a

.PHONY: cleandep
cleandep:
	rm -f $(dependencies)


