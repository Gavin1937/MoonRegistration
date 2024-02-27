#include "MoonRegistration/c_mrapi.h"
#include "MoonRegistration/version.hpp"


extern "C" {

EXPORT_SYMBOL const char* mrc_version()
{
    return mr::version();
}

}
