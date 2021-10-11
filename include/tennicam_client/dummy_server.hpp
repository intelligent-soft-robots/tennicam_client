

class DummyServer
{
public:
  DummyServer(const DriverConfig& config)
  {
    context_ = std::make_unique<zmq::context_t>();
    auto socket_type = zmqpp::socket_type::pub;
    socket_  = std::make_unique<zmq::socket_t>(*(context_),socket_type);
    socket_->bind(config.server_hostname);
  }

  void perform()
  {
    json obs;
    if (obs2d.size() != 0) {
      obs.push_back(obs2d[0].pt.x);
      obs.push_back(obs2d[0].pt.y);
    }
    json jframe {
      {"cam_id", frame.cam_id},
	{"num", frame.num},
	  {"time", frame.time.time_since_epoch().count()},
	    {"obs", obs}
    };
    BOOST_LOG_TRIVIAL(debug) << "Sending message: " << jframe.dump();
    zmqpp::message msg;
    msg << jframe.dump();
    this->send(msg);
  }
  
  
private:
    std::unique_ptr<zmq::context_t> context_;
    std::unique_ptr<zmq::socket_t> socket_;

  
  
        const json& srv_conf = conf.at("servers");
        context = unique_ptr<zmqpp::context>(new zmqpp::context);
        auto socket_type = zmqpp::socket_type::pub;
        position_pub = unique_ptr<zmqpp::socket>(new zmqpp::socket(*context, socket_type));
        const string& pp_url = srv_conf.at("position_publisher");
  
};
