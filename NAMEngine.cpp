#include "NAMEngine.hpp"
#include <cstring>

// Include ONLY the core headers you actually call.
// Example placeholders (adjust to real API names/paths in your checked-out revision):
// #include "../../extern/NeuralAmpModelerPlugin/NeuralAmpModelerCore/Model.hpp"
// #include "../../extern/NeuralAmpModelerPlugin/NeuralAmpModelerCore/Processor.hpp"

bool NAMEngine::loadModel(const char* /*path*/) {
    // TODO: call into NAM Core's loader if you want a default .nam
    return true;
}

void NAMEngine::process(const float** in, float** out, uint32_t chans, uint32_t frames) {
    // TODO: replace with NAM Core block processing
    for (uint32_t c=0; c<chans; ++c) {
        if (!in[c] || !out[c]) continue;
        std::memcpy(out[c], in[c], sizeof(float)*frames);
    }
}
