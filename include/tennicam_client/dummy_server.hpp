#pragma once

#include <math.h>
#include <atomic>
#include <chrono>
#include <memory>
#include <zmq.hpp>
#include <zmqpp/zmqpp.hpp>
#include "json_helper/json_helper.hpp"
#include "o80/time.hpp"
#include "real_time_tools/thread.hpp"
#include "tennicam_client/driver.hpp"

namespace tennicam_client
{
/**
 * @brief zmq publisher created for the purpose of testing Driver.
 * Similarly to tennicam, an instance of DummyServer publishes
 * ball information.
 */
class DummyServer
{
public:
    /**
     * @brief Instantiate a DummyDriver using the hostname
     * and port attributes of the configuration.
     */

    DummyServer(const DriverConfig& config);
    ~DummyServer();
    /**
     * @brief spawns a thread that publishes balls
     */
    void start();
    /**
     * @brief stops the thread
     */
    void stop();
    void run();

private:
    void perform(long int num, double x, double y, double z);

private:
    std::unique_ptr<zmqpp::context> context_;
    std::unique_ptr<zmqpp::socket> socket_;
    std::atomic<bool> running_;
    real_time_tools::RealTimeThread thread_;
};

}  // namespace tennicam_client
