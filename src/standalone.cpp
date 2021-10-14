#include "tennicam_client/standalone.hpp"

namespace tennicam_client
{
Standalone::Standalone(std::shared_ptr<Driver> driver_ptr,
                       double frequency,
                       std::string segment_id)
    : o80::Standalone<TENNICAM_CLIENT_QUEUE_SIZE,
                      1,
                      Driver,
                      Ball,
                      o80::VoidExtendedState>(driver_ptr, frequency, segment_id)
{
}

o80::States<1, Ball> Standalone::convert(const Ball& ball)
{
    o80::States<1, Ball> balls;
    balls.set(0, ball);
    return balls;
}

DriverIn Standalone::convert(const o80::States<1, Ball>&)
{
    return DriverIn();
}
}  // namespace tennicam_client
