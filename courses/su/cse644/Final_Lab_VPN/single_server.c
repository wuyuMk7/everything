#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <shadow.h>
#include <crypt.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/route.h>
#include <sys/ioctl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <errno.h>

#include "tun.h"
#include "myheader.h"

#define DEBUG 1

//#define CLIENT_SIZE 255
#define CLIENT_SIZE 3
#define SERVER_PORT 12605
#define BUFFER_SIZE 1024

#define SERVER_CONFIG "server_config"

int client_socks[CLIENT_SIZE];
int client_fds[CLIENT_SIZE][2];

char* extern_target_net = "";
char* extern_target_mask = "";

int setupTCPServer()
{
  struct sockaddr_in server;
  int sockfd, err; 

  sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY; 
  server.sin_port = htons(SERVER_PORT);

  int enable = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    perror("Setsockopt error");
    exit(1);
}

  if ((err = bind(sockfd, (struct sockaddr*)&server, sizeof(server))) < 0) {
    perror("Bind error");
    exit(-1);
  }

  if ((err = listen(sockfd, 10)) < 0) {
    perror("Listen error");
    exit(-1);
  }

  return sockfd;
}

int checkpassword(char *username, char *password)
{
  struct spwd *pw;
  char *epasswd; 

  pw = getspnam(username);
  if (pw == NULL)
    return -1;

  epasswd = crypt(password, pw->sp_pwdp);  
  if (strcmp(epasswd, pw->sp_pwdp))
    return -1;

  return 1;
}

void handleClientTransmission(SSL *ssl, int tunfd, int identifier) {
  char buffer[BUFFER_SIZE], 
       username[BUFFER_SIZE], 
       password[BUFFER_SIZE];
  int read_len = 0, split_index = 0;

  memset(buffer, '\0', BUFFER_SIZE);
  memset(username, '\0', BUFFER_SIZE);
  memset(password, '\0', BUFFER_SIZE);
  
  /* SSL tunnel initialization process */
  read_len = SSL_read(ssl, buffer, BUFFER_SIZE - 1);
#if DEBUG
  printf("Server(child): read %d bytes data from SSL tunnel. Client: %d.\n", read_len, identifier);
#endif
  /* Extract username and password from the incoming packet. */ 
  while(split_index < read_len && buffer[split_index] != ' ')
    ++ split_index;
  strncpy(username, buffer, split_index);
  strncpy(password, buffer + split_index, read_len - split_index);
#if DEBUG
  printf("Server(child): incomming user authentication credential: \n"
	"username - %s, password - %s\n", username, password + 1);
#endif
  if (checkpassword(username, password + 1) == 1) {
    printf("Server(child): user authentication succeeded. Client: %d, username: %s\n", identifier, username);
  
    /* Send IP back to the client */
    char ip[1024] = {'\0'};
    sprintf(ip, "192.168.53.%d", identifier);
    SSL_write(ssl, ip, strlen(ip));

    /* Input interfaces monitor */
    while(1) {
      fd_set readFDSet;

      FD_ZERO(&readFDSet);
      FD_SET(client_fds[identifier][0], &readFDSet);
      FD_SET(client_socks[identifier], &readFDSet);
      select(FD_SETSIZE, &readFDSet, NULL, NULL, NULL);

#if DEBUG
      printf("\nServer(child): waiting for data\n");
#endif

      if (FD_ISSET(client_fds[identifier][0], &readFDSet)) {
        read_len = read(client_fds[identifier][0], buffer, BUFFER_SIZE);
#if DEBUG
	printf("Server(child): got a packet from Main Process. Client: %d. Packet length %d bytes. \n", identifier, read_len);	
#endif
	SSL_write(ssl, buffer, read_len);
      }

      if (FD_ISSET(client_socks[identifier], &readFDSet)) {
        read_len = SSL_read(ssl, buffer, BUFFER_SIZE - 1);
        if (read_len > 0) {
          buffer[read_len] = '\0';
#if DEBUG
          printf("Server(child): got a packet from Client Socket. Client: %d. Packet length %d bytes\n", identifier, read_len);	
#endif
          write(tunfd, buffer, read_len);
        } else if (read_len == 0) {
#if DEBUG
          printf("Server(child): SSL socket disconnected. Client %d.\n", identifier);
#endif
          break;
        } else {
          perror("Server(child): SSL read error. \n");
          break;
        }
      }
    }
  } else {
    /* Print authentication failed/ with pid/ ip */
    printf("Server(child): user authentication failed. Client: %d, username: %s\n", identifier, username);
    SSL_write(ssl, "failed", 6);
  }

  return;
} 


int main(int argc, char* argv[])
{
  int tunfd, send_tunfd, server_sock, err;
  struct sockaddr_in client_addr;
  size_t client_len = 0;

  char certpath[1024] = {'\0'}, keypath[1024] = {'\0'};

  /* Load server configuration from SERVER_CONFIG */
  char config_key[1024], config_value[1024];
  FILE *fp;
  fp = fopen(SERVER_CONFIG, "r");

  /* Set parameters' default values */
  strncpy(certpath, "./server_cert/vpnlabserver-crt.pem", 34);
  strncpy(keypath, "./server_cert/vpnlabserver-key.pem", 34);
  while(fscanf(fp, "%s %s\n", config_key, config_value) == 2) {
    if (strncmp(config_key, "cert", 4) == 0) {
      strcpy(certpath, config_value);
    } else if (strncmp(config_key, "key", 3) == 0) {
      strcpy(keypath, config_value);
    }
  }

  SSL_METHOD *meth;
  SSL_CTX *ctx;

  /* SSL library initialization */
  SSL_library_init();
  SSL_load_error_strings();
  SSLeay_add_ssl_algorithms();

  meth = (SSL_METHOD *)TLSv1_2_method();
  ctx = SSL_CTX_new(meth);
  SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
  SSL_CTX_use_certificate_file(ctx, certpath, SSL_FILETYPE_PEM);
  SSL_CTX_use_PrivateKey_file(ctx, keypath, SSL_FILETYPE_PEM);

  /* Client socks and pipes initialization */
  for (int i = 0;i < CLIENT_SIZE; ++i) {
    client_socks[i] = -1;
    pipe(client_fds[i]);
  }
  client_socks[0] = server_sock;
  client_socks[1] = server_sock;

  /* Create tun device and listen socket */
  tunfd = createTunDevice("192.168.53.1", "255.255.255.0", 0);
  server_sock = setupTCPServer();

  /* Process transmission between client and server */
  struct ipheader *iph;
  char main_recv_buffer[BUFFER_SIZE];
  int main_recv_buffer_len = 0;

  /* Use select to monitor file descriptors */
  while (1) {
    fd_set readFDSet;

    FD_ZERO(&readFDSet);
    FD_SET(client_fds[0][0], &readFDSet);
    FD_SET(server_sock, &readFDSet);
    FD_SET(tunfd, &readFDSet);
    select(FD_SETSIZE, &readFDSet, NULL, NULL, NULL);

    if (FD_ISSET(client_fds[0][0], &readFDSet)) {
        main_recv_buffer_len = read(client_fds[0][0], main_recv_buffer, BUFFER_SIZE);
        int client_identifier = atoi(main_recv_buffer);
        if (client_identifier < 1 || client_identifier > 255) continue;
	printf("Server: got termination signal from client %d. Client disconnected.\n", client_identifier);	
        client_socks[client_identifier] = -1; 
    }

    if (FD_ISSET(tunfd, &readFDSet)) {
      main_recv_buffer_len = read(tunfd, main_recv_buffer, BUFFER_SIZE);
      iph = (struct ipheader*)(main_recv_buffer);
      unsigned char *source_addr = (unsigned char*)&(iph->iph_sourceip.s_addr);
      unsigned char *dest_addr = (unsigned char*)&(iph->iph_destip.s_addr);

#if DEBUG
      printf("Server: read %d bytes data from TUN device. " 
             "Packet from %d.%d.%d.%d to %d.%d.%d.%d\n", 
             main_recv_buffer_len, 
             source_addr[0], source_addr[1], source_addr[2], source_addr[3], 
             dest_addr[0], dest_addr[1], dest_addr[2], dest_addr[3]);
#else
      printf("Server: read %d bytes data from TUN device.\n", main_recv_buffer_len);
#endif

      //write(client_fds[dest_addr[3]][1], main_recv_buffer, main_recv_buffer_len);
      write(client_fds[2][1], main_recv_buffer, main_recv_buffer_len);
    }
  
    if (FD_ISSET(server_sock, &readFDSet)) {
      int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
      if (client_sock < 0) {
	perror("Server: accept client sock error");
	continue;
      }

      int cur = 0;
      for (; cur < CLIENT_SIZE && client_socks[cur] != -1;++cur) ;
      if (cur >= CLIENT_SIZE) {
        close(client_sock);
        continue;
      }
      client_socks[cur] = client_sock;

      if (fork() == 0) {
        close(server_sock);
        close(client_fds[cur][1]);

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_sock);
        if ((err = SSL_accept(ssl)) < 1) {
	  ERR_print_errors_fp(stderr);
          client_socks[cur] = -1;
	  exit(2);
        }
#if DEBUG
        printf("Server(child): SSL connection established! Client: %d.\n", cur);
#endif
 
        /* VPN packets process */
        handleClientTransmission(ssl, tunfd, cur);

        /* Notify the server to unlink this socket */
        char str_client_identifier[8] = {'\0'};
        sprintf(str_client_identifier, "%d", cur);
        write(client_fds[0][1], str_client_identifier, strlen(str_client_identifier));

        client_socks[cur] = -1;
        close(client_sock);
        SSL_shutdown(ssl);
        SSL_free(ssl);
        return 0;
      } else {
        close(client_sock);
      }
    }
  }
  return 0;
}
