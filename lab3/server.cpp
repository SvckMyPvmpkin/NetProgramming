#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

using namespace std;

// Мьютекс для синхронизации записи в файл
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
const char* FILENAME = "received_data.txt";

struct ClientInfo {
    int socket;
    struct sockaddr_in address;
};

void* handle_client(void* arg) {
    ClientInfo* info = (ClientInfo*)arg;
    int client_socket = info->socket;
    char buffer[1024];

    while (true) {
        memset(buffer, 0, 1024);
        int bytes_received = recv(client_socket, buffer, 1024, 0);

        if (bytes_received <= 0) {
            break;
        }

        // ВЫВОД: Только поток и данные (как в замечании)
        cout << "[Поток " << pthread_self() << "] получено: " << buffer << endl;

        // Запись в файл для обеспечения целостности
        pthread_mutex_lock(&file_mutex);
        ofstream file(FILENAME, ios::app);
        if (file.is_open()) {
            file << "Thread: " << pthread_self() << " | Data: " << buffer << endl;
            file.close();
        }
        pthread_mutex_unlock(&file_mutex);
    }

    close(client_socket);
    delete info;
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = 0; // Система сама выберет свободный порт

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Узнаем порт через getsockname
    if (getsockname(server_fd, (struct sockaddr*)&address, &addrlen) == -1) {
        perror("getsockname failed");
        exit(EXIT_FAILURE);
    }

    cout << "Сервер запущен!" << endl;
    cout << "Порт: " << ntohs(address.sin_port) << endl;

    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while (true) {
        ClientInfo* new_client = new ClientInfo();
        new_client->socket = accept(server_fd, (struct sockaddr*)&new_client->address, &addrlen);

        if (new_client->socket >= 0) {
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handle_client, (void*)new_client);
            pthread_detach(thread_id);
        } else {
            delete new_client;
        }
    }

    close(server_fd);
    pthread_mutex_destroy(&file_mutex);
    return 0;
}