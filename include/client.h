#define HEADER_MODE 1

int establish(char *host, int port, char *header);
long read_response(int sockfd, char *response, char *boundary, size_t length);
void close_socket(int sockfd);
