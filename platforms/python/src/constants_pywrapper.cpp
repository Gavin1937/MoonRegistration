#include "MoonRegistration/mrconfig.h"

#include "constants_pywrapper.hpp"


// initialize submodule
void init_constants(py::module &module)
{
    // mrconfig.h
    module.attr("MR_VERSION_STR") = py::str(MR_VERSION_STR);
    
#ifdef MR_HAVE_HOUGH_GRADIENT_ALT
    module.attr("MR_HAVE_HOUGH_GRADIENT_ALT") = py::bool_(true);
#else
    module.attr("MR_HAVE_HOUGH_GRADIENT_ALT") = py::bool_(false);
#endif
    
#ifdef MR_ENABLE_OPENCV_NONFREE
    module.attr("MR_ENABLE_OPENCV_NONFREE") = py::bool_(true);
#else
    module.attr("MR_ENABLE_OPENCV_NONFREE") = py::bool_(false);
#endif
    
#ifdef MR_HAVE_OPENCV_NONFREE
    module.attr("MR_HAVE_OPENCV_NONFREE") = py::bool_(true);
#else
    module.attr("MR_HAVE_OPENCV_NONFREE") = py::bool_(false);
#endif
    
}