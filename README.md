# ÍNDICE INVERTIDO EN C++

## Descripción

Este repositorio contiene dos aplicaciones desarrolladas en C++: `cliente` y `servidor`. Estas aplicaciones están diseñadas para comunicarse a través de sockets TCP, proporcionando una implementación de índice invertido en consola.

## Tecnologías Usadas

- **C++**: Lenguaje de programación principal.
- **Sockets TCP**: Para la comunicación entre cliente y servidor.
- **JSON**: Para el manejo de datos estructurados.
- **nlohmann/json**: Biblioteca para el manejo de JSON en C++.

## Estructura del Proyecto

- `IndiceC++`: Implementación en consola del índice invertido.
  - `database`: Datos para el índice invertido.
  - `libraries`: Blibliotecas nlohmann/json utilizadas.
  - `servidor.cpp`: Código fuente del servidor.
  - `cliente.cpp`: Código fuente del cliente.
  - `indice_invertido.cpp`: Implementación del índice invertido.
  - `indice_invertido.h`: Declaraciones del índice invertido.

## Configuración y Ejecución

### Requisitos

- **Compilador C++**: Necesario para compilar el código fuente.
- **Biblioteca nlohmann/json**: Para el manejo de JSON en C++.

### Instrucciones de Compilación y Ejecución

#### Índice Invertido en Consola

1. **Compilación**:
    ```bash
    g++ servidor.cpp indice_invertido.cpp -o servidor -lws2_32
    g++ cliente.cpp -o cliente -lws2_32
    ```

2. **Ejecución**:
    - Ejecutar el servidor:
        ```bash
        ./servidor
        ```
    - Ejecutar el cliente:
        ```bash
        ./cliente
        ```

## Conexión entre Múltiples Usuarios

### Instrucciones

1. **Verificar Conexión en la Misma Red**:
    - Asegúrate de que todos los dispositivos estén conectados a la misma red.

2. **Configurar Dirección IP y Puerto**:
    - La dirección IP y el puerto deben ser los mismos en el cliente y el servidor.

#### En Windows

1. **Verificar IP del Servidor**:
    - Abre el Símbolo del Sistema y ejecuta el siguiente comando:
        ```bash
        ipconfig
        ```
    - Verifica que la IP sea la misma que la dirección IPv4.

#### En Linux

1. **Verificar IP del Servidor**:
    - Abre una terminal y ejecuta el siguiente comando:
        ```bash
        ifconfig
        ```
    - Verifica que la IP sea la misma que la dirección IPv4.

## Ejemplo de Uso

1. **Iniciar el Servidor**:
    - Ejecuta el servidor en la máquina designada como servidor.

2. **Conectar el Cliente**:
    - Ejecuta el cliente en otra máquina y conéctalo al servidor utilizando la dirección IP y el puerto configurados.

3. **Enviar Consultas**:
    - Ingresa las palabras a buscar en el cliente y envía la consulta al servidor.
    - El servidor procesará la consulta y devolverá los resultados al cliente.

## Problemas Comunes

- **Error de Conexión**:
    - Verifica que la dirección IP y el puerto sean correctos.
    - Asegúrate de que el servidor esté en ejecución.
    - Verifica la configuración del firewall.

- **Datos Incompletos**:
    - Asegúrate de que el cliente maneje correctamente la recepción de datos en fragmentos.