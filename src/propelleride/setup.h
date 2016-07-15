#pragma once

#if defined(Q_OS_WIN) || defined(CYGWIN)
  #define APP_EXTENSION            ".exe"
  #define APP_RESOURCES_PATH        "/"

#elif defined(Q_OS_MAC)
  #define APP_EXTENSION            ""
  #define APP_RESOURCES_PATH        "/../Resources"

#elif defined(Q_OS_UNIX)
  #define APP_EXTENSION            ""
  #define APP_RESOURCES_PATH       "/../share/propelleride"

#else
  #error "OS not yet supported"
#endif

