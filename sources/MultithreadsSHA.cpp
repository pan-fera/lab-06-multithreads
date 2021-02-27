//
// Created by hacker on 04.02.2021.
// Copyright [2021] <pan-fera>

#include "MultithreadsSHA.hpp"

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

std::mutex mutex;
auto result = nlohmann::json::array();


MultithreadsSHA::MultithreadsSHA():_count(std::thread::hardware_concurrency()){
    _filename =  "";
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

[[noreturn]] void MultithreadsSHA::find_hash() {
    std::string hash;
    int number;
    std::string hex_number;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    while (true) {
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

            BOOST_LOG_TRIVIAL(trace) << " data: " << hex_number
                                     << " hash: " << hash << " id: "
                                     <<  std::this_thread::get_id();
        } while (hash.substr(60) != "0000");
        auto end = std::chrono::high_resolution_clock::now();
        milli_seconds +=
                std::chrono::duration_cast<std::chrono::milliseconds>
                (end - start).count();
        BOOST_LOG_TRIVIAL(info)<< " data: " << hex_number
                               << " hash: " << hash << " id: "
                               <<  std::this_thread::get_id();
        auto make_object = [&]
        {
            auto js = nlohmann::json::object();
            js["timestamp(ms)"] = milli_seconds;
            js["hash"] = hash;
            js["data"] = hex_number;
            return js;
        };
        result.push_back(make_object());
    }
}

void MultithreadsSHA::start() {
    std::vector<std::thread> th;
    signal(SIGTERM, ex);
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
                             keywords::filter = logging::trivial::severity
                                     == logging::trivial::info);

    typedef sinks::synchronous_sink<sinks::text_file_backend> file_sink;
    boost::shared_ptr<file_sink> sink(new file_sink(
            keywords::file_name = "./logs/file_%5N.log",
            keywords::rotation_size = 5 * 1024 * 1024,
            keywords::auto_flush = true));

    sink->set_formatter(expr::stream
                                << "["
                                << expr::attr<boost::posix_time::ptime>("TimeStamp")
                                << "] [" << logging::trivial::severity << "] "
                                << expr::smessage);

    logging::core::get()->add_sink(sink);
    logging::add_common_attributes();
}

void MultithreadsSHA::ex([[maybe_unused]] int sig_num) {
    mutex.lock();
    write_json();
    exit(sig_num);
}

void MultithreadsSHA::write_json() {
    if (!_filename.empty()) {
        std::ofstream file;
        file.open(_filename + ".json");
        file << result.dump(4);
        file.close();
    }
}
