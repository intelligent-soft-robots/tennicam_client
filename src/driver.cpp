#include "tennicam_client/driver.hpp"

namespace tennicam_client
{
Driver::Driver(std::string toml_config_file,
               std::string active_transform_segment_id)
    : config_{parse_toml(toml_config_file)},
      transform_{config_.translation, config_.rotation},
      ball_id_{-1},
      previous_time_stamp_{-1},
      active_transform_read_{false},
      active_transform_segment_id_{active_transform_segment_id}
{
    if (!active_transform_segment_id_.size() == 0)
    {
        active_transform_read_ = true;
        write_transform_to_memory(active_transform_segment_id_,
                                  config_.translation,
                                  config_.rotation);
    }
}

Driver::Driver(const DriverConfig& config)
    : config_(config),
      transform_(config.translation, config.rotation),
      ball_id_{-1},
      previous_time_stamp_{-1},
      active_transform_read_{false}
{
}

Driver::Driver(std::array<double, 3> translation,
               std::array<double, 3> rotation,
               std::string server_hostname,
               int server_port)
    : config_{server_hostname, server_port, translation, rotation},
      transform_{translation, rotation},
      ball_id_{-1},
      previous_time_stamp_{-1},
      active_transform_read_{false}
{
}

void Driver::start()
{
    context_ = std::make_unique<zmq::context_t>();
    socket_ = std::make_unique<zmq::socket_t>(*context_, ZMQ_SUB);
    socket_->connect(config_.get_url());
    socket_->setsockopt(ZMQ_SUBSCRIBE, "", 0);
}

void Driver::stop()
{
}

void Driver::set(const DriverIn&)
{
}

std::array<double, 3> Driver::compute_velocity(
    long int time_stamp, const std::array<double, 3>& position)
{
    std::array<double, 3> v;

    // can not perform finite differences
    // if no previous iteration
    if (previous_time_stamp_ < 0)
    {
        previous_time_stamp_ = time_stamp;
        previous_position_ = position;
        v.fill(0);
        return v;
    }

    double time_diff =
        static_cast<double>(time_stamp - previous_time_stamp_) * 1e-9;
    for (int i = 0; i < 3; i++)
    {
        v[i] = (position[i] - previous_position_[i]) / time_diff;
    }

    previous_time_stamp_ = time_stamp;
    previous_position_ = position;

    return v;
}

Ball Driver::get()
{
    // if active_transform_read_ is true, then updating
    // the transform with values written in the shared memory
    // by the user
    if (active_transform_read_)
    {
        std::tuple<std::array<double, 3>, std::array<double, 3>> t =
            read_transform_from_memory(active_transform_segment_id_);
        transform_ = Transform(std::get<0>(t), std::get<1>(t));
    }

    // receiving the ball information from zmq.
    // zmq serialize the information into a json formatted string
    bool not_received = true;
    while (not_received)
    {
        socket_->recv(&(reply_), ZMQ_NOBLOCK);
        not_received = (reply_.size() == 0);
    }
    std::string rpl =
        std::string(static_cast<char*>(reply_.data()), reply_.size());
    jh_.j = json::parse(rpl);
    // zmq is not broadcasting any information
    if (jh_.j["obs"].is_null())
    {
        // previous observations should not be used
        // to compute the velocity
        previous_time_stamp_ = -1;
        // this construct a ball with ball_id -1,
        // i.e. invalid ball
        return Ball();
    }

    long int time_stamp = static_cast<long int>(jh_.j["time"]);

    // if the time stamp did not change (i.e. same observation),
    // simply returning the previous observation
    if (time_stamp == previous_time_stamp_)
    {
        return Ball(
            ball_id_, previous_position_, previous_velocity_, time_stamp);
    }

    // otherwise updating all
    ball_id_++;

    // parsing the json string
    std::array<double, 3> position{static_cast<double>(jh_.j["obs"][0]),
                                   static_cast<double>(jh_.j["obs"][1]),
                                   static_cast<double>(jh_.j["obs"][2])};

    // updating the frame
    position = transform_.apply(position);

    // computing velocity (finite difference)
    // (note: this updates also previous_time_stamp_
    // and previous_position_)
    previous_velocity_ = compute_velocity(time_stamp, position);

    return Ball(ball_id_, position, previous_velocity_, time_stamp);
}

const DriverConfig& Driver::get_config() const
{
    return config_;
}

void Driver::set_active_config_read(std::string segment_id)
{
    active_transform_read_ = true;
    active_transform_segment_id_ = segment_id;
}

}  // namespace tennicam_client
