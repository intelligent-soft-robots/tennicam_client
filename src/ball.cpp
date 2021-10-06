#include "tennicam_ball/ball.hpp"

namespace tennicam_ball
{
  
  Ball::Ball()
    : ball_id_{-1}
  {}

  Ball::Ball(long int ball_id,
	     const std::array<double,3>& position,
	     const std::array<double,3>& velocity)
    : ball_id_{ball_id},
      position_{position},
      velocity_{velocity} {}

  void Ball::set_position(double x, double y, double z)
  {
    position_[0]=x;
    position_[1]=y;
    position_[2]=z;
  }

  void Ball::set_velocity(double dx, double dy, double dz)
  {
    velocity_[0]=x;
    velocity_[1]=y;
    velocity_[2]=z;
  }

  void Ball::set(const std::array<double,3>& position,
		 const std::array<double,3>& velocity)
  {
    position_ = position;
    velocity_ = velocity;
  }
  
  const std::array<double,3>& get_position() const
  {
    return position_;
  }

  const std::array<double,3>& get_velocity() const
  {
    return velocity_;
  }

}
