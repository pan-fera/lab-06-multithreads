//
// Created by hacker on 04.02.2021.
//
#include <iostream>
#include <vector>
#include "picosha2.h"
#include <cstdlib>
#include <ctime>
#include <thread>
#include <vector>
#include <cstdio>
#include <csignal>

#include <string>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/trivial.hpp>
#include <algorithm>
#include <chrono>
#include <nlohmann/json.hpp>
#include <mutex>

#ifndef TEMPLATE_MULTITHREADSSHA_HPP
#define TEMPLATE_MULTITHREADSSHA_HPP

class MultithreadsSHA {
 public:
  MultithreadsSHA();
  MultithreadsSHA(unsigned int count);
  MultithreadsSHA(unsigned int count, std::string filename);
  [[noreturn]] static void find_hash();
  static void init();
  void start();
  static void ex([[maybe_unused]] int sig_num);
  static void write_json();
 private:
  unsigned int _count;
};

#endif  // TEMPLATE_MULTITHREADSSHA_HPP
