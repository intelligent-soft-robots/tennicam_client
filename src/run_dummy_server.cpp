#include <filesystem>
#include "tennicam_client/dummy_server.hpp"
#include <signal_handler/signal_handler.hpp>

void execute(){

  // writting tmp config file
  std::filesystem::path tmp_file = std::filesystem::temp_directory_path();
  tmp_file /= "tennicam_client_tests_tmp";
  std::ofstream os;
  os.open(tmp_file.c_str());
  os << "[transform]" << std::endl
     << "translation = [0,1,2]" << std::endl
     << "rotation = [0.0,0.1,0.2]" << std::endl
     << "[server]" << std::endl
     << "hostname = \"*\"" << std::endl
     << "port = 7660" << std::endl;
  os.close();

  std::cout << "\n\nTennicam Client Dummy Server running\n"
	    << "using configuration file " << tmp_file.string()
	    << std::endl << std::endl;

  
  tennicam_client::DriverConfig config = tennicam_client::parse_toml(tmp_file.string());

  tennicam_client::DummyServer server{config};
  server.start();

  signal_handler::SignalHandler::initialize();
  std::cout << "Press Ctrl+C to exit" << std::endl << std::endl;
  while (true)
    {
      if (signal_handler::SignalHandler::has_received_sigint())
        {
	  server.stop();
	  break;
        }
    }

}


int main()
{
  execute();
}
