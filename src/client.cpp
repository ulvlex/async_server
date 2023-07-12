#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
    system("chcp 625001");
    try
    {

        if (argc != 3) //в начале задаются аргументы командной строки: хост и порт, с учётом нулевого аргумента - путь файла, получается 3 аргумента, если не равно трём, то значит не заданы какие-то параметры, или задано слишком много
        {
            std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
            return 1;
        }

        size_t numOfThreads, numOfCycles;
        std::cout << "Please enter the number of threads: ";
        std::cin >> numOfThreads;
        std::cout << "Please enter the number of cycles: ";
        std::cin >> numOfCycles;

        //cin игнорирует символы пробела и оставляет их в потоке как мусор, поэтому нужно проигнорировать это
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        char request[max_length] = "";
        char reply[max_length] = "";

        for (int i = 0; i < numOfThreads; ++i) { //запросы каждого клиента

            // одно подключение для одного клиента
            boost::asio::io_context io_context;

            tcp::socket s(io_context);
            tcp::resolver resolver(io_context); //объект resolver, который исользуется для разрешения имени хоста и порта
            boost::asio::connect(s, resolver.resolve(argv[1], argv[2])); //устаналивает соединение с удалённым сервером, используя разрешённые значения хоста и порта

            for (int j = 0; j < numOfCycles; ++j) { //запросы одного единственного клиента в данный момент сессии
                std::cout << "Enter message: ";

                std::cin.getline(request, max_length); //ввод сообщения от пользователя
                size_t request_length = std::strlen(request);
                boost::asio::write(s, boost::asio::buffer(request, request_length)); //запись сообщения в буфер для передачи данных 


                size_t reply_length = boost::asio::read(s,
                    boost::asio::buffer(reply, request_length)); //ответ от сервера, который считывается из сокета 
                std::cout << "Reply is: ";
                std::cout.write(reply, reply_length);
                std::cout << "\n";
            }

        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}