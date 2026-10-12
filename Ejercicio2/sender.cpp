#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <iostream>

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::Port;

constexpr double loop_delay = 1.0; // Tiempo entre envios
constexpr size_t top = 5; // Numero total de iteraciones

int main(int argc, char*argv[])
{
YARP_UNUSED(argc);
YARP_UNUSED(argv);

Network yarp;
Port output;
output.open("/custom_sender");

for (size_t i = 1; i <= top; i++) {
// Prepara el mensaje
Bottle bot;
bot.addInt32(i);

// Envia el mensaje
output.write(bot);
std::cout<< "Sent message : " << bot.toString() << std::endl;

/* Deault
Bottle bot;
bot.addString("Message");
bot.addInt32(i);
bot.addString("out of");
bot.addInt32(top);
bot.addString("sent by /custom_sender");

// Envia el mensaje
output.write(bot);
std::cout<< "Sent message : " << bot.toString() << std::endl;
*/

// Espera un momento
yarp::os::Time::delay( loop_delay );

// Mensaje adicional en la mitad de las iteraciones
if (i == top / 2) {
Bottle midBot;
midBot.addString (" Halfway through the sequence !");
output.write( midBot);
std::cout << "Sent message : " << midBot.toString() << std::endl;
}
}

// Mensaje final
Bottle endBot;
endBot.addString(" Sequence complete ! Total messages sent:");
endBot.addInt32(top);

output.write(endBot);
std::cout << "Sent message: " << endBot.toString() << std::endl;

output.close();
return 0;
}