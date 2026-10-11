# Práctica 1 – Middleware (YARP)

  **Repositorio:** `<!-- TODO: PENDIENTE DE AÑADIT -->`

## 0. Configuración adicional en el entorno

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
