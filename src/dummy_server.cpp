#include "tennicam_client/dummy_server.hpp"

namespace tennicam_client
{
static THREAD_FUNCTION_RETURN_TYPE run_helper(void* arg)
{
    ((DummyServer*)arg)->run();
    return THREAD_FUNCTION_RETURN_VALUE;
}

DummyServer::DummyServer(const DriverConfig& config) : running_{false}
{
    context_ = std::make_unique<zmq::context_t>(1);
    auto socket_type = zmq::socket_type::pub;
    socket_ = std::make_unique<zmq::socket_t>(*context_, socket_type);
    socket_->bind(config.get_url());
}

DummyServer::~DummyServer()
{
    if (running_)
    {
        stop();
    }
}

void DummyServer::perform(long int num, double x, double y, double z)
{
    json obs;
    obs.push_back(x);
    obs.push_back(y);
    obs.push_back(z);
    json jframe{{"num", num},
                {"time", o80::time_now().count()},
                {"proc_time", 1},
                {"obs", obs}};

    zmq::message_t msg(jframe.dump());
    socket_->send(msg, zmq::send_flags::none);
}

void DummyServer::start()
{
    thread_.create_realtime_thread(run_helper, (void*)this);
}

void DummyServer::stop()
{
    running_ = false;
    thread_.join();
}

void DummyServer::run()
{
    running_ = true;
    long int num = 0;
    double x = 0;
    double y = 0;
    double z = 0;
    while (running_)
    {
        x += 0.001;
        y += 0.001;
        z -= 0.0005;
        perform(num++, cos(x), sin(y), cos(z));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

}  // namespace tennicam_client
