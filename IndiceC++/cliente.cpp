#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    const int recvBufferSize = 4096; // Tamaño del buffer para recibir datos
    char buffer[recvBufferSize] = {0};

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed" << endl;
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cout << "Socket creation error" << endl;
        WSACleanup();
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8081);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address/ Address not supported" << endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection failed" << endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }
    cout << "Conectado al servidor" << endl;
    string input;
    while (true) {
        cout << "Ingrese las palabras a buscar (formato: palabra1 operador palabra2, o 'exit' para salir): ";
        getline(cin, input);

        if (input == "exit") {
            break;
        }

        send(sock, input.c_str(), input.length(), 0);

        // Recibir la respuesta en fragmentos
        string respuestaCompleta;
        int valread;
        while ((valread = recv(sock, buffer, recvBufferSize, 0)) > 0) {
            respuestaCompleta.append(buffer, valread);
            if (valread < recvBufferSize) {
                break; // Salir del bucle si se ha recibido el último fragmento
            }
        }

        if (!respuestaCompleta.empty()) {
            cout << "Archivos encontrados:\n" << respuestaCompleta << endl;
        } else {
            cout << "No se recibio respuesta del servidor." << endl;
        }

        memset(buffer, 0, sizeof(buffer));
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}