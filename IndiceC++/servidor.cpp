#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "indice_invertido.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;

void manejarCliente(SOCKET clienteSocket) {
    char buffer[4096] = {0};
    srand(static_cast<unsigned int>(time(0))); // Seed para generar números aleatorios
    Trie trie;
    iniciar_indice_invertido(trie);

    while (true) {
        int valread = recv(clienteSocket, buffer, 1024, 0);
        if (valread <= 0) {
            cerr << "Error al leer del socket o cliente desconectado" << endl;
            break;
        }


        string entrada(buffer);
        string respuesta = procesarEntrada(trie,entrada);

        if (send(clienteSocket, respuesta.c_str(), static_cast<int>(respuesta.length()), 0) == -1) {
            cerr << "Error al enviar la respuesta" << endl;
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }

    closesocket(clienteSocket);
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cerr << "Socket failed" << endl;
        WSACleanup();
        return 1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        cerr << "Setsockopt failed" << endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8081);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        cerr << "Bind failed" << endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        cerr << "Listen failed" << endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
            cerr << "Accept failed" << endl;
            closesocket(server_fd);
            WSACleanup();
            return 1;
        }

        thread(manejarCliente, new_socket).detach();
    }

    closesocket(server_fd);
    WSACleanup();

    return 0;
}