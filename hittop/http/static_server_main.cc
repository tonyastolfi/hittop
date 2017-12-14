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
    std::cout << config.DebugString() << std::endl;
  }

  boost::asio::io_service io;

  // TODO
  auto handler_factory = []() {
    return [](auto &request, auto &&continue_with, auto &&respond_with) {
      std::cout << "method name is " << request.method_name() << std::endl;
      continue_with(hittop::io::error_code{}, std::string{"foobar"});

      namespace http = hittop::http;

      respond_with(http::status = 200, http::message = "Ok",
                   http::headers =
                       [](auto &&write_header) {
                         write_header("Content-Type", "text/plain");
                       },
                   http::body = "Hello, World!");
    };
  };

  hittop::http::HttpServer<decltype(handler_factory)> server(
      io, config, std::forward_as_tuple(handler_factory),
      [](auto...) { std::cout << "server started" << std::endl; });

  server.AsyncRun([](auto...) { std::cout << "server stopped" << std::endl; });

  io.run();

  return 0;
}
