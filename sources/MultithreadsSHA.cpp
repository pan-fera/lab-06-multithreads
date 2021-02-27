//
// Created by hacker on 04.02.2021.
//

#include "MultithreadsSHA.hpp"

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

std::atomic<bool> state = true;
//std::mutex mutex;

std::vector<nlohmann::json> values;
std::string _filename;

MultithreadsSHA::MultithreadsSHA():_count(std::thread::hardware_concurrency()){
  _filename = "";
}

MultithreadsSHA::MultithreadsSHA(unsigned int count){
  _count = (count <= std::thread::hardware_concurrency())? count :
           std::thread::hardware_concurrency();
  _filename = "";
}

MultithreadsSHA::MultithreadsSHA(unsigned int count, std::string filename){
  _count = (count <= std::thread::hardware_concurrency())? count :
           std::thread::hardware_concurrency();
  _filename = filename;
}

void MultithreadsSHA::find_hash() {
  std::string hash;
  int number;
  std::string hex_number;
  nlohmann::json _j;

  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  src::severity_logger<logging::trivial::severity_level> slg;

  while(state) {
    int milli_seconds = 0;
    auto start = std::chrono::high_resolution_clock::now();

    do {
      number = std::rand();
      std::stringstream stream;

      stream << std::hex << number;
      hex_number = stream.str();

      const std::vector<unsigned char> data_vector(hex_number.begin(),
                                                   hex_number.end());

      hash = picosha2::hash256_hex_string(data_vector);
      //if (state) mutex.lock();
      BOOST_LOG_TRIVIAL(trace) << " data: " << hex_number
                               << " hash: " << hash << " id: " <<  std::this_thread::get_id();
    } while (hash.substr(60) != "0000");
    auto end = std::chrono::high_resolution_clock::now();
    milli_seconds +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
            .count();
    BOOST_LOG_TRIVIAL(info)<< " data: " << hex_number
                           << " hash: " << hash << " id: "
                           <<  std::this_thread::get_id();
    nlohmann::json j = {
        {"timestamp (ms)", milli_seconds}, {"hash", hash}, {"data", hex_number}};
    values.push_back(j);
  }

}

void MultithreadsSHA::start() {
  std::vector<std::thread> th;
  signal(SIGTERM, ex);
  signal(SIGKILL, ex);
  signal(SIGABRT, ex);
  signal(SIGINT, ex);
  init();

  for (unsigned int i = 0; i < _count; ++i) {
    std::thread _thread(find_hash);
    th.push_back(std::move(_thread));
  }

  for (std::thread& _th : th) {
    _th.join();
  }

}

void MultithreadsSHA::init() {
  logging::add_console_log(std::cout,
                           keywords::format = "[%TimeStamp%] [%Severity%] %Message%",
                           keywords::auto_flush = true,
                           keywords::filter = logging::trivial::severity == logging::trivial::info);

  typedef sinks::synchronous_sink<sinks::text_file_backend> file_sink;
  boost::shared_ptr<file_sink> sink(new file_sink(
      keywords::file_name = "./logs/file_%5N.log",
      keywords::rotation_size = 5 * 1024 * 1024,
      keywords::time_based_rotation =
          sinks::file::rotation_at_time_point(12, 0, 0),
      keywords::auto_flush = true));

  sink->set_formatter(expr::stream
                          << "[" << expr::attr<boost::posix_time::ptime>("TimeStamp")
                          << "] [" << logging::trivial::severity << "] "
                          << expr::smessage);

  logging::core::get()->add_sink(sink);
  logging::add_common_attributes();
  /*
  const std::string format = "[%TimeStamp%] [%Severity%] %Message%";

  auto sink_to_file = logging::add_file_log(
      boost::log::keywords::file_name = "logs/log_trace_%N.log",
      boost::log::keywords::rotation_size = 10*1024*1024,
      boost::log::keywords::format = format);
  sink_to_file->set_filter(
      boost::log::trivial::severity >= boost::log::trivial::trace);

  auto sink_to_console = boost::log::add_console_log(
      std::cout,
      boost::log::keywords::format = format);
  sink_to_console->set_filter(
      boost::log::trivial::severity >= boost::log::trivial::info);

  boost::log::add_common_attributes();*/
}

void MultithreadsSHA::ex([[maybe_unused]] int sig_num) {
  // mutex.lock();
  state = false;
  //logging::core::get()->flush();
  // logging::core::get()->remove_all_sinks();

  write_json();
  exit(sig_num);
}

void MultithreadsSHA::write_json() {
  std::ofstream file;
  file.open(_filename + ".json");
  for (unsigned i = 0; i < values.size(); ++i){
    file << values[i].dump(5) << "\n";
  }
  file.close();
}
