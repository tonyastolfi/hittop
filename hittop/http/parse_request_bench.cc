#include <chrono>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

#include "boost/lexical_cast.hpp"
#include "boost/range/iterator_range.hpp"

#include "hittop/http/parse_request.h"
#include "hittop/http/request.h"

int main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " REQUEST_FILE TIMES_TO_PARSE"
              << std::endl;
    return 1;
  }

  const char *const filename = argv[1];
  unsigned count = boost::lexical_cast<unsigned>(argv[2]);
  std::ostringstream contents;
  {
    std::ifstream ifs(filename);
    contents << ifs.rdbuf();
  }
  const std::size_t request_size = contents.str().length();
  std::unique_ptr<char[]> buffer(new char[request_size * count]);
  for (int i = 0; i < count; ++i) {
    std::memcpy(&buffer[i * request_size], contents.str().c_str(),
                request_size);
  }
  char *next = &buffer[0];
  for (int j = 0; j < 10; ++j) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < count; ++i) {
      hittop::http::ZeroCopyRequest<const char *> request;
      auto result = hittop::http::ParseRequest(
          boost::make_iterator_range(next, next + request_size), &request);
      if (!result.ok()) {
        std::cerr << "Fail!" << std::endl;
      }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    double usec =
        std::chrono::duration_cast<std::chrono::microseconds>(stop - start)
            .count();
    std::cout << "Ok "
              << "total: " << usec << "usec "
              << "rps: " << static_cast<double>(count) * 1000.0 * 1000.0 / usec
              << " "
              << "usec/r: " << usec / static_cast<double>(count) << std::endl;
  }
  return 0;
}
