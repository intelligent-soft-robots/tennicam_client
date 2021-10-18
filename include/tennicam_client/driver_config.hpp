#pragma once

#include <array>
#include <sstream>
#include <string>
#include "tennicam_client/toml/toml.hpp"

namespace tennicam_client
{
/**
 * Class which encapsulates the configuration for a Driver,
 * i.e. hostname, port and transform.
 */
class DriverConfig
{
public:
    /**
     * @brief construct an instance with undefined attributes
     */
    DriverConfig();
    DriverConfig(std::string server_hostname,
                 int server_port,
                 std::array<double, 3> translation,
                 std::array<double, 3> rotation);
    /**
     * returns tcp:://server_hostname:server_port
     */
    std::string get_url() const;

public:
    std::string server_hostname;
    int server_port;
    std::array<double, 3> translation;
    std::array<double, 3> rotation;

public:
    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(server_hostname, server_port, translation, rotation);
    }
};

/**
 * toml_config_file being an absolute path to a toml configuration file,
 * this parses the file and returns the corresponding instance of
 * DriverConfig. Example of toml configuration file:
 * https://github.com/intelligent-soft-robots/tennicam_client/blob/master/config/config.toml
 */
DriverConfig parse_toml(const std::string& toml_config_file);

/**
 * toml_config_file being an absolute path to a toml configuration file,
 * overwrite the translation and rotation attributes specified by the
 * configuration file.
 */
void update_transform_config_file(std::string file_path,
                                  const std::array<double, 3>& translation,
                                  const std::array<double, 3>& rotation);

}  // namespace tennicam_client
