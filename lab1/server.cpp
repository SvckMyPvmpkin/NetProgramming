#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sid;
    socklen_t len;
    sockaddr_in servAddr{}, clientAddr{};

    sid = socket(AF_INET, SOCK_DGRAM, 0);
    if (sid < 0) {
        std::perror("Ошибка при открытии сокета");
        return 1;
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = 0; 

    if (bind(sid, reinterpret_cast<sockaddr*>(&servAddr), sizeof(servAddr)) < 0) {
        std::perror("Ошибка связывания (bind)");
        return 1;
    }

    len = sizeof(servAddr);
    if (getsockname(sid, reinterpret_cast<sockaddr*>(&servAddr), &len) < 0) {
        std::perror("Ошибка getsockname");
        return 1;
    }

    std::cout << "СЕРВЕР ЗАПУЩЕН" << std::endl;
    std::cout << "Сервер выбрал порт: " << ntohs(servAddr.sin_port) << std::endl;
    std::cout << "Ожидание сообщений от клиентов..." << std::endl;

    while (true) {
        uint32_t receivedValue;
        len = sizeof(clientAddr);

        ssize_t mlen = recvfrom(sid, &receivedValue, sizeof(receivedValue), 0, 
                                reinterpret_cast<sockaddr*>(&clientAddr), &len);
        if (mlen < 0) {
            std::perror("Ошибка при получении данных");
            continue;
        }

        uint32_t value = ntohl(receivedValue);

        std::cout << "\n[Новый запрос]" << std::endl;
        std::cout << "IP клиента: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
        std::cout << "Порт клиента: " << ntohs(clientAddr.sin_port) << std::endl;
        std::cout << "Получено число: " << value << std::endl;

        uint32_t transformedValue = value * 2;
        std::cout << "Преобразовано (x2): " << transformedValue << std::endl;

        uint32_t toSend = htonl(transformedValue);
        sendto(sid, &toSend, sizeof(toSend), 0, 
               reinterpret_cast<sockaddr*>(&clientAddr), len);
    }

    close(sid);
    return 0;
}