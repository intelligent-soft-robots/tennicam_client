#pragma once

#include <array>
#include <iomanip>
#include <sstream>
#include "o80/sensor_state.hpp"
#include "shared_memory/serializer.hpp"
#include "shared_memory/shared_memory.hpp"

namespace tennicam_client
{
class Ball : public o80::SensorState
{
public:
    Ball();
    Ball(long int ball_id,
         const std::array<double, 3>& position,
         const std::array<double, 3>& velocity,
         long int time_stamp_ns);
    void set_position(double x, double y, double z);
    void set_velocity(double dx, double dy, double dz);
    void set(const std::array<double, 3>& position,
             const std::array<double, 3>& velocity);
    const std::array<double, 3>& get_position() const;
    const std::array<double, 3>& get_velocity() const;
    std::tuple<std::array<double, 3>, std::array<double, 3>> get() const;
    long int get_ball_id() const;
    long int get_time_stamp() const;
    std::string to_string() const;

public:
    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(position_, velocity_, ball_id_, time_stamp_ns_);
    }

private:
    friend shared_memory::private_serialization;

    long int ball_id_;
    std::array<double, 3> position_;
    std::array<double, 3> velocity_;
    long int time_stamp_ns_;
};

}  // namespace tennicam_client
