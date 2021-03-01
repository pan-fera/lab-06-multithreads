//
// Created by hacker on 04.02.2021.
// Copyright [2021] <pan-fera>
#include <iostream>
#include "PicoSHA2/picosha2.h"
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

#include <algorithm>
#include <chrono>
#include <nlohmann/json.hpp>
#include <mutex>

#ifndef INCLUDE_MULTITHREADSSHA_HPP_
#define INCLUDE_MULTITHREADSSHA_HPP_

class MultithreadsSHA {
public:
    MultithreadsSHA();
    explicit MultithreadsSHA(unsigned int count);
    MultithreadsSHA(unsigned int count, std::string filename);
    [[noreturn]] static void find_hash();
    static void init();
    void start();
    inline static void ex([[maybe_unused]] int sig_num);
    inline static void write_json();
    inline static std::string _filename;
private:
    unsigned int _count;
};

#endif  // INCLUDE_MULTITHREADSSHA_HPP_
