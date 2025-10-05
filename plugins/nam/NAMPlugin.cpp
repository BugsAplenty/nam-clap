#include "NAMPlugin.hpp"
#include <algorithm>
#include <cmath>

START_NAMESPACE_DISTRHO

NAMPlugin::NAMPlugin()
: Plugin(DISTRHO_PLUGIN_NUM_PARAMETERS, 0, 0) {}

void NAMPlugin::initParameter(uint32_t index, Parameter& p) {
    if (index == kParamInputGain) {
        p.hints = kParameterIsAutomable;
        p.name = "Input Gain";
        p.symbol = "ingain";
        p.unit = "dB";
        p.ranges.min = -24.f;
        p.ranges.max =  24.f;
        p.ranges.def =   0.f;
    }
}

void NAMPlugin::activate() {
    engine_.setSampleRate(getSampleRate());
    // Optionally load a default model bundled in Resources/models
    // engine_.loadModel("Resources/models/default.nam");
}

void NAMPlugin::run(const float** inputs, float** outputs, uint32_t frames) {
    const uint32_t chans = std::min<uint32_t>(2, getAudioInputCount());
    const float lin = std::pow(10.f, gainDb_ / 20.f);

    // pre-gain & copy input → output (planar buffers)
    for (uint32_t c=0; c<chans; ++c) {
        const float* in = inputs[c];
        float* out      = outputs[c];
        for (uint32_t i=0; i<frames; ++i)
            out[i] = in[i] * lin;
    }

    // Process with NAM core (NAMEngine will call the core)
    engine_.process(const_cast<const float**>(outputs), outputs, chans, frames);
}

float NAMPlugin::getParameterValue(uint32_t index) const {
    if (index == kParamInputGain) return gainDb_;
    return 0.f;
}

void NAMPlugin::setParameterValue(uint32_t index, float v) {
    if (index == kParamInputGain) gainDb_ = v;
}

Plugin* createPlugin() { return new NAMPlugin(); }

END_NAMESPACE_DISTRHO
