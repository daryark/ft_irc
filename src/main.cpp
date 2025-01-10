#include <iostream>

#include <cstring>       // Для memset
#include <cstdlib>       // Для exit
#include <unistd.h>      // Для close
#include <sys/socket.h>  // Для socket, bind, listen, accept
#include <netinet/in.h>  // Для sockaddr_in
#include <arpa/inet.h>   // Для htons
// #include "../Irc.hpp"

int main(int argc, char **argv)
{
    if (argc != 3) {
        std::cerr << "try again " << std::endl;
        return 1;
    }

    



	int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 1. Создаем сокет
    server_fd = socket(AF_INET, SOCK_STREAM, 0); // IPv4, потоковый (TCP)
    if (server_fd < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }

    // 2. Настраиваем адрес сервера
    std::memset(&server_addr, 0, sizeof(server_addr)); // Инициализируем структуру нулями
    server_addr.sin_family = AF_INET;                 // Используем IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;         // Привязываем ко всем доступным интерфейсам
    server_addr.sin_port = htons(6667);               // Указываем порт (например, 6667)

    // 3. Привязываем сокет к адресу
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка привязки сокета");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 4. Слушаем входящие соединения
    if (listen(server_fd, 5) < 0) { // Максимум 5 ожидающих соединений
        perror("Ошибка при переходе в режим прослушивания");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Сервер запущен и слушает порт 6667..." << std::endl;

    // 5. Принимаем клиентские соединения
    while (true) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Ошибка принятия соединения");
            continue;
        }

        // Выводим информацию о подключившемся клиенте
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "Новое соединение от " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;

        // Отправляем приветственное сообщение клиенту
        const char* welcome_msg = "Добро пожаловать на сервер IRC!\n";
        send(client_fd, welcome_msg, std::strlen(welcome_msg), 0);

        // Закрываем соединение с клиентом
        close(client_fd);
    }

    // Закрываем серверный сокет (хотя код сюда не дойдёт в этом примере)
    close(server_fd);
	return 0;
}