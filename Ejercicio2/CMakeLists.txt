cmake_minimum_required (VERSION 3.5)

# Nombre del proyecto
project (YARPExercises)

# Buscar YARP
find_package (YARP REQUIRED)

# Incluir cabeceras de YARP
include_directories (${YARP_INCLUDE_DIRS})

# Anyadir ejemplos
add_executable (sender sender.cpp)
add_executable (receiver receiver.cpp)
#add_executable (multithread multithread.cpp)

# Vincular los ejecutables con las bibliotecas de YARP
target_link_libraries (sender ${YARP_LIBRARIES})
target_link_libraries (receiver ${YARP_LIBRARIES})
#target_link_libraries (multithread ${YARP_LIBRARIES})

