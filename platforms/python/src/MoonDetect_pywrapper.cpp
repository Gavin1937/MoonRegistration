#include "../include/MoonDetect_pywrapper.hpp"


std::unique_ptr<mr::MoonDetector> new_MoonDetector(const py::object& data)
{
    // https://stackoverflow.com/a/8142943
    // https://stackoverflow.com/a/62310838
    
    std::unique_ptr<mr::MoonDetector> self(new mr::MoonDetector());
    
    // input bytes
    if (py::isinstance<py::bytes>(data))
    {
        std::string val = py::cast<std::string>(data);
        self->init_by_byte(std::vector<unsigned char>(val.begin(), val.end()));
    }
    // input str
    else if (py::isinstance<py::str>(data))
    {
        std::string val = py::cast<std::string>(data);
        self->init_by_path(val);
    }
    
    return self;
}

void wrap_MoonDetect_init_by_byte(mr::MoonDetector& self, const py::bytes& data)
{
    std::string val = py::cast<std::string>(data);
    self.init_by_byte(std::vector<unsigned char>(val.begin(), val.end()));
}
