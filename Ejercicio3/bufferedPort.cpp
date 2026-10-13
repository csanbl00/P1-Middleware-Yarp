# include <yarp/os/Network.h>
# include <yarp/os/BufferedPort.h>
# include <yarp/os/Bottle.h>
# include <iostream>

using namespace yarp::os;

int main(){
Network yarp; // Inicializa la red de YARP

// Crear un puerto de escritura y uno de lectura
BufferedPort<Bottle> senderPort ;
BufferedPort<Bottle> receiverPort ;

// Abrir los puertos
senderPort.open("/example/sender");
receiverPort.open("/example/receiver");


// Conectar los puertos
Network::connect ("/example/sender", "/example/receiver");

// Enviar un mensaje desde el puerto de escritura
Bottle& message = senderPort.prepare();
message.clear();
message.addString("Hello , BufferedPort !");
senderPort.write();

// Leer el mensaje desde el puerto de lectura
Bottle* receivedMessage = receiverPort.read();
if (receivedMessage != nullptr) {
std::cout << " Received : " << receivedMessage->toString() << std::endl;
}

// Cerrar los puertos
senderPort.close ();
receiverPort.close ();

return 0;
}