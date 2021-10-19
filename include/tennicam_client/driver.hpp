#pragma once

#include <stdlib.h>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <memory>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <zmq.hpp>
#include <zmqpp/zmqpp.hpp>
#include "json_helper/json_helper.hpp"
#include "o80/driver.hpp"
#include "tennicam_client/ball.hpp"
#include "tennicam_client/driver_config.hpp"
#include "tennicam_client/transform.hpp"

namespace tennicam_client
{
class DriverIn
{
};

/**
 * @brief o80 drivers for tennicam (see:
 * https://github.com/intelligent-soft-robots/tennicam/), i.e. driver that
 * connects to the zmq channel of tennicam and writes corresponding instances of
 * balls in the shared memory.
 */
class Driver : public o80::Driver<DriverIn, Ball>
{
public:
    /**
     * @brief ball will be received from the given hostname and port,
     * i.e. it is assumed tennicam broadcast on
     * tcp:://server_hostname:server_port When received, the ball goes through
     * the specified transform (i.e. translation and rotation) before being
     * written in the shared memory. The rotation is a 3d array of 3 angles in
     * radian.
     */
    Driver(std::array<double, 3> translation,
           std::array<double, 3> rotation,
           std::string server_hostname,
           int server_port);
    Driver(const DriverConfig& config);
    /**
     * @brief config is a path to a toml configuration file specifying
     * the tennicam host and port, as well as the transform.
     * See for example:
     * https://github.com/intelligent-soft-robots/tennicam_client/blob/master/config/config.toml
     */
    Driver(std::string toml_config_file);
  /** @brief Instantiate a driver in "active transform mode", i.e. at each iteration
     * the driver will read the corresponding shared memory segment for new
     * transformation parameter, allowing for runtime tuning of the transform,
     * as described here:
     * https://intelligent-soft-robots.github.io/pam_documentation/C5_visual_ball_tracking.html#how-to-fix-the-transform-of-the-ball
     * Note that this slows the driver down, so it is recommanded to call this
     * constructor only if the transform requires to be tuned.
     */
    Driver(std::string toml_config_file,
           std::string active_transform_segment_id);
    /**
     * @brief create the zmq socket required to connect with tennicam
     */
    void start();
    void stop();
    /**
     * @brief Dummy function required by the o80::Driver interface
     */
    void set(const DriverIn&);
    /**
     * @brief read a ball information from tennicam, apply the transform,
     * compute the ball velocity via finite differences and returns it   */
    Ball get();
    const DriverConfig& get_config() const;
    /**
     * @brief Activate the "active transform mode"
     */
    void set_active_config_read(std::string segment_id);

private:
    // compute the velocity using finite differences
    std::array<double, 3> compute_velocity(
        long int time_stamp, const std::array<double, 3>& position);

    void init_active_transform_read() const;

private:
    DriverConfig config_;
    Transform transform_;
    std::unique_ptr<zmq::context_t> context_;
    std::unique_ptr<zmq::socket_t> socket_;
    zmq::message_t reply_;
    json_helper::Jsonhelper jh_;
    long int ball_id_;
    long int previous_time_stamp_;
    std::array<double, 3> previous_position_;
    std::array<double, 3> previous_velocity_;
    bool active_transform_read_;
    std::string active_transform_segment_id_;
};

}  // namespace tennicam_client
