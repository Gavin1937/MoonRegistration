#include <pybind11/pybind11.h>
namespace py = pybind11;

#include "MoonRegistration/mrapi.hpp"

#include "typecvt.hpp"

#include "shapes_pywrapper.hpp"
#include "imgprocess_pywrapper.hpp"
#include "MoonDetect_pywrapper.hpp"
#include "MoonRegistrar_pywrapper.hpp"


PYBIND11_MODULE(MoonRegistration_pywrapper, module)
{
    NDArrayConverter::init_numpy();
    
    
    // version.hpp
    module.def("version", mr::version, R"pbdoc(MoonRegistration Library Version)pbdoc");
    
    
    py::module shapes_module = module.def_submodule("shapes", "shapes description");
    init_shapes(shapes_module);
    
    
    py::module imgprocess_module = module.def_submodule("imgprocess", "imgprocess description");
    init_imgprocess(imgprocess_module);
    
    
    py::module MoonDetect_module = module.def_submodule("MoonDetect", "MoonDetect description");
    init_MoonDetect(MoonDetect_module);
    
    
    py::module MoonRegistrar_module = module.def_submodule("MoonRegistrar", "MoonRegistrar description");
    init_MoonRegistrar(MoonRegistrar_module);
    
}
