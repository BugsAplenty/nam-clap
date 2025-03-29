#pragma once
#include "clap/clap.h"

const clap_plugin_descriptor *namClapGetDescriptor();
const clap_plugin *namClapCreate(const clap_host *host);
