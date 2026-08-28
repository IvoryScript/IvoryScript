#
# Gnu make file to build the IvovryScript compiler suite: ivorysc
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

ifdef ARM_ELF
DEFINES =  -DIVORY -DENABLE_ASSERT -DNO_STDIO -DMINIMAL_IOSTREAM 
else
DEFINES = -DIVORY -DGLOBAL_VM=1 -DRUN_TIME_TYPE_CHECKS=0 -DNO_INTERP_EXCEPTIONS -DFRAME_DESCRIPTORS=1 -DGARBAGE_COLLECTION=1 -DSERIALISATION=1 -DINTERPRETER=1

endif

objectDir = ../../../../build/gcc/$(PROC_TYPE)/obj/app/ivory/ivorysc

libDir = ../../../../build/gcc/$(PROC_TYPE)/lib/API

executableDir = ../../../../build/gcc/$(PROC_TYPE)/app/ivory

includes = -I../../../header

sources = $(wildcard *.cpp)
objects :=

all:	$(executableDir)/ivorysc

define makeObject =
$source = $(1)
object = $(objectDir)/$(source:.cpp=.o)
objects := $(objects) $$(object)
dependencyRule=$(objectDir)/$(source:.cpp=.d)
dependencies += $(dependencyRule)
$$(object): $$(source)
	$(CXX) $(DEPFLAGS) $(CFLAGS) $(DEFINES) $(includes) $$< -o $$@
-include $(dependencyRule)
endef

$(foreach source,$(sources),$(eval $(call makeObject,$(source))))

$(objectDir) :
	mkdir -p $(objectDir)

$(libDir)/ivory/common.lib:
	cd ../../../API/ivory/common && $(MAKE)

$(libDir)/ivory/compiler.lib:
	cd ../../../API/ivory/compiler && $(MAKE)

$(libDir)/archive.lib:
	cd ../../../API/archive && $(MAKE)

$(libDir)/gen.lib:
	cd ../../../API/gen && $(MAKE)

$(libDir)/MSA.lib:
	cd ../../../API/MSA && $(MAKE)

$(libDir)/nameTable.lib:
	cd ../../../API/nameTable && $(MAKE)

$(libDir)/segmentTable.lib:
	cd ../../../API/segmentTable && $(MAKE)

$(libDir)/objStore.lib:
	cd ../../../API/objStore && $(MAKE)


libraries = $(libDir)/ivory/common.lib\
            $(libDir)/ivory/compiler.lib\
	         $(libDir)/archive.lib\
	         $(libDir)/gen.lib\
	         $(libDir)/MSA.lib\
	         $(libDir)/nameTable.lib\
	         $(libDir)/segmentTable.lib\
	         $(libDir)/objStore.lib

$(executableDir) :
	mkdir -p $(executableDir)

$(executableDir)/ivorysc:  $(objectDir) $(objects) $(libraries) $(executableDir)
	gcc $(ARCH_FLAG) $(objects) -L$(libDir)/ivory -lcompiler -lcommon -L$(libDir) -larchive -lgen -lMSA -lnameTable -lsegmentTable -lobjStore -lstdc++ -lm -o $@

clean:
	rm -rf $(objectDir)/*
	rm -rf $(executableDir)/*
