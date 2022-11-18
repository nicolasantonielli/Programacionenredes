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

#include <sys/stat.h>
#include <fcntl.h>

// Constantes
#define PUERTO 8002

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

    // Configuracion de sockets
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
    char sendBuff[1024];
    char recvBuff[1024];
    int estadoRecibir = 0;
    int *descriptorHilo = 0;
    char body[2048];
    descriptorHilo = (int *)params;
    int lengthBody = 0;
    int lengthSend = 0;
    char request[1024];
    int bytesRecibidos = 0;
    
    char homeRequest[1024];
    char imageRequest[1024];
    char faviconRequest[1024];

    char imageHeader[2048];

    int fimage = 0;
 
    memset(sendBuff, '0', sizeof(sendBuff));
    memset(recvBuff, '0', sizeof(recvBuff));
    memset(request, '0', sizeof(request));
    memset(body, '0', sizeof(body));

    bzero(recvBuff, sizeof(recvBuff));
    bzero(request, sizeof(request));
    
    // recibo request
    do
    {
        estadoRecibir = recv(*descriptorHilo, recvBuff, sizeof(recvBuff), 0);

        // Concateno buffer recibido
        strcat(request, recvBuff);

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
            printf("Bytes recibidos: %d bytes\n", estadoRecibir);
            bytesRecibidos += estadoRecibir;
            break;
        }

    } while (1);

    printf("Bytes totales recibidos: %d bytes\n", bytesRecibidos);

    printf("\n****Request recibido****\n");

    printf("%s\n", request);
    

//    bzero(metodo, sizeof(metodo));

    sprintf(homeRequest,"GET / HTTP/1.1");
    sprintf(imageRequest,"GET /image.png HTTP/1.1");
    sprintf(faviconRequest,"GET /favicon.ico HTTP/1.1");

    //Respondo a Home
    if(memcmp(request, homeRequest, 14)==0){

    // printf("\n\nCabezera request: %s\n", request[1]);

        // Armo response del index.html

        // body
        bzero(sendBuff, sizeof(sendBuff));
        bzero(body, sizeof(body));
        sprintf(body, "<!DOCTYPE html\">\r\n<html>\r\n<head>\r\n<title>Servidor_Programacion en redes 2022</title>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" />\r\n</head>\r\n<body>\r\n<h2>Servidor TP Programacion en redes 2022</h2>\r\n<p>PID: %d</p><a href=\"image.png\">image.png</a>\r\n<p>Descriptor numero: %d</p>\r\n</body>\r\n</html>", getpid(), PUERTO,*descriptorHilo);

        // Calculo el length del Body para enviar el content length
        lengthBody = strlen(body);

        // headers
        sprintf(sendBuff, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nAccept-Ranges: bytes\r\nServer: NicoServer_2.0\r\nDate: Sun, 13 Nov 2022 02:49:07 GMT\r\nContent-Length: %d\r\n\r\n", lengthBody);
        
        // Concateno header y body
        strcat(sendBuff, body);
        
        // Calculo el length total a enviar

        printf("\n****Response enviado****\n");
        printf("%s\n", sendBuff);
        lengthSend = strlen(sendBuff);
        printf("\nDescriptor del hilo %d\n", *descriptorHilo);
        send(*descriptorHilo, sendBuff, lengthSend, 0);
    }

    else if(memcmp(request, imageRequest, 22)==0){
    // Envio imagen

    if ((fimage = open("/home/nico/Documentos/afa.png", O_RDONLY))==-1){
        printf("Error no se pudo abrir el archivo\n");
    }

    printf("\nDesciptor de la imagen %d\n", fimage);

    bzero(imageHeader, sizeof(imageHeader));
    
    sprintf(imageHeader, "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nDate: Sun, 13 Nov 2022 02:49:07 GMT\r\nContent-Length: 625688\r\n\r\n");
    lengthSend = strlen(imageHeader);
    send(*descriptorHilo, imageHeader, lengthSend, 0);

    if ((sendfile(*descriptorHilo, fimage, NULL, 625688)) == -1)
    {
        printf("Hubo un error enviando el archivo por el socket\n");
    }


    printf("Enviando imagen\n");

    //cierro imagen
    close(fimage);
    }

    //Respondo 404 a solicitud de favicon
    else if (memcmp(request, faviconRequest, 22)==0)
    {
        
        send(*descriptorHilo, "HTTP/1.1 404 Not Found/r/n", 24, 0);
    }
    
    // Cierro Socket

    close(*descriptorHilo);

    return 0;
}