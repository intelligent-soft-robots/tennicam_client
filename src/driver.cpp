#include "tennicam_client/driver.hpp"

namespace tennicam_client
{


  DriverConfig::DriverConfig(std::string _server_hostname,
			     int _server_port,
			     std::array<double,3> _translation,
			     std::array<double,3> _rotation)
    : server_hostname(_server_hostname),
      server_port(_server_port),
      translation(_translation),
      rotation(_rotation)
  {}

  
  namespace internal
  {
    static std::array<double,3> parse_toml_transform(const toml::table& config_table, const std::string& field)
    {
      std::array<double,3> a;
      toml::array translation_toml = *config_table["transform"][field].as_array();
      for(std::size_t index=0 ; index<3; index++)
	{
	  a[index]=*(translation_toml[index].value<double>());
	}
      return a;
    }

    template<class T>
    static T parse_toml_server(const toml::table& config_table, const std::string& field)
    {
      std::optional<T> opt_v = config_table["server"][field].value<T>();
      if(!opt_v.has_value())
	{
	  throw std::invalid_argument(std::string("failed to find node server/") +
				      field);
	}
      return opt_v.value();
    }

    DriverConfig parse_toml(const std::string& toml_config_file)
    {
      toml::table config_table = toml::parse_file(toml_config_file);
      std::array<double,3> translation = parse_toml_transform(config_table,std::string("translation"));
      std::array<double,3> rotation = parse_toml_transform(config_table,std::string("rotation"));
      std::string hostname = parse_toml_server<std::string>(config_table,std::string("hostname"));
      int port = parse_toml_server<int>(config_table,std::string("port"));
      return DriverConfig(hostname,port,translation,rotation);
    }
  }
  

  
  
  Driver::Driver(std::string toml_config_file)
    : config_{internal::parse_toml(toml_config_file)},
      transform_{config_.translation,config_.rotation},
      context_{nullptr},
      socket_{nullptr},
      ball_id_{-1},
      previous_time_stamp_{-1}
  {}

  Driver::Driver(const DriverConfig& config)
    : config_(config),
      transform_(config.translation,
		 config.rotation),
      context_{nullptr},
      socket_{nullptr},
      ball_id_{-1},
      previous_time_stamp_{-1}
  {}
  
  Driver::Driver(std::array<double,3> translation,
		 std::array<double,3> rotation,
	   std::string server_hostname, int server_port)
    : config_{server_hostname,
	      server_port,
	      translation,
	      rotation},
      transform_{translation,rotation},
      context_{nullptr},
      socket_{nullptr},
      ball_id_{-1},
      previous_time_stamp_{-1}
  {}

  void Driver::start()
    {
      context_ = std::make_unique<zmq::context_t>(1);
      socket_  = std::make_unique<zmq::socket_t>(*(context_),ZMQ_SUB);
      std::ostringstream s;
      s << "tcp://" << config_.server_hostname << ":" << config_.server_port;
      socket_->connect(s.str());
      socket_->setsockopt(ZMQ_SUBSCRIBE, "",0 );

    }
      
  void Driver::stop()
  {}
  
  void Driver::set(const DriverIn&){}


  std::array<double,3> Driver::compute_velocity(long int time_stamp, const std::array<double,3>& position)
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
    
  Ball Driver::get()
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
	  };
      
      // updating the frame 
      position = transform_.apply(position);

      // computing velocity (finite difference)
      // (note: this updates also previous_time_stamp_
      // and previous_position_)
      std::array<double,3> velocity = compute_velocity(static_cast<long int>(jh_.j["time"]),
						       position);
      
      
      return Ball(ball_id_++,position,velocity,static_cast<long int>(jh_.j["time"]));

    }

  const DriverConfig& Driver::get_config() const
  {
    return config_;
  }
  
}
