#include <fcntl.h>

#include <iostream>

#include "boost/asio/io_service.hpp"

#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/text_format.h"

#include "hittop/http/server.h"

int main(int argc, const char *const *argv) {
  hittop::http::HttpServerConfig config;
  {
    const char *const config_path = argv[1];
    const int fd = open(config_path, O_RDONLY);
    if (-1 == fd) {
      std::cerr << "failed to open config file: " << config_path << std::endl;
      return 1;
    }
    google::protobuf::io::FileInputStream fin(fd);
    fin.SetCloseOnDelete(true);
    google::protobuf::TextFormat::Parse(&fin, &config);
  }

  boost::asio::io_service io;

  // TODO
  auto handler_factory = []() { return nullptr; };

  hittop::http::HttpServer<decltype(handler_factory)> server(
      io, config, handler_factory,
      [](auto...) { std::cout << "server started" << std::endl; });

  server.AsyncRun([](auto...) { std::cout << "server stopped" << std::endl; });

  io.run();

  return 0;
}
