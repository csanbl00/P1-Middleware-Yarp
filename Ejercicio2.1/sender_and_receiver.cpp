# include <yarp/os/Network.h>
# include <yarp/os/BufferedPort.h>
# include <yarp/os/Bottle.h>
# include <iostream>
# include <yarp/os/Log.h>
# include <yarp/os/LogStream.h>

using namespace yarp ::os;

int main(){
Network yarp; // Inicializa la red de YARP

// Crear puertos de escritura y lectura
BufferedPort <Bottle> writerPort;
BufferedPort <Bottle> readerPort;

// Abrir los puertos con nombres especificos
writerPort.open("/example/writer");
readerPort.open("/example/reader");

// Conectar el puerto de escritura con el de lectura directamente
writerPort.addOutput("/example/reader");

// Enviar un mensaje desde el puerto de escritura
Bottle & message = writerPort.prepare();
message.clear();
message.addString("Hello, YARP!");
writerPort.write();

// Leer el mensaje en el puerto de lectura
Bottle * receivedMessage = readerPort.read();
if (receivedMessage != nullptr) {
yInfo() << "Received message :" << receivedMessage->toString();
//std::cout << "Received message :" << receivedMessage->toString() << std::endl;
}

yWarning("Los puertos no se han cerrado!");

// Cerrar los puertos
writerPort.close ();
readerPort.close ();

yError("Se cerraron los puertos");

return 0;
}