#include "dsp.h"
#include <NeuralAudio/NeuralModel.h> // adjust path as needed
#include <cmath>
#include <stdexcept>

NamDsp::~NamDsp() = default;

bool NamDsp::loadModel(const std::string& path)
{
    try {
        // if empty path, unload
        if (path.empty()) {
            model_.reset();
            return true;
        }
        auto* raw = NeuralAudio::NeuralModel::CreateFromFile(path.c_str());
        if (!raw)
            return false;

        model_.reset(raw);
        return true;
    }
    catch (const std::exception&) {
        // handle error
        return false;
    }
}

void NamDsp::process(float* in, float* out, uint32_t nframes)
{
    if (!model_) {
        // pass-through if no model loaded
        for (uint32_t i = 0; i < nframes; i++)
            out[i] = in[i];
        return;
    }

    // optional smoothing to inputLevel_ if you want
    // do the actual NAM process
    model_->Process(in, out, nframes);
    // optional smoothing to outputLevel_ if you want
}
