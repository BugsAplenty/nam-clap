
#include "plugin.h"
#include "dsp.h"  
#include <cstring> 

// We'll define the plugin descriptor here:
static const clap_plugin_descriptor s_descriptor = {
    CLAP_VERSION,
    "com.example.my-nam-clap", // .id
    "My NAM CLAP",             // .name
    "My Company",              // .vendor
    "https://example.com",     // .url
    "1.0.0",                   // .version
    "A minimal NAM-based plugin in C++",  // .description
    nullptr, // manual_url
    nullptr, // support_url
    nullptr, // future extension
    (const char*[]){ CLAP_PLUGIN_FEATURE_AUDIO_EFFECT, nullptr } // features
};

// We expose an accessor for main.cpp
const clap_plugin_descriptor *getPluginDescriptor() {
    return &s_descriptor;
}

// We'll define our plugin data structure
struct NeuralAmpModeler {
    clap_plugin plugin;  // must be first
    NamWrapper dsp;      // reference to our wrapper from dsp.cpp
};

// Callback: init
static bool pluginInit(const clap_plugin *plugin) {
    auto *self = reinterpret_cast<const NeuralAmpModeler*>(plugin->plugin_data);
    // if you want to load a .nam file or do setup, do it now:
    // self->dsp.loadNamFile("some.nam");
    return true;
}

// Callback: destroy
static void pluginDestroy(const clap_plugin *plugin) {
    auto *self = reinterpret_cast<const NeuralAmpModeler*>(plugin->plugin_data);
    delete self;
}

// Callback: process
static clap_process_status pluginProcess(const clap_plugin *plugin,
                                         const clap_process *process) {
    auto *self = reinterpret_cast<const NeuralAmpModeler*>(plugin->plugin_data);

    if (process->audio_inputs_count == 0 || process->audio_outputs_count == 0)
        return CLAP_PROCESS_CONTINUE;

    uint32_t frames = process->frames_count;
    uint32_t inch = process->audio_inputs[0].channel_count;
    uint32_t outch = process->audio_outputs[0].channel_count;

    // For simplicity, process min(inch, outch) channels
    uint32_t channels = (inch < outch) ? inch : outch;

    for (uint32_t c = 0; c < channels; c++) {
        const float *inbuf  = process->audio_inputs[0].data[c];
        float *outbuf       = process->audio_outputs[0].data[c];
        self->dsp.processBlock(inbuf, outbuf, frames);
    }
    return CLAP_PROCESS_CONTINUE;
}

// We define createPlugin for main.cpp
const clap_plugin *createPlugin(const clap_host *host) {
    NeuralAmpModeler *plugin_data = new NeuralAmpModeler;
    std::memset(plugin_data, 0, sizeof(NeuralAmpModeler));

    plugin_data->plugin.desc           = getPluginDescriptor();
    plugin_data->plugin.plugin_data    = plugin_data;
    plugin_data->plugin.init           = pluginInit;
    plugin_data->plugin.destroy        = pluginDestroy;
    plugin_data->plugin.process        = pluginProcess;
    // you can fill other callbacks as needed

    return &plugin_data->plugin;
}
