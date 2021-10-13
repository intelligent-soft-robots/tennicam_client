#include "o80/pybind11_helper.hpp"
#include "tennicam_client/standalone.hpp"


void add_observation(pybind11::module& m)
{
  typedef o80::Observation<1,tennicam_client::Ball,o80::VoidExtendedState>
    observation;

  pybind11::class_<observation>(m, "Observation")
    .def(pybind11::init<>())
    .def("get_observed_states", &observation::get_observed_states)
    .def("get_desired_states", &observation::get_desired_states)
    .def("get",
	 [](observation& obs) {
	   return obs.get_observed_states().get(0);
	 })
    .def("get_position",
	 [](observation& obs){
	   return obs.get_observed_states().get(0).get_position();
	 })
    .def("get_velocity",
	 [](observation& obs){
	   return obs.get_observed_states().get(0).get_velocity();
	 })
    .def("get_iteration", &observation::get_iteration)
    .def("get_time_stamp",
	 [](observation& obs) {
	   return obs.get_observed_states().get(0).get_time_stamp();
	 })
    .def("get_ball_id",
	 [](observation& obs) {
	   return obs.get_observed_states().get(0).get_ball_id();
	 });
    }

PYBIND11_MODULE(tennicam_client, m)
{
  o80::create_python_bindings<tennicam_client::Standalone,o80::NO_OBSERVATION>(m);
  // the standard API for o80::Observation is not convenient for this case, so
  // creating another simpler one.
  add_observation(m);
  o80::create_standalone_python_bindings<tennicam_client::Driver,
                                           tennicam_client::Standalone,
					   std::string> // argument for the driver (path to toml file)
					   (m);

}
