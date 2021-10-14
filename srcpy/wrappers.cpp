#include "o80/pybind11_helper.hpp"
#include "tennicam_client/driver_config.hpp"  // update_transform_config_file
#include "tennicam_client/standalone.hpp"
#include "tennicam_client/transform.hpp"  // read/write_transform_from/to_memory

void add_tennicam_client(pybind11::module& m)
{
    m.def("update_transform_config_file",
          &tennicam_client::update_transform_config_file);
    m.def("read_transform_from_memory",
          &tennicam_client::read_transform_from_memory);
    m.def("write_transform_to_memory",
          &tennicam_client::write_transform_to_memory);
}

void add_observation(pybind11::module& m)
{
    typedef o80::Observation<1, tennicam_client::Ball, o80::VoidExtendedState>
        observation;

    pybind11::class_<observation>(m, "Observation")
        .def(pybind11::init<>())
        .def("get_observed_states", &observation::get_observed_states)
        .def("get_desired_states", &observation::get_desired_states)
        .def("get",
             [](observation& obs) { return obs.get_observed_states().get(0); })
        .def("get_position",
             [](observation& obs) {
                 return obs.get_observed_states().get(0).get_position();
             })
        .def("get_velocity",
             [](observation& obs) {
                 return obs.get_observed_states().get(0).get_velocity();
             })
        .def("get_iteration", &observation::get_iteration)
        .def("get_time_stamp",
             [](observation& obs) {
                 return obs.get_observed_states().get(0).get_time_stamp();
             })
        .def("get_ball_id", [](observation& obs) {
            return obs.get_observed_states().get(0).get_ball_id();
        });
}

PYBIND11_MODULE(tennicam_client, m)
{
    // adding update_transform_config_file, read_transform_from_memory
    // and write transform to memory
    add_tennicam_client(m);
    o80::create_python_bindings<tennicam_client::Standalone,
                                o80::NO_OBSERVATION>(m);
    // the standard API for o80::Observation is not convenient for this case, so
    // creating another simpler one.
    add_observation(m);
    // o80 standalone
    o80::create_standalone_python_bindings<
        tennicam_client::Driver,
        tennicam_client::Standalone,
        std::string,  // argument for the driver (path to toml file)
        std::string>  // argument for the driver (active transform)
        (m);
}
