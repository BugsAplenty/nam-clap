#include <cstring>          // for strcmp
#include "plugin.h"    // for namClapGetDescriptor(), namClapCreate()
#include "clap/clap.h"
#include "visibility.h"

// Ensure proper symbol visibility
#ifndef CLAP_EXPORT
    #ifdef _WIN32
        #define CLAP_EXPORT __declspec(dllexport)
    #else
        #define CLAP_EXPORT __attribute__((visibility("default")))
    #endif
#endif

// We'll define a static plugin_factory
static uint32_t getPluginCount(const clap_plugin_factory *factory) {
    (void)factory;
    return 1; // we have exactly one plugin
}

static const clap_plugin_descriptor *getPluginDescriptor(const clap_plugin_factory *factory,
                                                         uint32_t index) {
    (void)factory;
    if (index == 0)
        return namClapGetDescriptor(); // from plugin.cpp
    return nullptr;
}

static const clap_plugin *factoryCreate(const clap_plugin_factory *factory,
                                        const clap_host *host,
                                        const char *plugin_id) {
    (void)factory;
    // compare ID to our descriptor
    if (strcmp(plugin_id, namClapGetDescriptor()->id) == 0) {
        return namClapCreate(host); // from plugin.cpp
    }
    return nullptr;
}

// The plugin_factory struct
static const clap_plugin_factory s_factory = {
    .get_plugin_count     = getPluginCount,
    .get_plugin_descriptor= getPluginDescriptor,
    .create_plugin        = factoryCreate,
};

// The plugin_entry
static bool entryInit(const char *plugin_path) {
    // global init if needed
    return true;
}

static void entryDeinit(void) {
    // global cleanup if needed
}

static const void *entryGetFactory(const char *factory_id) {
    if (strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) == 0) {
        return &s_factory;
    }
    return nullptr;
}

// Export the clap_entry symbol
extern "C" {
    CLAP_EXPORT const struct clap_plugin_entry clap_entry = {
        .clap_version = CLAP_VERSION_INIT,
        .init = entryInit,
        .deinit = entryDeinit,
        .get_factory = entryGetFactory
    };
}
