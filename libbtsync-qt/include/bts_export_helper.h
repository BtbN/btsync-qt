#pragma once

#ifdef _WIN32
# ifdef libbtsync_qt_EXPORTS
#  define LIBBTS_EXPORT __declspec(dllexport)
# else
#  define LIBBTS_EXPORT __declspec(dllimport)
# endif
#else
# define LIBBTS_EXPORT
#endif
