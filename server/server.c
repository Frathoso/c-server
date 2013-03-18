/*  Author: Francis Sowani
 *  Instr : Prof. John Sterling
 *  Course: Parallel and Distributed Systems
 *  Date  : 15 March 2013
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
#define MAX_ENTITY_LENGTH    160
#define MAX_USER_DETAILS     4
#define VALID_MINIMUM_DATA   5

// Server details
#define TOTAL_CLIENTS   50
#define DEFAULT_PORT    8888
#define BUFFER_SIZE     8192
#define DATA_ROOT       "data/"
#define USERS_FILENAME  "users"
#define FOLLOW_EXT      ".follow"
#define TWEETS_EXT      ".tweets"
#define FILE_MODE       0777

// Protocol constants
#define AUTHENTICATE_USER   "AUTUSR"
#define ADD_USER            "ADDUSR"
#define REMOVE_USER         "REMUSR"
#define FOLLOW_USER         "FOLUSR"
#define UNFOLLOW_USERS      "UNFUSR"
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
void unfollow_users(char*, char*);
void get_user(char*, char*);
void get_user_tweets(char*, char*);
void put_user_tweet(char*, char*);
void get_followed_users(char*, char*);
void unknown_request(char*);
void explode(char*, char*, char[][MAX_ENTITY_LENGTH]);
void lower_case(char[]);
void trim(char*);

// Main function
int main(int argc, char* argv[])
{
    // Setting up server's port number
    int PORT;
    if( argc == 2 )
        PORT = atoi(argv[1]);
    else if( argc > 2 )
    {
        log_error("Wrong number of arguments!\nUsage: ./server [port no]\n");
        exit(1);
    }
    else
        PORT = DEFAULT_PORT;

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
    while(TRUE)
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
            //printf("REQUEST [%d](%s) ---> ", strlen(request), request);
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
            else if( strcmp(header, UNFOLLOW_USERS) == 0)
                unfollow_users(request, response);
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

            //printf("RESPONSE [%d](%s)\n\n", strlen(response), response);

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
            char user[MAX_USER_DETAILS][MAX_ENTITY_LENGTH];
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
                        fclose(file);
                        return;
                    }
                }
                fgets(line,MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
        }
    }
    strcpy(response, AUTHENTICATE_USER);
    strcat(response, DELIMINATER);
    strcat(response, FAILURE);
}

// Add new user if not existing
void  add_user(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMINATER, data);

    if(data[0] != NULL && data[1] != NULL && data[2] != NULL &&
            data[3] != NULL && data[4] != NULL )
    {
        char file_name[MAX_ENTITY_LENGTH];
        strcpy(file_name, DATA_ROOT);
        strcat(file_name, USERS_FILENAME);
        FILE* file = fopen(file_name, "r");
        if(file)
        {
            // Get existing users
            char line[MAX_ENTITY_LENGTH];
            bzero(line, sizeof(line));
            fgets(line,MAX_ENTITY_LENGTH, file);
            while(!feof(file))
            {
                if(strlen(line) > VALID_MINIMUM_DATA)
                {
                    char user[MAX_USER_DETAILS][MAX_ENTITY_LENGTH];
                    bzero(user, sizeof(user));
                    explode(line, " ", user);
                    if(strcmp(data[1], user[0]) == 0)
                    {
                        // Exists a user with the same email
                        strcpy(response, ADD_USER);
                        strcat(response, DELIMINATER);
                        strcat(response, FAILURE);
                        return;
                    }
                }
                fgets(line, MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
        }

        // Add the new user to the users file
        char record[MAX_ENTITY_LENGTH];
        bzero(record, sizeof(record));
        strcpy(record, data[1]);
        strcat(record, " ");
        strcat(record, data[2]);
        strcat(record, " ");
        strcat(record, data[3]);
        strcat(record, " ");
        strcat(record, data[4]);
        strcat(record, "\n");

        file = fopen(file_name, "a");
        fputs(record, file);
        fclose(file);

        // Create tweets and followed-users files for the new user
        strcpy(file_name, DATA_ROOT);
        strcat(file_name, data[1]);
        strcat(file_name, FOLLOW_EXT);
        file = fopen(file_name, "w");
        fclose(file);
        chmod(file_name, FILE_MODE);

        strcpy(file_name, DATA_ROOT);
        strcat(file_name, data[1]);
        strcat(file_name, TWEETS_EXT);
        file = fopen(file_name, "w");
        fclose(file);
        chmod(file_name, FILE_MODE);

        strcpy(response, ADD_USER);
        strcat(response, DELIMINATER);
        strcat(response, SUCCESS);
        return;
    }

    strcpy(response, ADD_USER);
    strcat(response, DELIMINATER);
    strcat(response, FAILURE);
}

// Remove user and his/her data from the records
void  remove_user(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    char new_data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    bzero(new_data, sizeof(new_data));
    explode(request, DELIMINATER, data);
    int pos = 0;
    if( data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        strcpy(file_name, DATA_ROOT);
        strcat(file_name, USERS_FILENAME);
        FILE* file = fopen(file_name, "r");

        if(file)
        {
            // Get existing users
            char line[MAX_ENTITY_LENGTH];
            bzero(line, sizeof(line));
            fgets(line,MAX_ENTITY_LENGTH, file);
            while(!feof(file))
            {
                if(strlen(line) > VALID_MINIMUM_DATA)
                {
                    char user[MAX_USER_DETAILS][MAX_ENTITY_LENGTH];
                    bzero(user, sizeof(user));
                    explode(line, " ", user);
                    if(strcmp(data[1], user[0]) == 0)
                    {
                        // Remove tweets and followed people files
                        char file_name[MAX_ENTITY_LENGTH];
                        strcpy(file_name, DATA_ROOT);
                        strcat(file_name, data[1]);
                        strcat(file_name, FOLLOW_EXT);
                        remove(file_name);

                        strcpy(file_name, DATA_ROOT);
                        strcat(file_name, data[1]);
                        strcat(file_name, TWEETS_EXT);
                        remove(file_name);
                    }
                    else
                    {
                        // Keep all details for the user
                        strcpy(new_data[pos++], line);
                    }
                }
                fgets(line, MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
        }

        file = fopen(file_name, "w");
        int K = 0;
        while(K < pos)  fputs(new_data[K++], file);
        fclose(file);
    }
    strcpy(response, REMOVE_USER);
    strcat(response, DELIMINATER);
    strcat(response, SUCCESS);
}

// Follow the given user
void  follow_user(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMINATER, data);
    if( data[1] != NULL && data[2] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        strcpy(file_name, DATA_ROOT);
        strcat(file_name, data[1]);
        strcat(file_name, FOLLOW_EXT);
        FILE* file = fopen(file_name, "a");

        if(file)
        {
            fputs(data[2], file);
            fputs("\n", file);
            fclose(file);
        }
    }
    strcpy(response, FOLLOW_USER);
    strcat(response, DELIMINATER);
    strcat(response, SUCCESS);
}

// Remove user from the followed list
void  unfollow_users(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    char new_data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    bzero(new_data, sizeof(new_data));
    explode(request, DELIMINATER, data);
    int pos = 0;
    if( data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        strcpy(file_name, DATA_ROOT);
        strcat(file_name, data[1]);
        strcat(file_name, FOLLOW_EXT);
        FILE* file = fopen(file_name, "r");

        if(file)
        {
            // Read old contents and remove the submitted users
            char line[MAX_ENTITY_LENGTH];
            bzero(line, sizeof(line));
            fgets(line,MAX_ENTITY_LENGTH, file);
            while(!feof(file))
            {
                trim(line);
                if(strlen(line) > VALID_MINIMUM_DATA)
                {
                    int K = 2;
                    while(TRUE)
                    {
                        if(strlen(data[K]) < VALID_MINIMUM_DATA) break;
                        if(strcmp(line, data[K++]) != 0)
                        {
                            strcpy(new_data[pos++], line);
                            break;
                        }
                    }
                }
                fgets(line,MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
        }

        file = fopen(file_name, "w");
        int K=0;
        while(K < pos)
        {
            fputs(new_data[K++], file);
            fputs("\n", file);
        }
        fclose(file);
    }
    strcpy(response, UNFOLLOW_USERS);
    strcat(response, DELIMINATER);
    strcat(response, SUCCESS);
}

// Pull user detail (email)
void  get_user(char* request, char* response)
{
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
            char user[MAX_USER_DETAILS][MAX_ENTITY_LENGTH];
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
                        fclose(file);
                        return;
                    }
                }
                fgets(line,MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
        }
    }
    strcpy(response, GET_USER);
    strcat(response, DELIMINATER);
    strcat(response, FAILURE);
}

// Pull all tweets for the user
void  get_user_tweets(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMINATER, data);
    if( data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        strcpy(file_name, DATA_ROOT);
        strcat(file_name, data[1]);
        strcat(file_name, TWEETS_EXT);
        FILE* file = fopen(file_name, "r");

        if(file)
        {
            char line[MAX_ENTITY_LENGTH];
            fgets(line,MAX_ENTITY_LENGTH, file);
            strcpy(response, GET_USER_TWEETS);
            int count = 0;
            while(!feof(file) && count < MAX_TWEETS)
            {
                trim(line);
                strcat(response, DELIMINATER);
                strcat(response, line);
                fgets(line,MAX_ENTITY_LENGTH, file);
                count++;
            }
            fclose(file);
        }
    }
}

// Add new tweet for the user
void  put_user_tweet(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMINATER, data);
    if( data[0] != NULL && data[1] != NULL && data[2] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        strcpy(file_name, DATA_ROOT);
        strcat(file_name, data[1]);
        strcat(file_name, TWEETS_EXT);
        FILE* file = fopen(file_name, "a");

        if(file)
        {
            fputs(data[2], file);
            fputs("\n", file);
            fclose(file);
            strcpy(response, PUT_USER_TWEET);
            strcat(response, DELIMINATER);
            strcat(response, SUCCESS);
            return;
        }
    }
    strcpy(response, PUT_USER_TWEET);
    strcat(response, DELIMINATER);
    strcat(response, FAILURE);
}

// Retrieve all users followed by the user
void get_followed_users(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMINATER, data);
    if( data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        strcpy(file_name, DATA_ROOT);
        strcat(file_name, data[1]);
        strcat(file_name, FOLLOW_EXT);
        FILE* file = fopen(file_name, "r");

        if(file)
        {
            char line[MAX_ENTITY_LENGTH];
            fgets(line,MAX_ENTITY_LENGTH, file);
            strcpy(response, GET_FOLLOWED_USERS);
            while(!feof(file))
            {
                trim(line);
                strcat(response, DELIMINATER);
                strcat(response, line);
                fgets(line,MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
            return;
        }
    }
    strcpy(response, GET_FOLLOWED_USERS);
    strcat(response, DELIMINATER);
    strcat(response, FAILURE);
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

// Remove whitespace from string
void trim(char* str)
{
    int K = strlen(str) - 1;
    while(isspace(str[K])) str[K--] = '\0';
}
