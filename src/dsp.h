#pragma once

#include <cstdint>
#include <string>
#include <cmath>
#include <memory>
#include <vector>
#include <NeuralAudio/NeuralModel.h>  // Update include path

class AudioBuffer {
public:
    AudioBuffer(uint32_t channels) : _channels(channels) {}
    
    void resize(uint32_t frames) {
        _data.resize(frames * _channels);
    }
    
    float* getChannel(uint32_t channel) {
        return &_data[channel * (_data.size() / _channels)];
    }
    
private:
    uint32_t _channels;
    std::vector<float> _data;
};

class NamDsp
{
public:
    NamDsp() = default;
    ~NamDsp();

    // Attempt to load a model from disk
    // In LV2, you did this in a worker thread. In CLAP, you could use the thread-pool extension.
    bool loadModel(const std::string& path);

    // Process `nframes` of audio in place (or with separate in/out)
    void process(float* in, float* out, uint32_t nframes);

    // Simple placeholders for example
    float getInputDbAdjustment() const  { return (model_ ? model_->GetRecommendedInputDBAdjustment() : 0.f); }
    float getOutputDbAdjustment() const { return (model_ ? model_->GetRecommendedOutputDBAdjustment() : 0.f); }

private:
    std::unique_ptr<NeuralAudio::NeuralModel> model_;
    // store any smoothing state, etc.
    float inputLevel_ = 1.f;
    float outputLevel_= 1.f;
};
