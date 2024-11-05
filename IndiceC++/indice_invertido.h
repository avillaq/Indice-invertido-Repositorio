#ifndef INDICE_INVERTIDO_H
#define INDICE_INVERTIDO_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <stack>
#include "./libraries/json.hpp"

using json = nlohmann::json;
using namespace std;

extern mutex mx;
extern vector<unordered_map<string, vector<string>>> datosTotalesAgrupados;

// Nodo del Trie
struct Node {
    unordered_map<char, Node*> children; // hijo, contiene un caracter y una referencia a un hijo
    unordered_set<string> idDocumentos;  // id de documentos que contienen la palabra hasta ese punto
};

// Clase Trie
class Trie {
private:
    Node* root; // nodo inicial

public:
    Trie();
    void insertar(const string& palabra, const string& id_documento);
    unordered_set<string> buscar(const string& palabra);
};

// Función para convertir texto a minúsculas
string convertirMinuscula(string& text);

// Función para eliminar signos de puntuación y saltos de linea
string eliminarSignos(string& texto);

// Función para tokenizar un texto (separa palabra por palabra)
vector<string> tokenizarTexto(string& texto);

// Función para eliminar palabras que no brindan informacion
vector<string> eliminarStopWords(vector<string>& listaPalabras, unordered_set<string>& stopWords);

// Estructura auxiliar para almacenar la palabra y el id de documento
struct PalabraArchivo { 
    string palabra;
    string idDocumento;
};

// Función para mapear los archivos procesados
vector<PalabraArchivo> mapearArchivos(unordered_map<string, vector<string>>& archivosProcesados);

// Organización de los datos intermedios: Agrupación por clave (palabra, palabra, ...)
unordered_map<string, vector<string>> shuffle(vector<PalabraArchivo>& datosMapeados);

// Reducir combinando listas de id de documentos para cada palabra usando un Trie
void reducirDatos(unordered_map<string, vector<string>>& datosAgrupados, Trie& trie);

unordered_set<string> procesarEntrada(Trie& trie, string& entrada);

// Funcion para buscar los documentos completos
string buscarDocumentosCompletos(unordered_map<string,json>& docsCompletos, unordered_set<string>& archivosEncontrados);

// Esta funcion se ejecutara en paralelo
void crearIndiceInvertido(unordered_map<string, string> archivosRecolectados, int inicio, int fin, unordered_set<string>& stopWords);

#endif // INDICE_INVERTIDO_H