# Práctica 1 – Middleware (YARP)

  **Repositorio:** `<!-- TODO: PENDIENTE DE AÑADIT -->`

## Configuración adicional en el entorno

La práctica recomienda lanzar el contenedor con `docker run --rm`, lo que borra el contenedor (y todo lo que hayas hecho dentro) al salir. Antes de empezar quise tener una carpeta permanente donde guardar el código y los resultados sin perderlos. Esto es lo que investigué:

1. Con `--rm` todo lo escrito dentro del contenedor desaparece al cerrarlo, así que busqué cómo conservar una carpeta de trabajo.
2. Docker permite montar una carpeta del host dentro del contenedor (denominado bind mount) con `-v ruta_host:ruta_contenedor`.
3. Verifiqué que `/workspace` en el contenedor era la misma carpeta que había enlazado en el host (`WorkingFolder`).
4. Al intentar editar la carpeta enlazada desde el contenedor, el sistema me lo impedía:

   ![](img/01-permission-denied.png)

5. Probé a lanzar el contenedor con mi propio usuario (`--user "$(id -u):$(id -g)"`). Puesto que el contenedor no conoce ese UID, el sistema estaba trabajando sin usuario, A parte del trauma visual, supuse que esto podía interferir en el correcto funcionamiento del contenedor por lo que busqué otra manera.

   ![](img/02-user-sin-nombre.png)

6. Como los permisos se asignan por UID, mi usuario (`cesar`) es el 1000, pero el usuario `codespace` de la imagen es el 33333, y la carpeta del host no le concedía acceso. La solución fue darle permisos explícitos con ACL (`setfacl`), incluyendo la ACL por defecto para que los ficheros nuevos hereden el permiso:

   ![](img/03-setfacl-permisos.png)

7. Con eso ya se puede leer y escribir desde el contenedor. El comando final queda así:

   docker run -it --rm -p 6080:6080 --user codespace -v /home/cesar/Escritorio/WorkingFolder:/workspace ghcr.io/robotology/icub-gazebo-grasping-sandbox:latest

### ¿Por qué esta solución y no un `docker cp` al final?

Aunque no es una diferencia dramática, por preferencia personal me quedo con esta forma de trabajar ya que me parece más natural trabajar simplemente desde esa carpeta, guardándose en el host en tiempo real. No hay que acordarse de copiar nada y, si el contenedor falla o se cierra (con `--rm` ya no se puede hacer `docker cp`), no se pierde trabajo. Además, puedo editar desde el IDE del host y ejecutar dentro del contenedor sobre los mismos archivos, que ya quedan listos para `git` y para el zip de Moodle.

## Ejercicio 1: Comandos básicos de YARP en la línea de comandos

### 1. Verificar el entorno de YARP

* **`yarp check`**: ejecuta un diagnóstico del entorno: comprueba la instalación, la red y si el servidor de nombres es alcanzable, y hace una prueba de comunicación entre puertos.

![](img/04-yarpcheck.png)

* **`yarp conf <hostname> <port>`**: guarda en la configuración local la dirección (IP y puerto) del servidor de nombres al que se conectarán los programas YARP de esa máquina.

### 2. Iniciar y verificar el servidor de nombres

* **`yarpserver`**: arranca el servidor de nombres, que registra los puertos y resuelve dónde está cada uno para poder conectarlos. Podemos acceder a la interfaz usando la dirección y puerto donde se ha levantado el servicio.
![](img/05-yarpserver.png)

* **`yarp where`**: muestra dónde está el servidor de nombres que se está usando (IP y puerto). Como el servidor está levantado en otra sesion del contenedor, al ejecutar el comando en un nuevo terminal, detecta esta localización, Nótese que coincide con la IP y puerto donde se levantó el servicio.
![](img/06-yarpwhere.png)

### 3. Crear y manipular puertos

* **`yarp write /example/writer`**: crea un puerto de escritura y envía a través de él lo que se escribe por teclado.
* **`yarp read /example/reader`**: crea un puerto de lectura que imprime en la terminal lo que recibe.
![](img/07-yarpmessages.png)

* **`yarp connect /example/writer /example/reader`**: conecta un puerto de salida con uno de entrada, de modo que lo escrito en el primero llega al segundo. Nótese que sin esto, los mensajes enviados por el writer no llegarán al reader.
* **`yarp disconnect /example/writer /example/reader`**: elimina esa conexión, sin cerrar los puertos.
* **`yarp name list`**: lista los puertos registrados en el servidor de nombres. Nótese que se puede apreciar cómo Yarp les da un puerto distinto y secuencial dentro de la dicrección IP local.
![](img/08-yarpnamelist.png)

### 4. Pruebas y diagnóstico

* **`yarp ping /example/reader`**: comprueba que el puerto responde y muestra su información (dirección y conexiones). Para ello el puerto debe estar abierto.
* **`yarp read /example/reader`**: *ver Ejercicio 3*.
* **`yarp write /example/writer`** : *ver Ejercicio 3*.

### 5. Manipulación avanzada de puertos

* **`yarp write --anon`**: no parece funcionar. En la documentacion (https://www.yarp.it/latest/group__yarp.html#yarp_write) tampoco parece indicar la existencia de esta configuración.
![](img/09-yarp-anon.png)

* **`yarp name register /my/port`**: reserva un nombre en el servidor de nombres. De nuevo, Yarp asigna un puerto distinto y secuencial dentro de la dicrección IP local.
* **`yarp name unregister /my/port`**: borra ese nombre del servidor de nombres.

### 6. Limpiar el estado del servidor de nombres

* **`yarp clean`**: revisa los puertos registrados y elimina los que ya no responden. En mi caso, ha borrado el puerto creado con yarp name register, que no estaba siendo usado, simplemente fue definidio.
![](img/10-yarp-clean.png)

### 7. Redes distribuidas

* **`yarp check --remote <hostname>:<port>`**: parece ignorar los parámetros y simplemente hace un yarp check. Tampoco aparece en la documentación.
![](img/11-yarpcheck--remote.png)


* **`yarp name reconfigure <port> <new-server>`**: no parece funcionar y tampoco aparece en la documentación.

### 8. Configuración básica de puertos desde la línea de comandos

(Comandos ejecutados previamente)

### Preguntas

* **¿Qué sucede si intentas enviar un mensaje antes de conectar los puertos?** Que el mensaje no llega a ninguna parte.
* **¿Cómo se desconectan los puertos desde la terminal?** Con `yarp disconnect /sender /receiver`.
* **Si creas un segundo lector `/receiver2` y lo conectas al escritor, ¿ambos reciben el mensaje?** Sí. Un puerto de salida envía una copia a cada conexión que tenga, así que /receiver y /receiver2 reciben el mismo mensaje.


## Ejercicios 2: Uso de Puertos en Yarp

Como el script del receiver no contiene un bucle como el sender, se puede observar que simplemente se ejecuta y capta un mensaje puntual antes de llegar a la instrucción de cerrar el puerto y terminar el programa.
![](img/12-yarp-ejercicio2.png)

### Preguntas

* **¿Que sucede si los puertos no se conectan?** Que el mensaje no llega a ninguna parte, de forma similar a como pasaba por comando de terminal.
* **¿Cómo podrías modificar el nodo receptor para manejar múltiples mensajes con diferentes formatos?** Comprobando el posible tipo de cada elemento con `get(i)` y los métodos de `Value (isString(), isInt32()... etc)` según la documentacion en https://www.yarp.it/latest/classyarp_1_1os_1_1Value.html.


```cpp
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
  ```

  ![](img/13-yarp-types.png)

## 2.1. Conexión Directa entre Puertos en YARP

![](img/14-sender-and-receiver.png)

### Preguntas

* **¿Cuáles son los pasos realizados?**
  1. Se inicializa YARP (Network yarp;).
  2. Se crean dos `BufferedPort<Bottle>, writerPort y readerPort`, y se abren con los nombres /example/writer y /example/reader.
  3. Se conecta el escritor con el lector desde el código con `writerPort.addOutput("/example/reader")`.
  4. Se prepara el mensaje `(prepare(), clear(), addString("Hello, YARP!"))` y se envía con `write()`.
  5. El lector lo recibe con `read()` y se imprime: `Received message :"Hello, YARP!"`.
  6. Se cierran los dos puertos, y en la captura se ve cómo YARP elimina la conexión.

* **¿Cuál es la diferencia entre Network::connect y Port::addOutput?** `Network::connect(origen, destino)` es una función estática de red que conecta dos puertos por su nombre. `addOutput(destino)` es un método de un puerto concreto que añade una conexión de salida desde ese puerto hacia otro.
* **¿Cómo cambiarías los `cout` por mensajes de logging?** Usar las funcionalidades que ofrece Yarp es interesante porque, según la documentación (https://www.yarp.it/latest/yarp_logging.html), además de recolectar varios tipos de información, se ofrece la opción de mandarlos al yarplogger. De los 6 distintos niveles del logging que muestra la documentación, se ha probado `yInfo`, `yWarning` y `yError`. Como se ha usado la forma con `<<`, ha sido necesario incluir la librería `<yarp/os/LogStream.h>`. Además, se puede observar que Yarp usa este tipo de logging (en concreto [INFO]) para mostrar información adicional sobre los procesos como cerrar puertos. 

![](img/15-yarp-logging.png)

## Ejercicios 3. BufferedPort en YARP

![](img/16-yarp-bufferedPort.png)

### Preguntas

* **Descripción del programa:** En un mismo programa se crean dos `BufferedPort<Bottle>`, `/example/sender` y `/example/receiver`, y se conectan con `Network::connect`. El emisor prepara una botella con `prepare()`, le añade la cadena `"Hello , BufferedPort !"` y la envía con `write()`. El receptor la recoge con `read()`, que devuelve un puntero a la botella (o `nullptr` si no hay), y la imprime. Por último se cierran los dos puertos. En la captura se ve el mensaje recibido y cómo YARP elimina la conexión al cerrar.

* **Lo más característico:** El `BufferedPort<Bottle>` que guarda los mensajes en un búfer interno, facilitando atomaticamente la sincronización entre el envío y la recepción de datos, y evitando el bloqueo del flujo del programa.

### Ejercicio Propuesto: Uso de BufferedPort en Comunicación Simultánea

**Objetivo:** Practicar el uso de BufferedPort para manejar la comunicación entre varios productores y un único consumidor.

El programa (`bufferedPortSync.cpp`) crea dos productores (`/producer1`, `/producer2`) y un consumidor (`/consumer`), todos `BufferedPort<Bottle>`, y conecta ambos productores al consumidor.

![](img/17%2018%20good.png)

### Preguntas

* **Descripción del proceso:** En cada ronda, los dos productores envían un mensaje y el consumidor hace dos `read()` para mostrarlos. Sin un time delay entre los dos envíos, el programa se quedaba bloqueado ya que los mensajes llegaban casi a la vez, uno se descartaba y el segundo `read()` esperaba indefinidamente. Según la docuemntacion (https://www.yarp.it/latest/classyarp_1_1os_1_1BufferedPort.html), esto podría explicase con "messages may be dropped by the writer if BufferedPort::write is called too quickly The reader may also drop old messages if BufferedPort::read is not called fast enough". Añadiendo un pequeño `Time::delay` entre ambos envíos los mensajes ya no coinciden y se reciben todos en perfecto orden.

* **Código clave:**

  ```diff
   for (int i = 1; i <= top; i++) {
        // El productor 1 envia su mensaje
        Bottle& msg1 = producer1.prepare();
        msg1.clear();
        msg1.addString("Mensaje de producer1");
        msg1.addInt32(i);
        producer1.write();

  +      Time::delay(loop_delay/10);

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
  ```

## Ejercicios 4: Tiempo real en YARP

**Objetivo:** El objetivo del ejercicio es diseñar un programa en YARP que permita gestionar el comportamiento por defecto en las colas de mensajes.

Con el código base, `setStrict(true)` o `setStrict(false)` daba igual ya que el receptor lee en un bucle infinito sin pausa, mientras que el cuello de botella está en el sender, que espera entre envíos. El receptor siempre está listo antes de que llegue el siguiente mensaje, así que se reciben todos:

![](img/19-yarp-strict-no-effect.png)

Para que se note el parámetro hay que invertir la situación, haciendo que el receptor tarde más en procesar (time delay más alto en este caso) que el sender en enviar.

* **`setStrict(false)`:** el receptor solo recibe uno de cada dos mensajes (1, 2, 4, 6, 8…). Los mensajes que llegan sin haberse leído se descartan y siempre se procesa el más reciente.

  ![](img/20-yarp-strict-false.png)

* **`setStrict(true)`:** el receptor recibe todos los mensajes en orden (1, 2, 3, 4…), pero se va quedando atrás: cuando el sender va por el 17, el receptor solo ha mostrado hasta el 9. Los mensajes pendientes se acumulan en el búfer.

  ![](img/21-yarp-strict-true.png)

### Preguntas

* **Puerto del emisor (`/sender`):** gestionar la frecuencia de emisión de información, ya que esto puede saturar al receptor y, según la configuración, perder información relevante.

* **Puerto del receptor (`/receiver`):** tener en cuenta una posible saturación debido a que la velocidad de procesamiento sea menor a la velocidad de emisión del sender y actuar en consecuecnia con una configuración adecuada.

* **Frecuencia de envío:** Como se ha explicado previamente, una frecuencia demasiado alta puede saturar al receptor y perderinformación relevante.

* **Ventajas y desventajas de `setStrict()`:** con `true` no se pierde ningún mensaje y se procesan en orden, pero si el receptor es más lento que el emisor, acumula retraso y trabaja con datos cada vez más antiguos. Con `false` (por defecto) el receptor siempre trabaja con el dato más reciente y sin retraso, que es lo que interesa en tiempo real (control, sensores), a costa de perder mensajes intermedios.

