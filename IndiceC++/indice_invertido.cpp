#include "indice_invertido.h"

mutex mx;
vector<unordered_map<string, vector<string>>> datosTotalesAgrupados;

Trie::Trie() {
    root = new Node(); // constructor, crea nodo inicial
}

void Trie::insertar(const string& palabra, const string& id_documento) { 
    Node* node = root;
    for (char letra : palabra) { // para cada letra en nuestra palabra
        if (!node->children.count(letra)) { // si existe aun un hijo con esa letra
            node->children[letra] = new Node(); // creamos un nuevo nodo hijo con dicha letra
        }
        node = node->children[letra]; // nos movemos al nodo hijo que contiene la letra
    }
    node->idDocumentos.insert(id_documento);
}

unordered_set<string> Trie::buscar(string& palabra) {
    Node* node = root; 
    for (char letra : palabra) { // para cada letra en nuestra palabra
        if (!node->children.count(letra)) { // si no existe un hijo con esa letra
            return {}; // no existe, devolvemos un conjunto vacío
        }
        node = node->children[letra]; // nos movemos al nodo hijo que contiene la letra
    }
    return node->idDocumentos; // retorna los documentos a los que pertenece el ultimo nodo (final de palabra)
}

string convertirMinuscula(string& texto) {
    string nuevoTexto = texto;

    // Transformamos el texto a minúsculas
    transform(nuevoTexto.begin(), nuevoTexto.end(), nuevoTexto.begin(), ::tolower);
    return nuevoTexto;
}

string eliminarSignos(string& texto) {
    string nuevoTexto;
    for (char caracter : texto) { // para cada caracter en el texto
        if (isalnum(caracter) || caracter == ' ') { // si es alfanumerico o espacio en blanco
            nuevoTexto += caracter; // lo añade a nuevoTexto
        } else if (caracter == '\n') { // si es salto de linea
            nuevoTexto += " "; // añade un espacio
        }
    }
    return nuevoTexto;
}

vector<string> tokenizarTexto(string& texto) {
    vector<string> listaPalabras; // lista de palabras
    
    // istringstream sirve para tratar un string como si fuera un stream de entrada
    istringstream stream(texto); // se crea un stream para el texto
    string palabra; 

    // Se extraen palabra por palabra del stream y se almacenan en la lista
    while (stream >> palabra) { // mientras se pueda extraer una palabra
        listaPalabras.push_back(palabra);  // esa palabra se almacena en la lista
    }
    return listaPalabras; // retorna la lista de palabras (vector)
}

vector<string> eliminarStopWords(vector<string>& listaPalabras, unordered_set<string>& stopWords) {
    vector<string> palabrasFiltradas;
    for (string& palabra : listaPalabras) { // para cada palabra en nuestra lista de palabras
        // Se verifica si la palabra no está en el conjunto de palabras vacías
        if (stopWords.find(palabra) == stopWords.end()) { // si esa palabra no esta en los StopWords
            palabrasFiltradas.push_back(palabra);
        }
    }
    return palabrasFiltradas;
}

vector<PalabraArchivo> mapearArchivos(unordered_map<string, vector<string>>& archivosProcesados) {
    PalabraArchivo pa; 
    vector<PalabraArchivo> datosMappeados;

    for (auto& [id_documento, listaPalabras] : archivosProcesados) { 
        for (string& palabra : listaPalabras) { // para cada palabra en la lista de palabras
            pa.palabra = palabra;
            pa.idDocumento = id_documento;
            datosMappeados.push_back(pa); // la palabraArchivo se almacena en datosMappeados
        }
    }
    return datosMappeados; // retorna los datosMappeados (vector de PalabraArchivo)
}

unordered_map<string, vector<string>> shuffle(vector<PalabraArchivo>& datosMapeados) {
    unordered_map<string, vector<string>> datosAgrupados; //  (palabra - id de documento)
    for (auto& dato : datosMapeados) {
        datosAgrupados[dato.palabra].push_back(dato.idDocumento);
    }
    return datosAgrupados; 
}

void reducirDatos(unordered_map<string, vector<string>>& datosAgrupados, Trie& trie) {
    for (auto& [palabra, id_documentos] : datosAgrupados) {
        for (auto& nom : id_documentos) {
            trie.insertar(palabra, nom); 
        }
    }
}

unordered_set<string> procesarEntrada(Trie& trie, string& entrada) { // procesa la entrada
    istringstream stream(entrada);
    string palabra1; 
    stream >> palabra1; // se extrae la primer palabra
    string operador;
    stream >> operador; // se extrae el operador
    string palabra2;
    stream >> palabra2; // se extrae la segunda palabra

    if (operador == "AND" || operador == "and") { // si es AND (deben estar si o si las 2 palabras)
        // hacemos doble busqueda en el trie
        unordered_set<string> archivosEncontrados1 = trie.buscar(palabra1); 
        unordered_set<string> archivosEncontrados2 = trie.buscar(palabra2);
        unordered_set<string> interseccionArchivos;
        for (const string& ids : archivosEncontrados1) { // para los ids encontrados en la primera busqueda
            // el metodo find devuelve un iterador al elemento si lo encuentra, si no, devuelve un iterador al final ( end() )
            if (archivosEncontrados2.find(ids) != archivosEncontrados2.end()) {
                interseccionArchivos.insert(ids); 
            }
        }
        return interseccionArchivos; // retorna la interseccionArchivos
    } else if (operador == "OR" || operador == "or") { // si el operador es OR (debe contener almenos una de las palabras)
        // hacemos doble busqueda en el trie
        unordered_set<string> archivosEncontrados1 = trie.buscar(palabra1); 
        unordered_set<string> archivosEncontrados2 = trie.buscar(palabra2);
        
        archivosEncontrados1.merge(archivosEncontrados2); // combina los resultados de ambas busquedas
        return archivosEncontrados1;
    } else {
        unordered_set<string> archivosEncontrados1 = trie.buscar(palabra1); // busca la primer palabra
        return archivosEncontrados1;
    }
}

void crearIndiceInvertido(unordered_map<string, string> archivosRecolectados, int inicio, int fin, unordered_set<string>& stopWords) {
    unordered_map<string, vector<string>> archivosProcesados;

    auto it = archivosRecolectados.begin();
    advance(it, inicio); // avanzamos el iterador al inicio
    for (int i = inicio; i < fin && it != archivosRecolectados.end(); ++i, ++it) {
        string id_documento = it->first;
        string texto = it->second;
        texto = eliminarSignos(texto);
        vector<string> listaPalabras = tokenizarTexto(texto);
        vector<string> palabrasFiltradas = eliminarStopWords(listaPalabras, stopWords);
        archivosProcesados[id_documento] = palabrasFiltradas;
    }

    vector<PalabraArchivo> datosMapeados = mapearArchivos(archivosProcesados);
    unordered_map<string, vector<string>> datosAgrupados = shuffle(datosMapeados);

    // Bloquear el acceso a la variable compartida
    lock_guard<mutex> lock(mx);
    datosTotalesAgrupados.push_back(datosAgrupados);
}

int main() {
    // Iniciamos el cronómetro
    auto start = chrono::high_resolution_clock::now();

    // Cargamos las palabras vacias del archivo
    ifstream archivoEntrada("stop_words.txt");
    unordered_set<string> stopWords;
    if (archivoEntrada) {
        string palabra;
        while (getline(archivoEntrada, palabra)) {
            stopWords.insert(palabra);
        }
    } else {
        cerr << "Error al abrir el archivo de palabras vacias." << endl;
        return 1;
    }

    // Lectura de datos en archivo json
    ifstream f("./database/datos_repositorio.json");
    json data = json::parse(f);
    unordered_map<string, string> datosArchivos;
    for (auto& documento : data[2]["data"]) {
        datosArchivos[documento["id_documento"]] = documento["resumen"];
    }
    
    Trie trie;
    int numeroThreads = 8;
    thread threads[numeroThreads];
    int tamanoPorThread = datosArchivos.size() / numeroThreads;
    
    for (int i = 0; i < numeroThreads; ++i) {
        int inicio = i * tamanoPorThread;
        int fin = (i == numeroThreads - 1) ? datosArchivos.size() : (i + 1) * tamanoPorThread;
        threads[i] = thread(crearIndiceInvertido, datosArchivos, inicio, fin, ref(stopWords));
    }
    // Esperamos a que todos los hilos terminen
    for (int i = 0; i < numeroThreads; ++i) {
        if (threads[i].joinable()){
            threads[i].join();
        } else {
            cerr << "Error al unir el hilo " << i << endl;
        }
    }
    // Insertamos las palabras en el trie
    for (auto& datos : datosTotalesAgrupados) {
        reducirDatos(datos, trie);        
    }

    // Detenemos el cronómetro y mostramos el tiempo transcurrido
    auto stop = chrono::high_resolution_clock::now();
    cout << "tiempo total = " << chrono::duration_cast<chrono::milliseconds>(stop - start).count() << " ms" << endl;

    // Solicitar al usuario que ingrese una palabra para buscar en el índice
    string palabraBuscar; 
    bool salir = false; 
    do { 
        cout << "Ingrese una palabra para buscar en el indice invertido (o '0' para terminar): ";
        getline(cin, palabraBuscar); // leemos la palabra
        
        if (palabraBuscar == "0") { // si es 0, salimos del bucle
            salir = true;
        }
        else { 
            palabraBuscar = convertirMinuscula(palabraBuscar);

            unordered_set<string> archivosEncontrados = procesarEntrada(trie,palabraBuscar); // buscamos la palabra y se almacena en archivosEncontrados
            if (archivosEncontrados.empty()) {  
                cout << "La palabra '" << palabraBuscar << "' no esta en el indice invertido." << endl;
            }
            else { // si el resultado no es vacío, imprime los documentos pertenecientes a la palabra
                cout << "La palabra '" << palabraBuscar << "' esta en los documentos:" << endl;
                for (const string& ids : archivosEncontrados) {
                    cout << "- " << ids << endl;
                }
            }
        }
        
    } while (!salir);

    return 0; 
}