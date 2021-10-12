#include "o80/pybind11_helper.hpp"
#include "tennicam_client/standalone.hpp"

PYBIND11_MODULE(o80_example, m)
{
    o80::create_standalone_python_bindings<tennicam_client::Driver,
                                           tennicam_client::Standalone,
					   std::string> // argument for the driver (path to toml file)
        (m);
}
