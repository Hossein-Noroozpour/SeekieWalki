#pragma once

#ifdef ANDROID
#define IN_ANDROID
#elif __unix__
#define IN_LINUX
#elif defined(WIN32) || defined(WIN64)
#define IN_WINDOWS
#endif

#define LOG_ENABLED


#define APPLICATION_NAME "Seeki Walki"