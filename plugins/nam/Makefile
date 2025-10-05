# DPF plugin Makefile — builds a CLAP bundle
# Nix passes:
#   DPF_PATH (path to DPF source)
#   NAM_PATH (path to BugsAplenty/NeuralAmpModelerCore)
#   EXTRA_CFLAGS (Eigen + nlohmann include dirs)

DPF_PATH ?= ../../.deps/DPF
NAM_PATH ?= ../../.deps/NAM

include $(DPF_PATH)/dpf/Makefile.base.mk

NAME    = NAM
TARGETS = clap

# Our plugin sources
FILES_DSP = \
  NAMPlugin.cpp \
  NAMEngine.cpp

# Gather all NAM Core sources (adjust globs if your fork differs)
# If a dir doesn't exist, 'find' will be empty — that's fine.
NAM_CORE_CPP := $(shell find $(NAM_PATH)/NeuralAmpModelerCore -type f -name '*.cpp' 2>/dev/null)
ADSP_CPP     := $(shell find $(NAM_PATH)/AudioDSPTools       -type f -name '*.cpp' 2>/dev/null)

FILES_DSP += $(NAM_CORE_CPP) $(ADSP_CPP)

# Include dirs for NAM Core; Eigen/nlohmann come via EXTRA_CFLAGS
INCLUDE_DIRS += \
  $(NAM_PATH) \
  $(NAM_PATH)/NeuralAmpModelerCore \
  $(NAM_PATH)/AudioDSPTools

# Optimizations & external headers
BUILD_CXX_FLAGS += -DNDEBUG -O3 -ffast-math $(EXTRA_CFLAGS)

# Optional post-bundle assets (models, IRs, etc.)
POST_BUILD = \
  mkdir -p bin/$(NAME).clap/Resources && \
  if [ -d models ]; then cp -r models bin/$(NAME).clap/Resources/models; fi && \
  if [ -d resources ]; then cp -r resources/* bin/$(NAME).clap/Resources/ 2>/dev/null || true; fi

include $(DPF_PATH)/dpf/Makefile.plugins.mk
