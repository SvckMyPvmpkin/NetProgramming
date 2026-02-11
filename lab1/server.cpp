// #include <iostream>
// #include <cstring>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>

// int main() {
//     int sid;
//     sockaddr_in servAddr{}, clientAddr{};
//     socklen_t len = sizeof(servAddr);

//     sid = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sid < 0) { perror("Socket error"); return 1; }

//     servAddr.sin_family = AF_INET;
//     servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     servAddr.sin_port = 0; 

//     if (bind(sid, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
//         perror("Bind error"); return 1;
//     }

//     getsockname(sid, (struct sockaddr*)&servAddr, &len);
//     std::cout << "SERVER: Работаю на порту: " << ntohs(servAddr.sin_port) << std::endl;

//     while (true) {
//         uint32_t received;
//         len = sizeof(clientAddr);

//         if (recvfrom(sid, &received, sizeof(received), 0, (struct sockaddr*)&clientAddr, &len) > 0) {
//             uint32_t value = ntohl(received);

//             std::cout << "Получено от " << inet_ntoa(clientAddr.sin_addr) 
//                       << ":" << ntohs(clientAddr.sin_port) 
//                       << " значение: " << value << std::endl;

//             uint32_t result = htonl(value + 100);

//             sendto(sid, &result, sizeof(result), 0, (struct sockaddr*)&clientAddr, len);
//         }
//     }
//     return 0;
// }

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sid;
    sockaddr_in servAddr{}, clientAddr{};
    socklen_t len = sizeof(servAddr);

    sid = socket(AF_INET, SOCK_DGRAM, 0);
    if (sid < 0) { perror("Ошибка сокета"); return 1; }

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = 0;

    if (bind(sid, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("Ошибка bind"); return 1;
    }

    getsockname(sid, (struct sockaddr*)&servAddr, &len);
    std::cout << "--- СЕРВЕР ЗАПУЩЕН ---" << std::endl;
    std::cout << "Порт для подключения клиента: " << ntohs(servAddr.sin_port) << std::endl;

    while (true) {
        uint32_t received;
        len = sizeof(clientAddr);

        if (recvfrom(sid, &received, sizeof(received), 0, (struct sockaddr*)&clientAddr, &len) > 0) {
            uint32_t value = ntohl(received);
            
            std::cout << "[Запрос] от " << inet_ntoa(clientAddr.sin_addr) 
                      << ":" << ntohs(clientAddr.sin_port) 
                      << " Данные: " << value << std::endl;

            uint32_t result = htonl(value * 2); 
            sendto(sid, &result, sizeof(result), 0, (struct sockaddr*)&clientAddr, len);
        }
    }
    return 0;
}