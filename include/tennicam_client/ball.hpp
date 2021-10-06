#pragma one

#include <array>
#include "shared_memory/serializer.hpp"
#include "shared_memory/shared_memory.hpp"

namespace tennicam_ball
{

  class Ball
  {
  public:

    Ball();
    Ball(const std::array<double,3>& position,
	 const std::array<double,3>& velocity)
    void set_position(double x, double y, double z);
    void set_velocity(double dx, double dy, double dz);
    void set(const std::array<double,3>& position,
	     const std::array<double,3>& velocity);
    const std::array<double,3>& get_position() const;
    const std::array<double,3>& get_velocity() const;

  private:
    std::array<double,3> compute_velocity(long int time_stamp,
					  const std::array<double,3>& position);
    
  public:

    template <class Archive>
    void serialize(Archive& archive)
    {
      archive(position_,velocity_,ball_id_);
    }

  private:

    friend shared_memory::private_serialization;
    
    std::array<double,3> position_;
    std::array<double,3> velocity_;

    long int ball_id_;

    long int time_stamp_ns;

  };

  

}
