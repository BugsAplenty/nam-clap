#pragma once

#if defined _WIN32 || defined __CYGWIN__
  #define CLAP_EXPORT __declspec(dllexport)
#else
  #define CLAP_EXPORT __attribute__((visibility("default")))
#endif