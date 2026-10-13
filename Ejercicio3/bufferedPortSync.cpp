#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <iostream>

using namespace yarp::os;

constexpr double loop_delay = 1.0;
constexpr int top = 5;

int main() {
    Network yarp; // Inicializa la red de YARP

    // Dos productores y un unico consumidor
    BufferedPort<Bottle> producer1;
    BufferedPort<Bottle> producer2;
    BufferedPort<Bottle> consumer;

    // Abrir los puertos
    consumer.open("/consumer");
    producer1.open("/producer1");
    producer2.open("/producer2");

    // Conectar ambos productores al mismo consumidor
    Network::connect("/producer1", "/consumer");
    Network::connect("/producer2", "/consumer");

    for (int i = 1; i <= top; i++) {
        // El productor 1 envia su mensaje
        Bottle& msg1 = producer1.prepare();
        msg1.clear();
        msg1.addString("Mensaje de producer1");
        msg1.addInt32(i);
        producer1.write();

        Time::delay(loop_delay/10);

        // El productor 2 envia su mensaje
        Bottle& msg2 = producer2.prepare();
        msg2.clear();
        msg2.addString("Mensaje de producer2");
        msg2.addInt32(i);
        producer2.write();

        // El consumidor lee los dos mensajes de esta ronda y los muestra
        for (int j = 0; j < 2; j++) {
            Bottle* received = consumer.read();
            if (received != nullptr) {
                std::cout << "Consumer received : " << received->toString() << std::endl;
            }
        }

        // Espera antes de la siguiente ronda
        Time::delay(loop_delay);
    }

    // Cerrar los puertos
    producer1.close();
    producer2.close();
    consumer.close();

    return 0;
}
