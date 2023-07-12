#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session //сессия обмена данными с клиентом
    : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, size_t connectedClient) //конструктор принимает сокет (IP, Port)
        : socket_(std::move(socket)), connectedClient_(connectedClient)
    {
        std::cout << "connection established: " << connectedClient_ << std::endl;
    }

    ~session() {
        std::cout << "connection lost " << connectedClient_ << std::endl;
    }


    void start() //метод запускает асинхронное чтение
    {
        do_read(); //асинхронное чтение
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length), //читаем в буфер, захватываем в лямбду обработчика завершающий указатель на разделяемую копию себя 
            [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    do_write(length); //асинхронная запись данных 
                }
            });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length), //асинхронная запись данныз в сокет
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
                if (!ec) //после успешной записи снова идёт чтение из сокета
                {
                    do_read();
                }
            });
    }

    tcp::socket socket_;
    size_t connectedClient_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), //объект ассептор, который используется для прослушивания подключений на указанном порту
        connectedClient(1)
    {
        do_accept(); //асинхронное принимает входящее подключение на указ. порту
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket)
            {
                if (!ec) //если подключение принято успешно, создаётся новый объект сессии и начинается обмен данными с клиентом 
                {
                    std::make_shared<session>(std::move(socket), connectedClient)->start();
                    ++connectedClient;
                }

                do_accept(); //вызывается снова, чтобы ожидать следующего подключения
            });
    }

    size_t connectedClient;
    tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2) //аргумент командной строки - порт
        {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        server s(io_context, std::atoi(argv[1])); //передаётся порт

        io_context.run(); //метод run объекта io_context запускает цикл обработки событий, который продолжается, пока есть активные асинхронные операции
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}