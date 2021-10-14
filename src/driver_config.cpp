#include "tennicam_client/driver_config.hpp"

namespace tennicam_client
{

  DriverConfig::DriverConfig()
    : server_hostname{"undefined"}{}
  
  DriverConfig::DriverConfig(std::string _server_hostname,
			     int _server_port,
			     std::array<double,3> _translation,
			     std::array<double,3> _rotation)
    : server_hostname(_server_hostname),
      server_port(_server_port),
      translation(_translation),
      rotation(_rotation)
  {}

  std::string DriverConfig::get_url() const
  {
    std::ostringstream s;
    s << "tcp://" << server_hostname << ":" << server_port;
    return s.str();
  }
  
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

  }

  DriverConfig parse_toml(const std::string& toml_config_file)
  {
    toml::table config_table = toml::parse_file(toml_config_file);
    std::array<double,3> translation = internal::parse_toml_transform(config_table,std::string("translation"));
    std::array<double,3> rotation = internal::parse_toml_transform(config_table,std::string("rotation"));
    std::string hostname = internal::parse_toml_server<std::string>(config_table,std::string("hostname"));
    int port = internal::parse_toml_server<int>(config_table,std::string("port"));
    return DriverConfig(hostname,port,translation,rotation);
  }

  namespace internal
  {
    static std::string str_array(const std::array<double,3>& a)
    {
      std::ostringstream s;
      s << "[" << a[0] << "," << a[1] << "," << a[2] << "]";
      return s.str();
    }
    
  }

  
  void update_transform_config_file(std::string file_path,
				    const std::array<double,3>& translation,
				    const std::array<double,3>& rotation)
  {
    DriverConfig config = parse_toml(file_path);
    std::ofstream os;
    os.open(file_path);
    os << "[transform]" << std::endl
       << "translation = "<< internal::str_array(translation)  << std::endl
       << "rotation = " << internal::str_array(rotation)  << std::endl
       << "[server]" << std::endl
       << "hostname = \""<< config.server_hostname <<"\"" << std::endl
       << "port = "<< config.server_port << std::endl;
    os.close();
  }

  
}
