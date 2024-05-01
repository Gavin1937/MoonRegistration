#include "MoonRegistration/mrconfig.h"
#include "MoonRegistration/version.hpp"

extern "C"
{

const char* __MR_VERSION_STR__()
{
    return MR_VERSION_STR;
}

int __MR_HAVE_HOUGH_GRADIENT_ALT__()
{
#ifdef MR_HAVE_HOUGH_GRADIENT_ALT
    return 1;
#else
    return 0;
#endif
}

int __MR_ENABLE_OPENCV_NONFREE__()
{
#ifdef MR_ENABLE_OPENCV_NONFREE
    return 1;
#else
    return 0;
#endif
}

int __MR_HAVE_OPENCV_NONFREE__()
{
#ifdef MR_HAVE_OPENCV_NONFREE
    return 1;
#else
    return 0;
#endif
}

}