#pragma once
#include <cstdint>

class NAMEngine {
public:
    bool  loadModel(const char* path);
    void  setSampleRate(double sr) { sampleRate_ = sr; }
    void  process(const float** in, float** out, uint32_t chans, uint32_t frames);
private:
    double sampleRate_ = 48000.0;
    // TODO: hold your NAM model instance / oversampling / IR here.
};
