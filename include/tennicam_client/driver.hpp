#pragma once

#include "o80/driver.hpp"
#include <stdlib.h>
#include <cmath>
#include <chrono>
#include <zmq.hpp>

namespace tennicam_ball
{

  class Driver : public o80::Driver<void, Ball>
  {
  public:
    Driver(std::array<double,3> translation,
	   std::array<double,3> rotation,
	   std::string server_hostname, int server_port)
      : transform_{translation,rotation},
	server_hostname_{server_hostname},
	server_port_{server_port}
    {}

    void start()
    {
      context_ = new zmq::context_t(1);
      socket_  = new zmq::socket_t(*(context_),ZMQ_SUB);
      std::ostringstream s;
      s << "tcp://" << server_hostname_ << ":" << server_port_;
      socket_->connect(s.str());
      socket_->setsockopt(ZMQ_SUBSCRIBE, "",0 );
    }
      
    void stop()
    {
      if(socket_)
	delete socket_;
      if(context_)
	delete context_;
    }
      
    void set(void){}


    // compute the velocity using finite differences
    std::array<double,3> compute_velocity(long int time_stamp, const std::array<double,3>& position)
    {
      std::array<double,3> v;
      
      // can not perform finite differences
      // if no previous iteration
      if (previous_time_stamp_<0)
	{
	  previous_time_stamp_ = time_stamp;
	  previous_position_ = position;
	  v.fill(0);
	  return v;
	}

      double time_diff = static_cast<double>(time_stamp-previous_time_stamp_)*1e-9;
      for(int i=0;i<3;i++)
	{
	  v[i]=(position[i]-previous_position_[i]) / time_diff;
	}

      previous_time_stamp_ = time_stamp;
      previous_position_ = position;

      return v;
      
    }
    
    Ball get()
    {

      // receiving the ball information from zmq.
      // zmq serialize the information into a json formatted string
      socket_->recv(&(reply_),ZMQ_NOBLOCK);
      std::string rpl = std::string(static_cast<char*>(reply_.data()), reply_.size());
      jh_.j=json::parse(rpl);

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

      // parsing the json string
      std::array<double,3> position {
	static_cast<double>(jh_.j["obs"][0]),
	  static_cast<double>(jh_.j["obs"][1]),
	  static_cast<double>(jh_.j["obs"][2])
	  }

      // updating the frame 
      position = transform_.apply(position);

      // computing velocity (finite difference)
      // (note: this updates also previous_time_stamp_
      // and previous_position_)
      std::array<double,3> velocity = compute_velocity(static_cast<long int>(jh_.j["time"]),
						       position);
      
      
      return Ball(position,velocity);

    }
    
  private:

    Transform transformation_;
    std::string server_hostname_;
    int server_port_;
    zmq::socket_t* socket_;
    zmq::context_t* context_;
    zmq::message_t reply_;
    json_helper::Jsonhelper jh_;

    long int previous_time_stamp_;
    std::array<double,3> previous_position_;
    std::array<double,3> previous_velocity_;
  };
  
}  
