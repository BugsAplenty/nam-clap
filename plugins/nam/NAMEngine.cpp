#include "NAMEngine.hpp"
#include <cstring>

/* 
 * Here you include & call into your fork’s core.
 * The exact headers/types depend on BugsAplenty/NeuralAmpModelerCore.
 * Because we compile the whole core into the plugin (Makefile globs),
 * you only need to include the public headers you use.
 *
 * Examples (adjust to the actual API of your fork):
 *   #include "NeuralAmpModelerCore/Model.hpp"
 *   #include "NeuralAmpModelerCore/Processor.hpp"
 *   #include "AudioDSPTools/Filters/OnePole.hpp"
 */

bool NAMEngine::loadModel(const char* /*path*/) {
    // TODO: use your core’s loader if you want a default model
    return true;
}

void NAMEngine::process(const float** in, float** out, uint32_t chans, uint32_t frames) {
    // TODO: call into your core’s block processor here.
    // For now, passthrough:
    for (uint32_t c=0; c<chans; ++c) {
        if (!in[c] || !out[c]) continue;
        std::memcpy(out[c], in[c], sizeof(float)*frames);
    }
}
