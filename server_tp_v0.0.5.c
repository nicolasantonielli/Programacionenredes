#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sendfile.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

// Constantes
#define PUERTO 8080

//
void *atenderCliente(void *params);

int main(int argc, char *argv[])
{
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in server_addr;
    struct sockaddr cliente_addr;

    char sendBuff[1025];

    pthread_attr_t atributos;

    // creacion del socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // Inicializacion de variables
    memset(&server_addr, '0', sizeof(server_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    //Configuracion de sockets
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PUERTO);

    // Nombro el Socket
    bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Definicion de modo escucha y seteo la cola en 10
    listen(listenfd, 10);

    printf("Escuchando en puerto %d...\n", PUERTO);

    // Asigno atributos del thread
    pthread_attr_init(&atributos);
    pthread_attr_setdetachstate(&atributos, PTHREAD_CREATE_JOINABLE);

    while (1)
    {
        // Acepto las conexiones y paso parametros al thread
        socklen_t size;
        connfd = accept(listenfd, &cliente_addr, &size);

        if (connfd < 0)
        {

            printf("Hubo un error.");
        }

        printf("Coneccion establecida en socket %d\n", connfd);

        pthread_t clienteThread;

        //
        if (pthread_create(&clienteThread, NULL, atenderCliente, &connfd) != 0)
        {
            perror("No puedo crear thread");
            exit(-1);
        }
    }

    close(listenfd);
}

// Funcion de los hilos
void *atenderCliente(void *params)
{
    int pid = 0;
    int ip = 0;
    char sendBuff[8192];
    char recvBuff[1024];
    int estadoRecibir = 0;
    int *descriptorHilo = 0;
    char body[2048];
    descriptorHilo = (int *)params;
    int length = 0;
    char metodo[4];
    char request[8192];
    int bytesRecibidos = 0;
    char headerImage[2048];
    char imageBody[127];

    int fimage = NULL;

    fimage = fopen("/home/nico/Documentos/2x2.png","r");

    /*
    memset(sendBuff, '0', sizeof(sendBuff));
    memset(sendBuff, '0', sizeof(recvBuff));
    // memset(request, '0', sizeof(request));
    memset(body, '0', sizeof(body));
    memset(metodo, '0', sizeof(metodo));

    // recibo request
    do
    {
        estadoRecibir = recv(*descriptorHilo, recvBuff, sizeof(recvBuff), 0);
        if (estadoRecibir == 0)
        {
            printf("Conexión cerrada por el cliente.\n");
            break;
        }
        if (estadoRecibir == -1)
        {
            printf("Error recibiendo en socket\n");
            break;
        }
        if (estadoRecibir != 0)
        {
            printf("Recibiendo datos\n");
            printf("Bytes recibidos %d\n", estadoRecibir);
            bytesRecibidos += estadoRecibir;
        }

        printf("Bytes recibidos: %d bytes\n", bytesRecibidos);
        printf("%s", request);

        length = 360;

        sprintf(body, "<!DOCTYPE html\">\r\n<html>\r\n<head>\r\n<title>Servidor_Programacion en redes 2022</title>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\r\n</head>\r\n<body>\r\n<h2>Servidor TP Programacion en redes 2022</h2>\r\n<p>PID: %d</p><a href=\"http://127.0.0.1/image.jpg\">image.jpg</a>\r\n<p>IP del servidor: %d</p>\r\n<p>Descriptor numero: %d</p>\r\n</body>\r\n</html>", getpid(), ip, *descriptorHilo);

        sprintf(sendBuff, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nAccept-Ranges: bytes\r\nServer: NicoServer_2.0\r\nDate: Sun, 13 Nov 2022 02:49:07 GMT\r\nContent-Length: %d\r\n\r\n", length);
        strcat(sendBuff, body);

        printf("%s", sendBuff);
        printf("\nDescriptor del hilo %d\n", *descriptorHilo);
        send(*descriptorHilo, sendBuff, sizeof(sendBuff), 0);


text/html
image/png
image/jpeg
    } while (1);
    */
        sprintf(headerImage, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nDate: Sun, 13 Nov 2022 02:49:07 GMT\r\nContent-Length: 127\r\n\r\n");
        //sprintf(imageBody,"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
        //strcpy(imageBody,(char*)fimage);
        send(*descriptorHilo, headerImage, 102, 0);

        while(fgets(imageBody, sizeof(imageBody), fimage) != NULL) {
            if (send(*descriptorHilo, imageBody, sizeof(imageBody), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
            }
        bzero(imageBody, 127);
        }


        sendfile(*descriptorHilo,fimage,NULL,127);
        printf("Enviando imagen\n");
    fclose(fimage);
}


//https://idiotdeveloper.com/file-transfer-using-tcp-socket-in-c/