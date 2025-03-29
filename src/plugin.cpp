#include "plugin.h"
#include "dsp.h"
#include <cstring>  // for std::strcmp
#include <cmath>
#include <cstdio>   // for logging to stderr, if desired

// Example plugin descriptor data
static const clap_plugin_descriptor_t descriptor = {
    .clap_version = CLAP_VERSION,  // Use the predefined version
    .id = "com.fyzyx.nam-clap",
    .name = "NAM CLAP",
    .vendor = "FYZ ZYX",
    .url = "https://github.com/BugsAplenty/nam-clap",
    .manual_url = "",
    .support_url = "",
    .version = "0.1.0",
    .description = "Neural Amp Modeler as a CLAP plugin",
    .features = (const char *[]){ 
        CLAP_PLUGIN_FEATURE_AUDIO_EFFECT,
        CLAP_PLUGIN_FEATURE_DISTORTION,
        CLAP_PLUGIN_FEATURE_MONO,
        CLAP_PLUGIN_FEATURE_STEREO,
        nullptr 
    }
};

// Our internal data structure
struct NamClap {
    clap_plugin plugin;   // must be first
    const clap_host *host;

    NamDsp dsp;
    AudioBuffer inBuffer;
    AudioBuffer outBuffer;
    
    float inputGain = 0.f;
    float outputGain = 0.f;
    double sample_rate = 0.0;
    bool active = false;

    NamClap() : inBuffer(2), outBuffer(2) {}  // Initialize with stereo support
};

// --------------- CLAP Callbacks ---------------

// Called after creation, prior to audio processing
static bool pluginInit(const clap_plugin *plugin)
{
    auto *self = reinterpret_cast<NamClap*>(plugin->plugin_data);
    // Initialize NAM DSP
    if (!self) {
        return false;
    }
    
    // If you want to load a default .nam file, do that here:
    // if (!self->dsp.loadModel("/path/to/default.nam")) {
    //     return false;
    // }
    
    return true;
}

static void pluginDestroy(const clap_plugin *plugin)
{
    auto *self = reinterpret_cast<NamClap*>(plugin->plugin_data);
    delete self;
}

static bool pluginActivate(const clap_plugin *plugin, double sample_rate, uint32_t min_frames, uint32_t max_frames) {
    auto *self = reinterpret_cast<NamClap*>(plugin->plugin_data);
    if (!self) return false;
    
    self->active = true;
    self->sample_rate = sample_rate;
    return true;
}

static void pluginDeactivate(const clap_plugin *plugin)
{
    auto *self = reinterpret_cast<NamClap*>(plugin->plugin_data);
    self->active = false;
    // do any cleanup if needed
}

// This is called every audio block
static clap_process_status pluginProcess(const clap_plugin *plugin, const clap_process *process)
{
    auto *self = reinterpret_cast<NamClap*>(plugin->plugin_data);
    if (!self->active)
        return CLAP_PROCESS_CONTINUE;

    // If no audio I/O, just skip
    if (process->audio_inputs_count == 0 || process->audio_outputs_count == 0)
        return CLAP_PROCESS_CONTINUE;

    const uint32_t nframes = process->frames_count;
    const uint32_t inch = process->audio_inputs[0].channel_count;
    const uint32_t outch= process->audio_outputs[0].channel_count;
    // handle as many channels as the plugin wants
    uint32_t channels = (inch < outch) ? inch : outch;

    float inLevel  = std::pow(10.0f, (self->inputGain  + self->dsp.getInputDbAdjustment())  * 0.05f);
    float outLevel = std::pow(10.0f, (self->outputGain + self->dsp.getOutputDbAdjustment()) * 0.05f);

    for (uint32_t c = 0; c < channels; c++) {
        const float *inbuf  = process->audio_inputs[0].data32[c];
        float *outbuf = process->audio_outputs[0].data32[c];

        // 1) apply input gain
        // 2) run NAM DSP
        // 3) apply output gain

        // you could do an in-place approach:
        // copy from inbuf to a small buffer if you prefer, or just pass in/out
        // directly, applying input gain first.
        // For simplicity, let's do it in two steps:

        // step1: create local buffer to store "inputGain * inbuf"
        std::vector<float> temp(nframes);
        for (uint32_t i = 0; i < nframes; i++)
            temp[i] = inbuf[i] * inLevel;

        // step2: pass through the NAM model
        self->dsp.process(temp.data(), outbuf, nframes);

        // step3: apply output gain in place
        for (uint32_t i = 0; i < nframes; i++)
            outbuf[i] *= outLevel;
    }

    return CLAP_PROCESS_CONTINUE;
}

// You can add more callbacks for params, GUI, state, etc.
// but let's keep it minimal for now.

static const void *pluginGetExtension(const clap_plugin *plugin, const char *extension)
{
    // e.g., if you implement CLAP_EXT_STATE, CLAP_EXT_GUI, etc.
    // if (!strcmp(extension, CLAP_EXT_STATE)) return &my_state_callbacks;
    return nullptr;
}

static void pluginOnMainThread(const clap_plugin *plugin)
{
    // If you need to do main-thread actions (like open a file dialog?), handle it here.
}

// --------------- Plugin Factory Functions ---------------

const clap_plugin_descriptor *namClapGetDescriptor() {
    return &descriptor;
}

const clap_plugin *namClapCreate(const clap_host *host)
{
    // allocate + zero
    NamClap *self = new NamClap();  // Use constructor instead of memset

    // fill plugin vtable
    self->plugin.desc           = &descriptor;
    self->plugin.plugin_data    = self;
    self->plugin.init           = pluginInit;
    self->plugin.destroy        = pluginDestroy;
    self->plugin.activate       = pluginActivate;
    self->plugin.deactivate     = pluginDeactivate;
    self->plugin.process        = pluginProcess;
    self->plugin.get_extension  = pluginGetExtension;
    self->plugin.on_main_thread= pluginOnMainThread;
    // no params, GUI, etc. yet
    self->host = host;

    return &self->plugin;
}

static const clap_plugin_factory_t plugin_factory = {
    .get_plugin_count = [](const clap_plugin_factory *factory) -> uint32_t { 
        return 1; 
    },
    .get_plugin_descriptor = [](const clap_plugin_factory *factory, uint32_t index) -> const clap_plugin_descriptor_t * {
        if (index == 0) return namClapGetDescriptor();
        return nullptr;
    },
    .create_plugin = [](const clap_plugin_factory *factory, const clap_host *host, const char *plugin_id) -> const clap_plugin_t * {
        if (!strcmp(plugin_id, descriptor.id))
            return namClapCreate(host);
        return nullptr;
    }
};
