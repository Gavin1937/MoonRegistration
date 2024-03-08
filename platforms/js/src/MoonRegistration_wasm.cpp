#include <MoonRegistration/version.hpp>


extern "C"
{
    void* mrwasm_version()
    {
        return reinterpret_cast<void*>(const_cast<char*>(mr::version()));
    }
}
