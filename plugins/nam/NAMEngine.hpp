#pragma once
#include <cstdint>

class NAMEngine {
public:
    bool  loadModel(const char* path); // wire to NAM loader if desired
    void  setSampleRate(double sr) { sampleRate_ = sr; }
    void  process(const float** in, float** out, uint32_t chans, uint32_t frames);

private:
    double sampleRate_ = 48000.0;
    // TODO: store NAM model instance, oversampling, IR loader, etc.
};
