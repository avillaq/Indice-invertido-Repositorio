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

Trie trie;

void manejarCliente(SOCKET clienteSocket) {
    const size_t recvBufferSize = 1024; // Tamaño del buffer para recibir datos
    char buffer[recvBufferSize] = {0};

    const size_t sendBufferSize = 4096; // Tamaño del buffer para enviar datos

    while (true) {
        int valread = recv(clienteSocket, buffer, recvBufferSize, 0);
        if (valread <= 0) {
            cerr << "Error al leer del socket o cliente desconectado" << endl;
            break;
        }

        string entrada(buffer);
        cout << "Mensaje recibido: " << entrada << endl;
        string respuesta = procesarEntrada(trie, entrada);

        // Respuesta en fragmentos
        size_t totalSent = 0;
        size_t respuestaLength = respuesta.length();
        while (totalSent < respuestaLength) {
            size_t fragmentSize = min(sendBufferSize, respuestaLength - totalSent);
            int sent = send(clienteSocket, respuesta.c_str() + totalSent, static_cast<int>(fragmentSize), 0);
            if (sent == -1) {
                cerr << "Error al enviar la respuesta" << endl;
                break;
            }
            totalSent += sent;
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

    cout << "Creando el indice invertido..." << endl;
    if (iniciar_indice_invertido(trie) != 0) {
        cerr << "Error al crear el indice invertido" << endl;
        closesocket(server_fd);
        return 1;
    }
    cout << "Indice invertido creado" << endl;

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    getnameinfo((struct sockaddr*)&address, sizeof(address), host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    cout << "Servidor IP: " << host << "\nPuerto: " << ntohs(address.sin_port) << endl;
    cout << "Servidor listo para recibir mensajes." << endl;
    
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