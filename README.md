# Sala de chat

*Sala de chat* hecha en C que con la implementacion de **sockets** se logra crear una sala de chat, en la cual distintos **clientes** se comunican a travéz de un **servidor**, se implementa el manejo de **señales** para ignorar *SIGPIPE* señales, y el uso de **hilos** en el servidor para procesar multiple de usuarios a la vez, lo cual nos genera una **seccion critica** la cual terminamos manejando mendiante un **semaforo**.

## Este sofware fue probado en Ordenador:

* Intel® Core™ i3-4030U CPU @ 1.90GHz × 4
* 64bits.
* RAM 7,7 GiB
* GNU/LINUX Ubuntu 16.04  

## Construir

Corra make
`make`

## Remover Ejecutables

Corra make clean
`make clean`

## Ejecutar

Ejecutar servidor
`./server [port]`

Ejecutar cliente
`./client [port]`

## Caracteristicas

* Multiples cliente chatenado al mismo tiempo
* Personalizacion del nombre de usuario
* Capacidad para abandonar el chat
* Mostrar la Cantidad de clientes activos

## Comandos

| Comando       | Parametros            | Función                             |
| ------------- | --------------------- | ----------------------------------- |
| #SALIR        |                       | Salir del chat                      |
| #NOMBRE       | [nuevo-nombre]        | Cambiar nombre                      |
| #ACTIVOS      |                       | Show active clients                 |
| #AYUDA        |                       | Mostrar Ayuda                       |


