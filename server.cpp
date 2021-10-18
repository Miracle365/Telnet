#include "server.hpp"
#include <string>
#include <iostream>
#include <boost/process.hpp>
#include <boost/asio/spawn.hpp>

namespace bp = boost::process;


bool isEqualTo(std::string it, std::string that) {
    std::transform(it.begin(), it.end(), it.begin(), tolower);
    return (it.compare(that) == 0);
}

void session::start() {
    bp::environment e = boost::this_process::environment();
    bp::child c(io_context, e.at("SHELL").to_string());
    ba::spawn(io_context, [this, self = shared_from_this()](auto yc){
        for (;;) {
            std::string s;
            boost::system::error_code ec;
            bp::ipstream ipipe;
            std::string line;
            ba::async_read_until(socket, streambuf, "\n", yc[ec]);
            if (ec)
                std::cout << "error\n";
            else {
                std::istream is(&streambuf);
                std::getline(is, s);
                streambuf.consume(s.size());
                std::cout << s << '\n';
                bp::system(io_context, s, bp::std_out > ipipe);
                std::getline(ipipe, line);
            }


            ba::async_write(socket, ba::buffer(line, line.size()), yc[ec]);
            if (ec)
                std::cout << "not read\n";

        }
    });

}

void server::async_accept() {
    socket.emplace(io_context);
    acceptor.async_accept(*socket, [&](boost::system::error_code error) {
        std::make_shared<session>(std::move(*socket), io_context)->start();
        async_accept();
        });
}
