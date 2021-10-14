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
class DummyServer
{
public:
    DummyServer(const DriverConfig& config);
    ~DummyServer();
    void start();
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
