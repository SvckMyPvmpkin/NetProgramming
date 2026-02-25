#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char const *argv[]) {
    // Проверка аргументов: программа, IP, Порт, Число N
    if (argc != 4) {
        cout << "Использование: " << argv[0] << " <IP сервера> <Порт> <Число N>" << endl;
        return -1;
    }

    const char* server_ip = argv[1];
    int server_port = stoi(argv[2]);
    int N = stoi(argv[3]); // Получаем N из аргументов запуска

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Ошибка создания сокета" << endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        cout << "Неверный адрес" << endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Ошибка соединения" << endl;
        return -1;
    }

    string message = to_string(N);
    cout << "Начинаю отправку числа " << N << " каждые " << N << " сек." << endl;

    while (true) {
        if (send(sock, message.c_str(), message.length(), 0) < 0) {
            cout << "Связь разорвана" << endl;
            break;
        }
        cout << "Отправлено: " << N << endl;
        sleep(N);
    }

    close(sock);
    return 0;
}