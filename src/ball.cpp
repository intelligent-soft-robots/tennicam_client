#include "tennicam_client/ball.hpp"

namespace tennicam_client
{
  
  Ball::Ball()
    : ball_id_{-1}
  {}

  Ball::Ball(long int ball_id,
	     const std::array<double,3>& position,
	     const std::array<double,3>& velocity,
	     long int time_stamp_ns)
    : ball_id_{ball_id},
      position_{position},
      velocity_{velocity},
      time_stamp_ns_{time_stamp_ns}
  {}
      
  void Ball::set_position(double x, double y, double z)
  {
    position_[0]=x;
    position_[1]=y;
    position_[2]=z;
  }

  void Ball::set_velocity(double dx, double dy, double dz)
  {
    velocity_[0]=dx;
    velocity_[1]=dy;
    velocity_[2]=dz;
  }

  void Ball::set(const std::array<double,3>& position,
		 const std::array<double,3>& velocity)
  {
    position_ = position;
    velocity_ = velocity;
  }

  
  
  const std::array<double,3>& Ball::get_position() const
  {
    return position_;
  }

  const std::array<double,3>& Ball::get_velocity() const
  {
    return velocity_;
  }

  std::tuple<std::array<double,3>,std::array<double,3>> Ball::get() const
  {
    return std::make_tuple(position_,velocity_);
  }
  
  long int Ball::get_time_stamp() const
  {
    return time_stamp_ns_;
  }

  long int Ball::get_ball_id() const
  {
    return ball_id_;
  }

  std::string Ball::to_string() const
  {
    std::stringstream s;
    s << "Ball " << ball_id_ << "(" << time_stamp_ns_ << ") ";
    s << "position: ";
    for(const double& p : position_)
      {
	s << std::setprecision(3) << p << " ";
      }
    s << "velocity: ";
    for(const double& v : velocity_)
      {
	s << std::setprecision(3) << v << " "; 
      }
    s << std::endl;
    return s.str();
  }
  
}
