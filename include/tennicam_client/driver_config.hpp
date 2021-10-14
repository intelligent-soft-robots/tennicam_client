#pragma once

#include <string>
#include <array>
#include <sstream>
#include "tennicam_client/toml/toml.hpp"


namespace tennicam_client
{

  class DriverConfig
  {
  public:
    DriverConfig();
    DriverConfig(std::string server_hostname,
		 int server_port,
		 std::array<double,3> translation,
		 std::array<double,3> rotation);
    std::string get_url() const;
  public:
    std::string server_hostname;
    int server_port;
    std::array<double,3> translation;
    std::array<double,3> rotation;
  public:
    template <class Archive>
    void serialize(Archive& archive)
    {
      archive(server_hostname,server_port,translation,rotation);
    }
  };

  DriverConfig parse_toml(const std::string& toml_config_file);

  void update_transform_config_file(std::string file_path,
				    const std::array<double,3>& translation,
				    const std::array<double,3>& rotation);
  
}
