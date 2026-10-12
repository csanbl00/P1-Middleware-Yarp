#include <yarp/os/Network.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <iostream>

using namespace yarp::os;

int main(int argc, char*argv[]) {
// Declarar pero no usar argumentos
YARP_UNUSED (argc);
YARP_UNUSED (argv);

Network yarp; // Inicializa YARP
Bottle bot; // Botella para recibir mensajes
Port input; // Puerto de entrada

// Abrir el puerto receptor
input.open("/receiver");

// Conexion directa entre puertos para este ejemplo
Network::connect ("/custom_sender", "/receiver");

// Leer los mensajes recibidos
bool fin = false;
while (!fin && input.read(bot)){
    for (size_t i = 0; i < bot.size(); i++){
        Value& v = bot.get(i);
        if (v.isString()) {
            std::cout << "[texto] " << v.asString() << std::endl;
            if (v.asString().find("Sequence complete") != std::string::npos)
                fin = true;
        }
        else if (v.isInt32()) std::cout << "[entero] " << v.asInt32() << std::endl;
    }
}

/* Default
input.read(bot);
std::cout << "Got message : " << bot.toString() << std::endl;
*/

// Cerrar el puerto receptor
input.close();

return 0;
}