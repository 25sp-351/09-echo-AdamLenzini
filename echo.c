#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUF_LEN 1024
#define DEFAULT_PORT 2345
#define LISTEN_BACKLOG 5

// This routine frees fd_ptr
void handleConnection(int* fd_ptr, int print_text) {
    ssize_t returnval;
    char buf[MAX_BUF_LEN];

    printf("Handling connection on %d\n", *fd_ptr);

    while(1) {
        //printf("Reading from %d\n", *fd_ptr);
        returnval = read(*fd_ptr, buf, sizeof(buf));

        if (returnval <= 0) {
            printf("Closing connection on %d\n", *fd_ptr);
            close(*fd_ptr);
            free(fd_ptr);
            break;
        }

        if (print_text == 1)
            printf("%zd, %s", returnval, buf);

        memset(buf, '\0', sizeof(buf)); // Clear the buffer
    }
}

int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;
    int print_text = 0;

    if (argc > 5) {
        printf("Please use the following program call:\n");
        printf("%s -p <port number> (defaults to 2345 if not included) -v (if you want to print text)\n", argv[0]);
        return 1;
    }

    // Checks if the -p flag is present in the command line arguments
    if (argc >= 3 && strcmp(argv[1], "-p") == 0) {
        sscanf(argv[2], "%d", &port);
        if (port <= 1024) {
            printf("Port number must be greater than 1024. Defaulting to 2345\n");
            port = DEFAULT_PORT;
        }
    }

    // Checks if the -v flag is present in the command line arguments
    if ((argc == 4 && strcmp(argv[3], "-v") == 0) || (argc == 2 && strcmp(argv[1], "-v") == 0))
        print_text = 1;        

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(port);

    printf("Binding to port %d\n", port);

    int returnval;

    returnval = bind(socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address));

    if (returnval < 0) {
        perror("bind");
        return 1;
    }

    returnval = listen(socket_fd, LISTEN_BACKLOG);

    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    while (1) {
        int* client_fd_buf = malloc(sizeof(int));

        *client_fd_buf = accept(socket_fd, (struct sockaddr*)&client_address, &client_address_len);
        printf("Accepted connection from %d\n", *client_fd_buf);

        handleConnection(client_fd_buf, print_text);
    }

    return 0;
}