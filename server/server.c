/*  Author: Francis Sowani
 *  Instr : Prof. John Sterling
 *  Course: Parallel and Distributed Systems
 *  Date  : 7 May 2013
 */

/*  IO, Types, System Calls Headers  */
#include<ctype.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>
#include<sys/stat.h>
#include<unistd.h>

/*  PThreads Headers   */
#include<pthread.h>

/*  Sockets  Headers   */
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>

/*  Server Details     */
#define TOTAL_CLIENTS   50
#define BUFFER_SIZE     8192
#define DATA_ROOT       "data/"
#define USERS_FILENAME  "users"
#define CONFIG_FILENAME "config_server"
#define FOLLOW_EXT      ".follow"
#define TWEETS_EXT      ".tweets"
#define FILE_MODE       0777

/*  Protocol Constants   */
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
#define DELIMITER           "~"

/*  Global Constants   */
#define TRUE                 1
#define FALSE                0
#define ERROR_FD             -1
#define MAX_TWEETS           10
#define HEADER_SIZE          6
#define MAX_REQUEST_ENTITIES 5
#define MAX_ENTITY_LENGTH    160
#define MAX_USER_DETAILS     4
#define VALID_MINIMUM_DATA   5

/*  File Locks and Conditional Variables         */
pthread_mutex_t mutx_users_file = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond_users_file = PTHREAD_COND_INITIALIZER;

/*  Functions prototypes  */
int   initialize_server(int);
void  run_server(int);
void* talk_to_client(void*);
void  broadcast_change(char*);
void  authenticate_user(char*, char*);
void  add_user(char*, char*);
void  remove_user(char*, char*);
void  follow_user(char*, char*);
void  unfollow_users(char*, char*);
void  get_user(char*, char*);
void  get_user_tweets(char*, char*);
void  put_user_tweet(char*, char*);
void  get_followed_users(char*, char*);
void  unknown_request(char*);
void  explode(char*, char*, char[][MAX_ENTITY_LENGTH]);
void  lower_case(char[]);
void  trim(char*);
void  log_error(char*);

/*  Address-Port type  */
typedef struct{char name[MAX_ENTITY_LENGTH]; int port;} ADDRESS;

/*  Main   */
int main(int argc, char* argv[])
{
    // Prepare errors log file
    //freopen("error_log.txt", "a", stderr);

    // Set up server's port number
    FILE* config_file = fopen(CONFIG_FILENAME, "r");
    if(config_file == NULL) log_error("Configuration file not found");

    int port, id, temp_id, temp_port;
    char temp_addr[MAX_ENTITY_LENGTH];
    fscanf(config_file, "id =%d", &id); // read the server id
    while(!feof(config_file))
    {
        fscanf(config_file, "%d%d", &temp_id, &temp_port);
        fgets(temp_addr, MAX_ENTITY_LENGTH, config_file);
        if(temp_id == id)
        {
            port = temp_port;
            break;
        }
    }

    // Configure and start server
    int fd = initialize_server(port);
    if(fd == ERROR_FD) return 1;

    run_server(fd);

    return 0;
}

/*  Create server objects and bind them together  */
int initialize_server(int port)
{
    // Create a server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) log_error("Creating server socket");

    // Bind the socket to an address
    struct sockaddr_in server_addr;
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        log_error("Binding server address");
        return ERROR_FD;
    }
    return server_fd;
}

/*  Start listening and serving clients  */
void run_server(int server_fd)
{
    while(TRUE)
    {
        listen(server_fd, TOTAL_CLIENTS);

        // Connect to a client
        struct sockaddr cli_addr;
        socklen_t length = sizeof(cli_addr);
        int cli_fd = accept(server_fd, (struct sockaddr*) &cli_addr, &length);
        if(cli_fd < 0) log_error("Accepting client");

        // Start a thread that will talk to the client
        pthread_t thread;
        pthread_create(&thread, NULL, talk_to_client, (void*) &cli_fd);
        pthread_join(thread, NULL);
    }
}

/*  Talk to a client: receive request, analyse it and respond     */
void* talk_to_client(void* fd)
{
    int* cli_fd = (int*) fd;    // Get client's socket descriptor

    char request[BUFFER_SIZE];
    bzero(request, sizeof(request));
    if(read(*cli_fd, request, BUFFER_SIZE))
    {
        // Analyse and respond to the request
        printf("REQUEST [%d](%s) ---> \n", (int) strlen(request), request);
        char header[HEADER_SIZE+1];
        bzero(header, sizeof(header));
        strncpy(header, request, HEADER_SIZE);
        char response[BUFFER_SIZE];
        bzero(response, sizeof(response));

        if(strcmp(header, AUTHENTICATE_USER) == 0)
            authenticate_user(request, response);
        else if(strcmp(header, ADD_USER) == 0)
            add_user(request, response);
        else if(strcmp(header, REMOVE_USER) == 0)
            remove_user(request, response);
        else if(strcmp(header, FOLLOW_USER) == 0)
            follow_user(request, response);
        else if(strcmp(header, UNFOLLOW_USERS) == 0)
            unfollow_users(request, response);
        else if(strcmp(header, GET_USER) == 0)
            get_user(request, response);
        else if(strcmp(header, GET_USER_TWEETS) == 0)
            get_user_tweets(request, response);
        else if(strcmp(header, PUT_USER_TWEET) == 0)
            put_user_tweet(request, response);
        else if(strcmp(header, GET_FOLLOWED_USERS) == 0)
            get_followed_users(request, response);
        else
            unknown_request(response);

        //printf("RESPONSE [%d](%s)\n\n", strlen(response), response);
        if(write(*cli_fd, response, strlen(response)) < 0 ) log_error("Replying to client");

        // Broadcast modifications to other servers
        if(strcmp(header, ADD_USER) == 0 || strcmp(header, REMOVE_USER) == 0 ||
           strcmp(header, FOLLOW_USER) == 0 || strcmp(header, UNFOLLOW_USERS) == 0 ||
           strcmp(header, PUT_USER_TWEET) == 0)
        {
            broadcast_change(request);
        }
    }
    else log_error("Talking to client");
    close(*cli_fd);
    return NULL;
}

/*  Send changes to other live servers   */
void broadcast_change(char* request)
{
    FILE* config_file = fopen(CONFIG_FILENAME, "r");
    if(config_file == NULL) log_error("Configuration file not found");

    int id, temp_id, temp_port;
    char temp_name[MAX_ENTITY_LENGTH];
    fscanf(config_file, "id =%d", &id); // read this server's id
    while(!feof(config_file))
    {
        fscanf(config_file, "%d%d", &temp_id, &temp_port);
        fgets(temp_name, MAX_ENTITY_LENGTH, config_file);

        if(temp_id > id)
        {
            int fd = socket(AF_INET, SOCK_STREAM, 0);
            if(fd < 0) log_error("Creating socket");

            // Connect to the replication server
            struct sockaddr_in server_addr;
            bzero((char*) &server_addr, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(temp_port);

            trim(temp_name);
            struct hostent *server = gethostbyname(temp_name);
            if(server == NULL) log_error("Replication server");

            bcopy((char*) server->h_addr, (char*) &server_addr.sin_addr.s_addr, server->h_length);

            if(connect(fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
                log_error("Connecting to replication server");

            // Send broadcast message to the replication server
            if(write(fd, request, strlen(request)) < 0)
                log_error("Writing to replication server");

            char buffer[BUFFER_SIZE];
            read(fd, buffer, BUFFER_SIZE);
            //printf("RES: [%s]\n", buffer);
            close(fd);
            break;  // Just to send to the next replication server
        }
    }
}

/*  Validate user and password pair   */
void  authenticate_user(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMITER, data);
    if(data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        sprintf(file_name, "%s%s", DATA_ROOT, USERS_FILENAME);
        FILE* file = fopen(file_name, "r");

        if(file)
        {
            char line[MAX_ENTITY_LENGTH];
            char user[MAX_USER_DETAILS][MAX_ENTITY_LENGTH];
            fgets(line,MAX_ENTITY_LENGTH, file);

            // Check all records of the users' file for the match
            while(!feof(file))
            {
                bzero(user, sizeof(user));
                explode(line, " ", user);
                if(user[0] != NULL && user[1] != NULL)
                {
                    if(strcmp(user[0], data[1]) == 0 &&
                            strcmp(user[1], data[2]) == 0)
                    {
                        sprintf(response, "%s%s%s", AUTHENTICATE_USER, DELIMITER, SUCCESS);
                        fclose(file);
                        return;
                    }
                }
                fgets(line,MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
        }
    }
    sprintf(response, "%s%s%s", AUTHENTICATE_USER, DELIMITER, FAILURE);
}

/*  Add new user if not existing    */
void  add_user(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMITER, data);

    if(data[0] != NULL && data[1] != NULL && data[2] != NULL &&
            data[3] != NULL && data[4] != NULL )
    {
        char file_name[MAX_ENTITY_LENGTH];
        sprintf(file_name, "%s%s", DATA_ROOT, USERS_FILENAME);

        while(pthread_mutex_trylock(&mutx_users_file))  // Try to lock the Users' file
            pthread_cond_wait(&cond_users_file, &mutx_users_file);
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
                        sprintf(response, "%s%s%s", ADD_USER, DELIMITER, FAILURE);
                        return;
                    }
                }
                fgets(line, MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
        }

        // Add the new user to the users file
        char record[MAX_ENTITY_LENGTH];
        sprintf(record, "%s %s %s %s\n", data[1], data[2], data[3], data[4]);
        file = fopen(file_name, "a");
        fputs(record, file);
        fclose(file);
        pthread_mutex_unlock(&mutx_users_file);   // Release the Users' file
        pthread_cond_broadcast(&cond_users_file); // Broadcast to the waiting threads

        // Create tweets and followed-users files for the new user
        sprintf(file_name, "%s%s%s", DATA_ROOT, data[1], FOLLOW_EXT);
        file = fopen(file_name, "w");
        fclose(file);
        chmod(file_name, FILE_MODE);

        sprintf(file_name, "%s%s%s", DATA_ROOT, data[1], TWEETS_EXT);
        file = fopen(file_name, "w");
        fclose(file);
        chmod(file_name, FILE_MODE);

        sprintf(file_name, "%s%s%s", ADD_USER, DELIMITER, SUCCESS);
        return;
    }
    sprintf(response, "%s%s%s", ADD_USER, DELIMITER, FAILURE);
}

/*  Remove user and his/her data from the records   */
void  remove_user(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    char new_data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    bzero(new_data, sizeof(new_data));
    explode(request, DELIMITER, data);
    int pos = 0;
    if(data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        sprintf(file_name, "%s%s", DATA_ROOT, USERS_FILENAME);

        while(pthread_mutex_trylock(&mutx_users_file))   // Try to lock the Users' file
            pthread_cond_wait(&cond_users_file, &mutx_users_file);
        FILE* file = fopen(file_name, "r");

        if(file)
        {
            // Get existing users
            char line[MAX_ENTITY_LENGTH];
            bzero(line, sizeof(line));
            fgets(line, MAX_ENTITY_LENGTH, file);
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
                        sprintf(file_name, "%s%s%s", DATA_ROOT, data[1], FOLLOW_EXT);
                        remove(file_name);

                        sprintf(file_name, "%s%s%s", DATA_ROOT, data[1], TWEETS_EXT);
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
        pthread_mutex_unlock(&mutx_users_file);   // Release Users' file
        pthread_cond_broadcast(&cond_users_file); // Broadcast to the waiting threads
    }
    sprintf(response, "%s%s%s", REMOVE_USER, DELIMITER, SUCCESS);
}

/*  Follow the given user   */
void  follow_user(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMITER, data);
    if(data[1] != NULL && data[2] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        sprintf(file_name, "%s%s%s", DATA_ROOT, data[1], FOLLOW_EXT);
        FILE* file = fopen(file_name, "a");

        if(file)
        {
            fputs(data[2], file);
            fputs("\n", file);
            fclose(file);
        }
    }
    sprintf(response, "%s%s%s", FOLLOW_USER, DELIMITER, SUCCESS);
}

/*  Remove user from the followed list  */
void  unfollow_users(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    char new_data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    bzero(new_data, sizeof(new_data));
    explode(request, DELIMITER, data);
    int pos = 0;
    if(data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        sprintf(file_name, "%s%s%s", DATA_ROOT, data[1], FOLLOW_EXT);
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
                        if(strlen(data[K]) < VALID_MINIMUM_DATA ||
                           strcmp(line, data[K]) == 0)  break;
                        else K++;
                    }
                    if(strlen(data[K]) < VALID_MINIMUM_DATA)
                        strcpy(new_data[pos++], line);
                }
                fgets(line,MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
        }

        // Write back the modified contents
        file = fopen(file_name, "w");
        int K = 0;
        while(K < pos)
        {
            fputs(new_data[K++], file);
            fputs("\n", file);
        }
        fclose(file);
    }
    sprintf(response, "%s%s%s", UNFOLLOW_USERS, DELIMITER, SUCCESS);
}

/*  Pull user detail (email)    */
void  get_user(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMITER, data);
    if( data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        sprintf(file_name, "%s%s", DATA_ROOT, USERS_FILENAME);
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
                        sprintf(response, "%s%s%s", GET_USER, DELIMITER, user[0]);
                        fclose(file);
                        return;
                    }
                }
                fgets(line,MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
        }
    }
    sprintf(response, "%s%s%s", GET_USER, DELIMITER, FAILURE);
}

/*  Pull all tweets for the user    */
void  get_user_tweets(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMITER, data);
    if(data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        sprintf(file_name, "%s%s%s", DATA_ROOT, data[1], TWEETS_EXT);
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
                strcat(response, DELIMITER);
                strcat(response, line);
                fgets(line,MAX_ENTITY_LENGTH, file);
                count++;
            }
            fclose(file);
        }
    }
}

/*  Add new tweet for the user  */
void  put_user_tweet(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMITER, data);
    if( data[0] != NULL && data[1] != NULL && data[2] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        sprintf(file_name, "%s%s%s", DATA_ROOT, data[1], TWEETS_EXT);
        FILE* file = fopen(file_name, "a");

        if(file)
        {
            fputs(data[2], file);
            fputs("\n", file);
            fclose(file);
            sprintf(response, "%s%s%s", PUT_USER_TWEET, DELIMITER, SUCCESS);
            return;
        }
    }
    sprintf(response, "%s%s%s", PUT_USER_TWEET, DELIMITER, FAILURE);
}

/*  Retrieve all users followed by the user     */
void get_followed_users(char* request, char* response)
{
    char data[MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH];
    bzero(data, sizeof(data));
    explode(request, DELIMITER, data);
    if( data[0] != NULL && data[1] != NULL)
    {
        char file_name[MAX_ENTITY_LENGTH];
        sprintf(file_name, "%s%s%s", DATA_ROOT, data[1], FOLLOW_EXT);
        FILE* file = fopen(file_name, "r");

        if(file)
        {
            char line[MAX_ENTITY_LENGTH];
            fgets(line,MAX_ENTITY_LENGTH, file);
            strcpy(response, GET_FOLLOWED_USERS);
            while(!feof(file))
            {
                trim(line);
                strcat(response, DELIMITER);
                strcat(response, line);
                fgets(line,MAX_ENTITY_LENGTH, file);
            }
            fclose(file);
            return;
        }
    }
    sprintf(response, "%s%s%s", GET_FOLLOWED_USERS, DELIMITER, FAILURE);
}

/*  Respond to the unknown request      */
void unknown_request(char* response)
{
    strcpy(response, UNKNOWN_REQUEST);
}

/*  Separate the request into individual entities   */
void explode(char* request, char* del, char data [MAX_REQUEST_ENTITIES][MAX_ENTITY_LENGTH])
{
    char line[MAX_ENTITY_LENGTH];
    strcpy(line, request);
    char* req = strtok(line, del);
    int pos = 0;
    while(req != NULL)
    {
        strcpy(data[pos++], req);
        req = strtok(NULL, del);
    }
}

/*  Convert all characters to lower case    */
void lower_case(char line[MAX_ENTITY_LENGTH])
{
    int K = 0;
    while(K++ < MAX_ENTITY_LENGTH) line[K] = tolower(line[K]);
}

/*  Remove whitespace from a string   */
void trim(char* str)
{
    int K = strlen(str) - 1;
    while(isspace(str[K])) str[K--] = '\0';
    K = 0; while(isspace(str[K]) || str[K] == ' ') K++;
    char temp[MAX_ENTITY_LENGTH];
    bzero(temp, sizeof(temp));
    strcpy(temp, str+K);
    strcpy(str, temp);
}

/*  Dump error message into the standard error location     */
void log_error(char* msg)
{
    perror(msg);
}
