#pragma once

#include <optional>
#include <tuple>
#include <stdlib.h>
#include <stdexcept>
#include <cmath>
#include <chrono>
#include <zmq.hpp>
#include <filesystem>
#include "o80/driver.hpp"
#include "json_helper/json_helper.hpp"
#include "tennicam_client/transform.hpp"
#include "tennicam_client/ball.hpp"
#include "tennicam_client/toml/toml.hpp"

namespace tennicam_client
{

  class DriverConfig
  {
  public:
    DriverConfig()
      : server_hostname{"undefined"}{}
    DriverConfig(std::string server_hostname,
		 int server_port,
		 std::array<double,3> translation,
		 std::array<double,3> rotation);
    std::string get_url() const;
    std::string server_hostname;
    int server_port;
    std::array<double,3> translation;
    std::array<double,3> rotation;
  };


  DriverConfig parse_toml(const std::string& toml_config_file);
  
  class DriverIn{};

  class Driver : public o80::Driver<DriverIn, Ball>
  {
  public:

    Driver(std::array<double,3> translation,
	   std::array<double,3> rotation,
	   std::string server_hostname, int server_port);
    Driver(const DriverConfig& config);
    Driver(std::string toml_config_file);
    void start();
    void stop();
    void set(const DriverIn&);
    Ball get();

    const DriverConfig& get_config() const;
    
  private:

    // compute the velocity using finite differences
    std::array<double,3> compute_velocity(long int time_stamp, const std::array<double,3>& position);
    
  private:

    DriverConfig config_;
    Transform transform_;
    std::unique_ptr<zmq::context_t> context_;
    std::unique_ptr<zmq::socket_t> socket_;
    zmq::message_t reply_;
    json_helper::Jsonhelper jh_;
    long int ball_id_;
    long int previous_time_stamp_;
    std::array<double,3> previous_position_;
    std::array<double,3> previous_velocity_;
  };
  
}  
