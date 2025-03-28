
#include <cstring>          // for strcmp
#include "plugin.h"         // for getPluginDescriptor(), createPlugin
#include "clap/clap.h"      // if you have CLAP headers in your include path

// We'll define a static plugin_factory
static uint32_t getPluginCount(const clap_plugin_factory *factory) {
    (void)factory;
    return 1; // we have exactly one plugin
}

static const clap_plugin_descriptor *getPluginDescriptor(const clap_plugin_factory *factory,
                                                         uint32_t index) {
    (void)factory;
    if (index == 0)
        return getPluginDescriptor(); // from plugin.cpp
    return nullptr;
}

static const clap_plugin *factoryCreate(const clap_plugin_factory *factory,
                                        const clap_host *host,
                                        const char *plugin_id) {
    (void)factory;
    // compare ID to our descriptor
    if (strcmp(plugin_id, getPluginDescriptor()->id) == 0) {
        return createPlugin(host); // from plugin.cpp
    }
    return nullptr;
}

// The plugin_factory struct
static const clap_plugin_factory s_factory = {
    .get_plugin_count = getPluginCount,
    .get_plugin_descriptor = getPluginDescriptor,
    .create_plugin = factoryCreate,
};

// The plugin_entry
static bool entryInit(void) {
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
extern "C" const struct clap_plugin_entry *clap_entry(void) {
    static const clap_plugin_entry s_entry = {
        CLAP_VERSION,
        entryInit,
        entryDeinit,
        entryGetFactory,
    };
    return &s_entry;
}
