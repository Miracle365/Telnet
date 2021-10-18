#include "server.hpp"
int main(int argc, char* argv[]) {

    unsigned short port;
    size_t buffsize;
    size_t Num_thread;

    if (argc != 4)
    {
        std::cerr << "Incorrect number of arguments\n";
        return 1;
    }

    try
    {
        port = std::stoi(argv[1]);
    }
    catch (std::invalid_argument q)
    {
        std::cerr << "Invalid port\n";
        return 1;
    }

    try
    {
        buffsize = std::stoi(argv[2]);
    }
    catch (std::invalid_argument q)
    {
        std::cerr << "Invalid size of buffer\n";
        return 1;
    }

    try
    {
        Num_thread = std::stoi(argv[3]);
    }
    catch (std::invalid_argument q)
    {
        std::cerr << "Invalid thread number\n";
        return 1;
    }

    try
    {
        boost::asio::io_context io_context;
        boost::asio::thread_pool pool(Num_thread);
        server srv(io_context, port);
        srv.async_accept();

        for (int i = 0; i < Num_thread; i++)
        {
            boost::asio::post(pool, [&io_context]()
                {
                    io_context.run();
                });
        }


        pool.join();
    }
        //io_context.run();

    catch (std::exception& q)
    {
        std::cerr << "Exception: " << q.what() << "\n";
    }

	return 0;
}