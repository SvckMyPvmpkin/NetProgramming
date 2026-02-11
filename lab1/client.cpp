#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Использование: " << argv[0] << " <IP сервера> <Порт> <Число i>" << std::endl;
        return 1;
    }

    const char* serverIp = argv[1];
    int serverPort = std::atoi(argv[2]);
    int i = std::atoi(argv[3]);

    int sid;
    sockaddr_in servAddr{}, clientAddr{};
    hostent *hp;

    if ((sid = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::perror("Ошибка создания сокета");
        return 1;
    }

    servAddr.sin_family = AF_INET;
    hp = gethostbyname(serverIp);
    if (hp == nullptr) {
        std::cerr << "Ошибка: хост не найден" << std::endl;
        return 1;
    }
    std::memcpy(&servAddr.sin_addr.s_addr, hp->h_addr, hp->h_length);
    servAddr.sin_port = htons(serverPort);

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddr.sin_port = 0;
    bind(sid, reinterpret_cast<sockaddr*>(&clientAddr), sizeof(clientAddr));

    std::cout << "=== КЛИЕНТ ЗАПУЩЕН ===" << std::endl;
    std::cout << "Будет отправлено число " << i << " с задержкой " << i << " сек." << std::endl;

    for (int step = 1; step <= 10; ++step) {
        uint32_t toSend = htonl(i);

        if (sendto(sid, &toSend, sizeof(toSend), 0, 
                   reinterpret_cast<sockaddr*>(&servAddr), sizeof(servAddr)) < 0) {
            std::perror("Ошибка отправки");
            break;
        }

        uint32_t received;
        socklen_t servlen = sizeof(servAddr);
        ssize_t mlen = recvfrom(sid, &received, sizeof(received), 0, 
                                reinterpret_cast<sockaddr*>(&servAddr), &servlen);

        if (mlen < 0) {
            std::perror("Ошибка приема");
        } else {
            std::cout << "[" << step << "] Ответ от сервера (преобразованный): " 
                      << ntohl(received) << std::endl;
        }

        sleep(i);
    }

    std::cout << "Пересылка завершена. Выход." << std::endl;
    close(sid);
    return 0;
}