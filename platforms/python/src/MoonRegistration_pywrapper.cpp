#include <pybind11/pybind11.h>
namespace py = pybind11;

#include "MoonRegistration/mrapi.hpp"

#include "ndarray_converter.h"
#include "cvvec3f_vector_ndarray.hpp"
#include "shapes_pywrapper.hpp"
#include "utils_pywrapper.hpp"
#include "MoonDetect_pywrapper.hpp"
#include "preprocessing_pywrapper.hpp"




PYBIND11_MODULE(MoonRegistration_pywrapper, module)
{
    NDArrayConverter::init_numpy();
    
    
    // version.hpp
    module.def("version", mr::version, R"pbdoc(MoonRegistration Library Version)pbdoc");
    
    
    py::module shapes_module = module.def_submodule("shapes", "shapes description");
    init_shapes(shapes_module);
    
    
    py::module utils_module = module.def_submodule("utils", "utils description");
    init_utils(utils_module);
    
    
    py::module preprocessing_module = module.def_submodule("preprocessing", "preprocessing description");
    init_preprocessing(preprocessing_module);
    
    
    py::module MoonDetect_module = module.def_submodule("MoonDetect", "MoonDetect description");
    init_MoonDetect(MoonDetect_module);
    
}

