#include <iostream>

#include <cstring>       // Для memset
#include <cstdlib>       // Для exit
#include <unistd.h>      // Для close


#include "Server.hpp"
// #include "../Irc.hpp"


//Close the listening socket
//While recieving - display the message, echo message
//Close socket 
int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "try again " << std::endl;
        return 1;
    }
    //Create a socket (endpoint for communication btw two devices/processes)
    int servSockListen = socket(AF_INET, SOCK_STREAM, 0); //*serv._server_socket_fd
    if (servSockListen == -1)
    {
        std::cerr << "Can't create socket!" << std::endl;
        return -1;
    }
    //Bind the socket to IP / port (endpoint for network communication. 
    //  Port is being used by sockets to send the data. 
    //  it identifies some program/process on a machine ex: Port 80 = http)
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(argv[1]);//IRC standard. *
    inet_pton(AF_INET, "0.0.0.0", &servAddr.sin_addr);
    if (bind(servSockListen, (sockaddr*)&servAddr, sizeof(servAddr)) == -1) //bind socket fd to servAddr and it's size to reserve amnt of memory for it
    {
        std::cerr << "Can't bind IP/port" << std::endl;
        close(servSockListen);
        return -2;
    }
    //Mark the socket for listening in
    if (listen(servSockListen, SOMAXCONN) == -1) //max amnt of connections in queue before the next ones are refused
    {
        std::cerr << "Can't listen" << std::endl;
        close(servSockListen);
        return -3;
    }
    std::cout << GREEN << "Server runs on port: " << argv[1] << RE << std::endl;
    //Accept a call
    sockaddr_in client;
    // socklen_t   clientSize;

    int clientSocket = accept(servSockListen, (sockaddr*)&client, sizeof(client));
    if (clientSocket == -1)
    {
        std::cerr << "Problem with client connecting!" << std::endl;
        return -4;
    }


}

    





//*socket(AF_INET, SOCK_STREAM, 0);
//  AF_INET specifies usage IPv4 protocol family
//  SOCK_STREAM it defines that the socket is TCP type
//  0 will choose default protocol (some options: IPPROTO_TCP (int 6), IPPROTO_UDP (17), IPPROTO_ICMP(1)-Internet Control Message Protocol (ICMP), typically with SOCK_RAW)


//*sockaddr_in - struct describing sock addr (in) - for internet IPv4
//  servAddr.sin_addr.s_addr = INADDR_ANY is equal to: inet_pton(servSockListen, AF_INET, "0.0.0.0", &servAddr.sin_addr);
// both in prev line says that: 
//      inet_pton: Converts the human-readable IP address "0.0.0.0" into a binary and stores it in servAddr.sin_addr
//          "0.0.0.0" is a special address that tells the socket to bind to all available network interfaces on the host machine.
//          This means the socket will accept connections from any IP addr.


//*htons() - host to network short. computer to-> network order of bytes changed
//  use this bcs of big/little endian and differences in computer and network order of bytes reading in nums

//  That fn will change it depending on whatever type of machine it is in
//  there is also ntohs() backwards fn to transform, is used later.
//? 6667 standard IRC PORT. to avoid need of root priviliges 
//0–1023 (Well-Known Ports): Reserved for system and well-known services (e.g., HTTP on port 80, SSH on port 22). 
//  You generally cannot use these without superuser privileges.
//1024–49151 (Registered Ports): These ports are registered for specific services but can also be used by applications.
//9152–65535 (Dynamic/Private Ports): These are typically used for temporary or dynamic allocations by OS.
//  Choosing a port in this range minimizes the risk of conflicts with known services.


//























// 	int server_fd, client_fd;
//     struct sockaddr_in server_addr, client_addr;
//     socklen_t client_len = sizeof(client_addr);

//     // 1. Создаем сокет
//     server_fd = socket(AF_INET, SOCK_STREAM, 0); // IPv4, потоковый (TCP)
//     if (server_fd < 0) {
//         perror("Ошибка создания сокета");
//         exit(EXIT_FAILURE);
//     }

//     // 2. Настраиваем адрес сервера
//     std::memset(&server_addr, 0, sizeof(server_addr)); // Инициализируем структуру нулями
//     server_addr.sin_family = AF_INET;                 // Используем IPv4
//     server_addr.sin_addr.s_addr = INADDR_ANY;         // Привязываем ко всем доступным интерфейсам
//     server_addr.sin_port = htons(6667);               // Указываем порт (например, 6667)

//     // 3. Привязываем сокет к адресу
//     if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
//         perror("Ошибка привязки сокета");
//         close(server_fd);
//         exit(EXIT_FAILURE);
//     }

//     // 4. Слушаем входящие соединения
//     if (listen(server_fd, 5) < 0) { // Максимум 5 ожидающих соединений
//         perror("Ошибка при переходе в режим прослушивания");
//         close(server_fd);
//         exit(EXIT_FAILURE);
//     }

//     std::cout << "Сервер запущен и слушает порт 6667..." << std::endl;

//     // 5. Принимаем клиентские соединения
//     while (true) {
//         client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
//         if (client_fd < 0) {
//             perror("Ошибка принятия соединения");
//             continue;
//         }

//         // Выводим информацию о подключившемся клиенте
//         char client_ip[INET_ADDRSTRLEN];
//         inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
//         std::cout << "Новое соединение от " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

//         // Отправляем приветственное сообщение клиенту
//         const char* welcome_msg = "Добро пожаловать на сервер IRC!\n";
//         send(client_fd, welcome_msg, std::strlen(welcome_msg), 0);

//         // Закрываем соединение с клиентом
//         close(client_fd);
//     }

//     // Закрываем серверный сокет (хотя код сюда не дойдёт в этом примере)
//     close(server_fd);
// 	return 0;
// }