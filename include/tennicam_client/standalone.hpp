#pragma once

#include "o80/memory_clearing.hpp"
#include "o80/standalone.hpp"
#include "tennicam_client/ball.hpp"
#include "tennicam_client/driver.hpp"

#define TENNICAM_CLIENT_QUEUE_SIZE 50000

namespace tennicam_client
{
/**
 * @brief o80 standalone over the Driver, i.e. will
 * an instance of Standalone will instantiate an instance of
 * o80 backend that will subscribe to tennicam and write
 * corresponding ball information in the shared memory.
 */
class Standalone
    : public o80::Standalone<TENNICAM_CLIENT_QUEUE_SIZE,  // Queue size
                             1,                           // nb dofs
                             Driver,
                             Ball,                    // o80 observation
                             o80::VoidExtendedState>  // no info on top of obs
{
public:
    Standalone(std::shared_ptr<Driver> driver_ptr,
               double frequency,
               std::string segment_id);
    o80::States<1, Ball> convert(const Ball& ball);
    DriverIn convert(const o80::States<1, Ball>&);
};

}  // namespace tennicam_client
