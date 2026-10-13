# include <yarp/os/Network.h>
# include <yarp/os/BufferedPort.h>
# include <yarp/os/Bottle.h>
# include <iostream>

using namespace yarp::os;

int main(){
Network yarp;
BufferedPort<Bottle> receiverPort ;
receiverPort.open("/receiver");

receiverPort.setStrict(true);

std::cout << "Receiver is running. Waiting for messages..." << std::endl;

while (true){
Time::delay(1);
Bottle* message = receiverPort.read();
if ( message != nullptr ) {
std::cout << "Receiver received: " << message->toString() << std::endl;
}
}

receiverPort.close ();
return 0;
}