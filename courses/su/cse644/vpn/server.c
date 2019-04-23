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

#define CLIENT_SIZE 255
#define SERVER_PORT 12605
#define BUFFER_SIZE 1024

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
  /* Accomplish user authentication here. */
  read_len = SSL_read(ssl, buffer, BUFFER_SIZE - 1);
#if DEBUG
  printf("Read %d bytes data from SSL tunnel.\n", read_len);
#endif
  /* Extract username and password from the incoming packet. */ 
  while(split_index < read_len && buffer[split_index] != ' ')
    ++ split_index;
  strncpy(username, buffer, split_index);
  strncpy(password, buffer + split_index, read_len - split_index);
#if DEBUG
  printf("Incomming user authentication credential: \n"
	"username - %s, password - %s\n", username, password + 1);
#endif
  if (checkpassword(username, password + 1) == 1) {
    printf("User authentication succeeded.");
  
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
      printf("Waiting for data\n");
#endif

      if (FD_ISSET(client_fds[identifier][0], &readFDSet)) {
        read_len = read(client_fds[identifier][0], buffer, BUFFER_SIZE);
#if DEBUG
	printf("Got a packet from Main Process. Packet length %d bytes\n", read_len);	
#endif
	SSL_write(ssl, buffer, read_len);
      }

      if (FD_ISSET(client_socks[identifier], &readFDSet)) {
        read_len = SSL_read(ssl, buffer, BUFFER_SIZE - 1);
        if (read_len > 0) {
          buffer[read_len] = '\0';
#if DEBUG
          printf("Got a packet from Client Socket. Packet length %d bytes\n", read_len);	
#endif
          write(tunfd, buffer, read_len);
        } else if (read_len == 0) {
#if DEBUG
          printf("SSL socket disconnected.\n");
#endif
          break;
        } else {
#if DEBUG
          perror("SSL read error");
#endif
          break;
        }
      }
    }
  } else {
    /* Print authentication failed/ with pid/ ip */
    printf("User authentication failed.");
    SSL_write(ssl, "failed", 6);
  }

  return;
} 


int main(int argc, char* argv[])
{
  int tunfd, send_tunfd, server_sock, err;
  struct sockaddr_in client_addr;
  size_t client_len = 0;

  SSL_METHOD *meth;
  SSL_CTX *ctx;

  /* SSL library initialization */
  SSL_library_init();
  SSL_load_error_strings();
  SSLeay_add_ssl_algorithms();

  meth = (SSL_METHOD *)TLSv1_2_method();
  ctx = SSL_CTX_new(meth);
  SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
  SSL_CTX_use_certificate_file(ctx, "./server_cert/vpnlabserver-hecrt.pem", SSL_FILETYPE_PEM);
  SSL_CTX_use_PrivateKey_file(ctx, "./server_cert/vpnlabserver-hekey.pem", SSL_FILETYPE_PEM);
  //ssl = SSL_new(ctx);

  /* Client socks and pipes initialization */
  for (int i = 0;i < CLIENT_SIZE; ++i) {
    client_socks[i] = -1;
    pipe(client_fds[i]);
  }
  client_socks[0] = server_sock;
  client_socks[1] = server_sock;
  //client_socks[2] = server_sock;

  /* Create tun device and listen socket */
  tunfd = createTunDevice("192.168.53.1", "255.255.255.0", 0);
  //send_tunfd = createTunDevice("192.168.54.2", "255.255.255.0", 0);
  server_sock = setupTCPServer();

  /* Process transmission between client and server */
  struct ipheader *iph;
  char main_recv_buffer[BUFFER_SIZE];
  int main_recv_buffer_len = 0;
  while (1) {
    fd_set readFDSet;

    FD_ZERO(&readFDSet);
    FD_SET(server_sock, &readFDSet);
    FD_SET(tunfd, &readFDSet);
    select(FD_SETSIZE, &readFDSet, NULL, NULL, NULL);

    if (FD_ISSET(tunfd, &readFDSet)) {
      printf("Got a packet from TUN device.\n");	
      int main_recv_buffer_len = read(tunfd, main_recv_buffer, BUFFER_SIZE);
      printf("TUN:%d %s\n", main_recv_buffer_len, main_recv_buffer);
      iph = (struct ipheader*)(main_recv_buffer);
      //unsigned char *source_addr = (unsigned char*)&(iph->iph_sourceip.s_addr);
      unsigned char *dest_addr = (unsigned char*)&(iph->iph_destip.s_addr);
      /*
      printf("Source IP: %d.%d.%d.%d\n", source_addr[0], source_addr[1], source_addr[2], source_addr[3]);
      printf("Dest IP: %d\n", dest_addr[3]);
      */
      write(client_fds[dest_addr[3]][1], main_recv_buffer, main_recv_buffer_len);
    }
  
    if (FD_ISSET(server_sock, &readFDSet)) {
      int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
      if (client_sock < 0) {
#if DEBUG
	perror("Accept client sock error");
#endif
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
        printf("SSL connection established!\n");
#endif
 
        /* VPN packets process */
        handleClientTransmission(ssl, tunfd, cur);

	// TODO: send a signal to parent and reset the globals
	// PIPE!
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
