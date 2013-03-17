/*
 *  Author: Francis Sowani
 *  Date  : 15 March 2013
 *
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

#define TRUE                 1
#define FALSE                0
#define MAX_TWEETS           10
#define HEADER_SIZE          6
#define MAX_REQUEST_ENTITIES 5
#define MAX_ENTITY_LENGTH    80
#define USER_DETAILS         4

// Server details
#define TOTAL_CLIENTS   50
#define PORT            8888
#define BUFFER_SIZE     8192
#define DATA_ROOT       "../tweeter-data/"
#define USERS_FILENAME  "users"
#define FOLLOW_EXT      ".follow"
#define TWEETS_EXT      ".tweets"

// Protocol constants
#define AUTHENTICATE_USER   "AUTUSR"
#define ADD_USER            "ADDUSR"
#define REMOVE_USER         "REMUSR"
#define FOLLOW_USER         "FOLUSR"
#define UNFOLOW_USER        "UNFUSR"
#define GET_USER            "GETUSR"
#define GET_USER_TWEETS     "GETTWT"
#define PUT_USER_TWEET      "PUTTWT"
#define GET_FOLLOWED_USERS  "GETFOL"
#define UNKNOWN_REQUEST     "UNKNOW"
#define SUCCESS             "YES"
#define FAILURE             "NO"
#define DELIMINATER         "~"

// Functions prototypes
void log_error(char*);
void authenticate_user(char*, char*);
void add_user(char*, char*);
void remove_user(char*, char*);
void follow_user(char*, char*);
void unfollow_user(char*, char*);
void get_user(char*, char*);
void get_user_tweets(char*, char*);
void put_user_tweet(char*, char*);
void get_followed_users(char*, char*);
void unknown_request(char*);
void explode(char*, char*, char[][MAX_ENTITY_LENGTH]);
void lower_case(char[]);

// Main function
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
        char request[BUFFER_SIZE];
        bzero(request, sizeof(request));
        if(read(cli_fd, request, BUFFER_SIZE))
        {
            // Analyse and respond to the request
            //printf("Received: (%s)(%d)\n", request, strlen(request));
            char header[HEADER_SIZE+1];
            bzero(header, sizeof(header));
            strncpy(header, request, HEADER_SIZE);
            char response[BUFFER_SIZE];
            bzero(response, sizeof(response));

            if(strcmp(header, AUTHENTICATE_USER) == 0)
                authenticate_user(request, response);
            else if( strcmp(header, ADD_USER) == 0)
                add_user(request, response);
            else if( strcmp(header, REMOVE_USER) == 0)
                remove_user(request, response);
            else if( strcmp(header, FOLLOW_USER) == 0)
                follow_user(request, response);
            else if( strcmp(header, UNFOLOW_USER) == 0)
                unfollow_user(request, response);
            else if( strcmp(header, GET_USER) == 0)
                get_user(request, response);
            else if( strcmp(header, GET_USER_TWEETS) == 0)
                get_user_tweets(request, response);
            else if( strcmp(header, PUT_USER_TWEET) == 0)
                put_user_tweet(request, response);
            else if( strcmp(header, GET_FOLLOWED_USERS) == 0)
                get_followed_users(request, response);
            else
                unknown_request(response);

            printf("REPLY: %s\n", response);

            if(write(cli_fd, response, strlen(response)) < 0 ) log_error("Replying to client");
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

// Check if user and password are valid
void  authenticate_user(char* request, char* response)
{
    int pos = 0;
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMINATER, data);
    if( data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        strcpy(file_name, DATA_ROOT);
        strcat(file_name, USERS_FILENAME);
        FILE* file = fopen(file_name, "r");

        if(file)
        {
            char line[MAX_ENTITY_LENGTH];
            char user[USER_DETAILS][MAX_ENTITY_LENGTH];
            fgets(line,MAX_ENTITY_LENGTH, file);

            while(!feof(file))
            {
                bzero(user, sizeof(user));
                explode(line, " ", user);
                if(user[0] != NULL && user[1] != NULL)
                {
                    if(strcmp(user[0], data[1]) == 0 &&
                            strcmp(user[1], data[2]) == 0)
                    {
                        strcpy(response, AUTHENTICATE_USER);
                        strcat(response, DELIMINATER);
                        strcat(response, SUCCESS);
                        return;
                    }
                }
                fgets(line,MAX_ENTITY_LENGTH, file);
            }
        }
    }
    strcpy(response, AUTHENTICATE_USER);
    strcat(response, DELIMINATER);
    strcat(response, FAILURE);
}

// Add new user if not existing
void  add_user(char* request, char* response)
{

}

// Remove user and his/her data from the records
void  remove_user(char* request, char* response)
{

}

// Follow the given user
void  follow_user(char* request, char* response)
{

}

// Remove user from the followed list
void  unfollow_user(char* request, char* response)
{

}

// Pull all tweets for the user
void  get_user(char* request, char* response)
{
    int pos = 0;
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMINATER, data);
    if( data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        strcpy(file_name, DATA_ROOT);
        strcat(file_name, USERS_FILENAME);
        FILE* file = fopen(file_name, "r");

        if(file)
        {
            char line[MAX_ENTITY_LENGTH];
            char user[USER_DETAILS][MAX_ENTITY_LENGTH];
            fgets(line,MAX_ENTITY_LENGTH, file);

            while(!feof(file))
            {
                bzero(user, sizeof(user));
                lower_case(line);
                explode(line, " ", user);
                if(user[0] != NULL && user[1] != NULL)
                {
                    if(strcmp(user[0], data[1]) == 0 ||
                            strcmp(user[2], data[1]) == 0 ||
                            strcmp(user[3], data[1]) == 0 )
                    {
                        strcpy(response, GET_USER);
                        strcat(response, DELIMINATER);
                        strcat(response, user[0]);
                        return;
                    }
                }
                fgets(line,MAX_ENTITY_LENGTH, file);
            }
        }
    }
    strcpy(response, GET_USER);
    strcat(response, DELIMINATER);
    strcat(response, FAILURE);
}

// Pull all tweets for the user
void  get_user_tweets(char* request, char* response)
{

}

// Add new tweet for the user
void  put_user_tweet(char* request, char* response)
{

}

// Retrieve all users followed by the user
void get_followed_users(char* request, char* response)
{

}

// Respond to the unknown request
void unknown_request(char* response)
{
    strcpy(response, UNKNOWN_REQUEST);
}

// Separate the request into individual entities
void explode(char* request, char* del, char data [MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH])
{
    char* req = strtok(request, del);
    int pos = 0;
    while(req != NULL)
    {
        strcpy(data[pos++], req);
        req = strtok(NULL, del);
    }
}

// Convert all characters to lower case
void lower_case(char line[MAX_ENTITY_LENGTH])
{
    int K = 0;
    while(K++ < MAX_ENTITY_LENGTH) line[K] = tolower(line[K]);
}
