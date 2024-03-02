#include <pybind11/pybind11.h>
namespace py = pybind11;

#include "MoonRegistration/mrapi.hpp"

#include "typecvt.hpp"

#include "shapes_pywrapper.hpp"
#include "imgprocess_pywrapper.hpp"
#include "MoonDetect_pywrapper.hpp"
#include "MoonRegistrate_pywrapper.hpp"


PYBIND11_MODULE(MoonRegistration_pywrapper, module)
{
    NDArrayConverter::init_numpy();
    
    
    // version.hpp
    module.def("version", mr::version, R"pbdoc(MoonRegistration Library Version)pbdoc");
    
    
    py::module shapes_module = module.def_submodule("shapes", "Basic shape structs in the library.");
    init_shapes(shapes_module);
    
    
    py::module imgprocess_module = module.def_submodule("imgprocess", "Basic image processing functions.");
    init_imgprocess(imgprocess_module);
    
    
    py::module MoonDetect_module = module.def_submodule("MoonDetect", "MoonDetect Module, detect moon from input image.");
    init_MoonDetect(MoonDetect_module);
    
    
    py::module MoonRegistrate_module = module.def_submodule("MoonRegistrate", "MoonRegistrate Module, run image registration on input image.");
    init_MoonRegistrate(MoonRegistrate_module);
    
}
