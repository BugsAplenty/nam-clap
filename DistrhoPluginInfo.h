// DistrhoPluginInfo.h - Plugin metadata for DPF
#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_BRAND           "YourName"
#define DISTRHO_PLUGIN_NAME            "NAM"
#define DISTRHO_PLUGIN_URI             "https://yourname.com/plugins/nam"

#define DISTRHO_PLUGIN_HAS_UI          1
#define DISTRHO_PLUGIN_IS_RT_SAFE      1
#define DISTRHO_PLUGIN_NUM_INPUTS      2
#define DISTRHO_PLUGIN_NUM_OUTPUTS     2
#define DISTRHO_PLUGIN_WANT_PROGRAMS   0
#define DISTRHO_PLUGIN_WANT_STATE      1
#define DISTRHO_PLUGIN_WANT_FULL_STATE 1

// Version number (major.minor.micro)
#define DISTRHO_PLUGIN_VERSION         d_version(1, 0, 0)

// Number of parameters
#define DISTRHO_PLUGIN_NUM_PARAMETERS  2

// CLAP-specific features
#define DISTRHO_PLUGIN_CLAP_ID         "com.yourname.nam"
#define DISTRHO_PLUGIN_CLAP_FEATURES   "audio-effect", "amp-simulator", "distortion"

// Plugin type
enum Parameters {
    kParameterInputGain = 0,
    kParameterOutputGain,
    kParameterCount
};

#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED