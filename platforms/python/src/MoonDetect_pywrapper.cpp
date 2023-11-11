#include "../include/MoonDetect_pywrapper.hpp"


boost::shared_ptr<mr::MoonDetector> new_MoonDetector(const py::object& data)
{
    // input bytes
    if (PyBytes_Check(data.ptr()))
    {
        std::string val = py::extract<std::string>(data);
        return boost::shared_ptr<mr::MoonDetector>(new mr::MoonDetector(std::vector<unsigned char>(val.begin(), val.end())));
    }
    // input str
    else
    {
        std::string val = py::extract<std::string>(data);
        return boost::shared_ptr<mr::MoonDetector>(new mr::MoonDetector(val));
    }
    // input MatLike
    // else {}
}

void wrap_MoonDetect_init_by_byte(mr::MoonDetector& self, py::object& data)
{
    std::string val = py::extract<std::string>(data);
    self.init_by_byte(std::vector<unsigned char>(val.begin(), val.end()));
}
