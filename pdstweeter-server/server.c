#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

#define TOTAL_CLIENTS   50
#define PORT            8888
#define BUFFER_SIZE     256


void log_error(char*);

int main(int argc, char* argv[])
{
    // Create errors log file
    freopen("error_log.txt", "a", stderr);

    // Create a server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) log_error("Creating server socket");

    // Bind the socket to an address
    struct sockaddr_in server_addr;
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
        log_error("Binding server address");

    // Start listening for clients
    while(1)
    {
        listen(server_fd, TOTAL_CLIENTS);

        // Connect to a client
        struct sockaddr cli_addr;
        socklen_t length = sizeof(cli_addr);
        int cli_fd = accept(server_fd, (struct sockaddr*) &cli_addr, &length);
        if(cli_fd < 0) log_error("Accepting client");

        // Talk to the client
        char buff_request[BUFFER_SIZE];
        bzero(buff_request, sizeof(buff_request));
        if(read(cli_fd, buff_request, BUFFER_SIZE))
        {
            // Analyse the request sent by client
            printf("Received: (%s)(%d)\n", buff_request, strlen(buff_request));

            // Respond to client

            char buff_response[BUFFER_SIZE];
            bzero(buff_response, sizeof(buff_response));

            fgets(buff_response, BUFFER_SIZE, stdin);

            if(write(cli_fd, buff_response, strlen(buff_response)) < 0 ) log_error("Replying to client");
        }
        else log_error("Talking to client");

        close(cli_fd);
    }

    return 0;
}

// Dump error message into the standard error location
void log_error(char* msg)
{
    perror(msg);
}
