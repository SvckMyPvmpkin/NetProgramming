// #include <iostream>
// #include <cstring>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <unistd.h>

// int main(int argc, char* argv[]) {
//     if (argc < 2) {
//         std::cerr << "Использование: " << argv[0] << " <порт_сервера>" << std::endl;
//         return 1;
//     }

//     int sid = socket(AF_INET, SOCK_DGRAM, 0);
//     int port = std::atoi(argv[1]);

//     const uint32_t i = 2;
    
//     sockaddr_in servAddr{};
//     servAddr.sin_family = AF_INET;
//     servAddr.sin_port = htons(port);
//     servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

//     std::cout << "CLIENT: Начинаю отправку числа " << i << " на localhost:" << port << std::endl;

//     for (int step = 1; step <= 10; ++step) {
//         uint32_t msg = htonl(i);
//         sendto(sid, &msg, sizeof(msg), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));

//         uint32_t response;
//         socklen_t len = sizeof(servAddr);
//         if (recvfrom(sid, &response, sizeof(response), 0, (struct sockaddr*)&servAddr, &len) > 0) {
//             std::cout << "Сервер вернул преобразованное число: " << ntohl(response) << std::endl;
//         }

//         sleep(i);
//     }

//     std::cout << "Работа завершена." << std::endl;
//     close(sid);
//     return 0;
// }

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Использование: " << argv[0] << " <порт>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    const uint32_t i = 2;

    int sid = socket(AF_INET, SOCK_DGRAM, 0);
    if (sid < 0) { perror("Ошибка сокета"); return 1; }

    char host[256];
    gethostname(host, sizeof(host));
    struct hostent* hp = gethostbyname(host);
    
    if (hp == nullptr) {
        std::cerr << "Не удалось определить IP автоматически" << std::endl;
        return 1;
    }

    sockaddr_in servAddr{};
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    std::memcpy(&servAddr.sin_addr.s_addr, hp->h_addr, hp->h_length);

    std::cout << "--- КЛИЕНТ ЗАПУЩЕН ---" << std::endl;
    std::cout << "Автоматически определенный IP сервера: " << inet_ntoa(servAddr.sin_addr) << std::endl;
    std::cout << "Отправка числа " << i << " с задержкой " << i << " сек." << std::endl;

    for (int step = 1; step <= 10; ++step) {
        uint32_t msg = htonl(i);
        sendto(sid, &msg, sizeof(msg), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));

        uint32_t response;
        socklen_t len = sizeof(servAddr);
        if (recvfrom(sid, &response, sizeof(response), 0, (struct sockaddr*)&servAddr, &len) > 0) {
            std::cout << "[" << step << "] Ответ сервера: " << ntohl(response) << std::endl;
        }

        sleep(i);
    }

    std::cout << "Готово." << std::endl;
    close(sid);
    return 0;
}