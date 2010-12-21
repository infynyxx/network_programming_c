#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in address;
    int listen_socket;
    int new_socket;
    char buffer[25];
    int result;
    int nread;
    int pid;
    int number_of_children = 1;
    int x;
    int val;

    if (argc > 1) {
        number_of_children = atoi(argv[1]);
    }

    listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    val = 1;
    result = setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (result < 0) {
        perror("preforking server");
        return 0;
    }

    //bind socket to local port and address
    address.sin_family = AF_INET;
    address.sin_port = htons(2025);
    address.sin_addr.s_addr = INADDR_ANY;

    result = bind(listen_socket, (struct sockaddr *) &address, sizeof(address));
    if (result < 0) {
        perror("preforking server");
        return 0;
    }

    result = listen(listen_socket, 5);
    if (result < 0) {
        perror("preforking server");
        return 0;
    }

    for (x = 0; x < number_of_children; x++) {
        if ((pid = fork()) == 0) {  //parent process
            while (1) {
                new_socket = accept(listen_socket, NULL, NULL);
                printf("client connected to child process %i.\n", getpid());
                nread = recv(new_socket, buffer, 25, 0);
                buffer[nread] = '\0';
                printf("%s\n", buffer);
                send(new_socket, buffer, nread, 0);
                close(new_socket);
                printf("client disconnected from child process %i\n", getpid());
            }
        }
    }

    wait(NULL);
}

/**
 * **Usage**
 * gcc -o preforking_server.out preforking_server.c
 * ./preforking_server.out
 * in another console: echo "C is Unix" | nc 2025
**/
