# =========================
# DPF + NAM Core → CLAP
# Root Makefile (no Nix)
# =========================

# Directory of THIS Makefile (absolute, trailing slash)
ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# CRITICAL: DPF uses DPF_BUILD_DIR and DPF_TARGET_DIR
DPF_BUILD_DIR  := $(ROOT).dpfbuild
DPF_TARGET_DIR := $(ROOT)bin

# Submodules
DPF_PATH := $(ROOT)libs/DPF
NAM_ROOT := $(ROOT)extern/NeuralAmpModelerPlugin

# Find DPF make includes
ifneq ($(wildcard $(DPF_PATH)/Makefile.base.mk),)
  DPF_MK_DIR := $(DPF_PATH)
else ifneq ($(wildcard $(DPF_PATH)/dpf/Makefile.base.mk),)
  DPF_MK_DIR := $(DPF_PATH)/dpf
else
  $(error Could not find DPF makefiles in $(DPF_PATH) or $(DPF_PATH)/dpf)
endif

# Include DPF base
include $(DPF_MK_DIR)/Makefile.base.mk

# Plugin metadata
NAME    = NAM
TARGETS = clap

UI_TYPE = cairo

# DSP files
FILES_DSP = \
  $(ROOT)NAMPlugin.cpp \
  $(ROOT)NAMEngine.cpp

# UI files
FILES_UI = \
  $(ROOT)NAMPluginUI.cpp

# -------- Source discovery --------
define find_cpp
$(shell find $(1) \
  -type d \( -name Dependencies -o -name dependency -o -name tests -o -name test -o -name examples -o -name benchmarks -o -name tools \) -prune -false -o \
  -type f -name '*.cpp')
endef

NAM_CORE_CPP := $(call find_cpp,$(NAM_ROOT)/NeuralAmpModelerCore)
FILES_DSP   += $(NAM_CORE_CPP)

ifneq ($(wildcard $(NAM_ROOT)/AudioDSPTools),)
  ADSP_CPP := $(call find_cpp,$(NAM_ROOT)/AudioDSPTools)
  FILES_DSP += $(ADSP_CPP)
endif

# -------- Include paths --------
EIGEN_DIR := $(firstword \
  $(wildcard $(NAM_ROOT)/eigen) \
  $(wildcard $(NAM_ROOT)/third_party/eigen) \
  $(wildcard $(NAM_ROOT)/NeuralAmpModelerCore/Dependencies/eigen))

JSON_DIR  := $(firstword \
  $(wildcard $(NAM_ROOT)/third_party/json/single_include) \
  $(wildcard $(NAM_ROOT)/NeuralAmpModelerCore/Dependencies/json/single_include) \
  $(wildcard $(NAM_ROOT)/NeuralAmpModelerCore/Dependencies/nlohmann))

RTN_DIR   := $(firstword \
  $(wildcard $(NAM_ROOT)/third_party/RTNeural) \
  $(wildcard $(NAM_ROOT)/NeuralAmpModelerCore/Dependencies/RTNeural))

# Debug output
$(info =====================================)
$(info NAM CLAP Build Configuration)
$(info =====================================)
$(info ROOT=$(ROOT))
$(info DPF_BUILD_DIR=$(DPF_BUILD_DIR))
$(info DPF_TARGET_DIR=$(DPF_TARGET_DIR))
$(info UI_TYPE=$(UI_TYPE))
$(info DPF_MK_DIR=$(DPF_MK_DIR))
$(info EIGEN_DIR=$(EIGEN_DIR))
$(info JSON_DIR=$(JSON_DIR))
$(info RTN_DIR=$(RTN_DIR))
$(info =====================================)

ifeq ($(strip $(EIGEN_DIR)),)
  $(error Could not locate Eigen headers inside $(NAM_ROOT))
endif

# Compiler flags
BUILD_CXX_FLAGS += \
  -I$(NAM_ROOT) \
  -I$(NAM_ROOT)/NeuralAmpModelerCore \
  -I$(EIGEN_DIR) \
  -std=gnu++17 -O3 -ffast-math -DNDEBUG \
  -Wno-unused-parameter

ifneq ($(strip $(JSON_DIR)),)
  BUILD_CXX_FLAGS += -I$(JSON_DIR)
endif
ifneq ($(strip $(RTN_DIR)),)
  BUILD_CXX_FLAGS += -I$(RTN_DIR)
endif
ifneq ($(wildcard $(NAM_ROOT)/AudioDSPTools),)
  BUILD_CXX_FLAGS += -I$(NAM_ROOT)/AudioDSPTools
endif

# Include DPF plugins makefile
include $(DPF_MK_DIR)/Makefile.plugins.mk