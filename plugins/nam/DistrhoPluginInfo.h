#pragma once

#define DISTRHO_PLUGIN_NAME        "NAM"
#define DISTRHO_PLUGIN_URI         "https://yourdomain.dev/plugins/nam"
#define DISTRHO_PLUGIN_CLAP_ID     "com.yourname.nam"

#define DISTRHO_PLUGIN_BRAND       "YourName"
#define DISTRHO_PLUGIN_VERSION     0x000100  // 0.1.0

#define DISTRHO_PLUGIN_IS_SYNTH    0
#define DISTRHO_PLUGIN_HAS_UI      0
#define DISTRHO_PLUGIN_IS_RT_SAFE  1

#define DISTRHO_PLUGIN_NUM_INPUTS  2
#define DISTRHO_PLUGIN_NUM_OUTPUTS 2

#define DISTRHO_PLUGIN_NUM_PARAMETERS 1

// Host categorization
#define DISTRHO_PLUGIN_CLAP_FEATURES "audio-effect", "distortion"
