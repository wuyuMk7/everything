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

int main(int argc, char* argv[])
{
  int tunfd, send_tunfd, server_sock, err;
  struct sockaddr_in client_addr;
  size_t client_len;

  SSL_METHOD *meth;
  SSL_CTX *ctx;
  //SSL *ssl;

  SSL_library_init();
  SSL_load_error_strings();
  SSLeay_add_ssl_algorithms();

  meth = (SSL_METHOD *)TLSv1_2_method();
  ctx = SSL_CTX_new(meth);
  SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
  SSL_CTX_use_certificate_file(ctx, "./server_cert/vpnlabserver-hecrt.pem", SSL_FILETYPE_PEM);
  SSL_CTX_use_PrivateKey_file(ctx, "./server_cert/vpnlabserver-hekey.pem", SSL_FILETYPE_PEM);
  //ssl = SSL_new(ctx);

  for (int i = 0;i < CLIENT_SIZE; ++i) {
    client_socks[i] = -1;
    pipe(client_fds[i]);
  }
  client_socks[0] = server_sock;
  client_socks[1] = server_sock;
  //client_socks[2] = server_sock;

  tunfd = createTunDevice("192.168.53.1", "255.255.255.0", 0);
  //send_tunfd = createTunDevice("192.168.54.2", "255.255.255.0", 0);
  server_sock = setupTCPServer();
  //////////////////////////////////
      printf("Main process\n");
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
	  printf("Got a packet from TUN\n");	
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
    int cur = 0;
    for (; cur < CLIENT_SIZE && client_socks[cur] != -1;++cur) ;
    if (cur >= CLIENT_SIZE) {
      close(client_sock);
      continue;
    }
    client_socks[cur] = client_sock;
    printf("Comming connection.");

    if (fork() == 0) {
      close(server_sock);
      close(client_fds[cur][1]);

      SSL *ssl = SSL_new(ctx);
      SSL_set_fd(ssl, client_sock);
      if ((err = SSL_accept(ssl)) < 1) {
	ERR_print_errors_fp(stderr);
	//perror("SSL accept error");
        client_socks[cur] = -1;
	exit(2);
      }

      printf("SSL connection established!\n");

      char buffer[BUFFER_SIZE], username[BUFFER_SIZE], password[BUFFER_SIZE];
      int read_len = 0, split_index = 0;
      memset(buffer, '\0', BUFFER_SIZE);
      memset(username, '\0', BUFFER_SIZE);
      memset(password, '\0', BUFFER_SIZE);
      
      read_len = SSL_read(ssl, buffer, BUFFER_SIZE - 1);
      printf("Received: %s\n", buffer);
      while(split_index < read_len && buffer[split_index] != ' ')
        ++ split_index;
      strncpy(username, buffer, split_index);
      strncpy(password, buffer + split_index, read_len - split_index);
      printf("username: %s\n", username);
      printf("password: %s\n", password + 1);
      if (checkpassword(username, password + 1) == 1) {
	printf("User authentication succeeded.");

	char ip[1024] = {'\0'};
        sprintf(ip, "192.168.53.%d", cur);
	SSL_write(ssl, ip, strlen(ip));

	// SSL VPN starts from this part.
	while(1) {
      	  fd_set readFDSet;

	  FD_ZERO(&readFDSet);
          //FD_SET(tunfd, &readFDSet);
	  FD_SET(client_fds[cur][0], &readFDSet);
	  FD_SET(client_sock, &readFDSet);
	  select(FD_SETSIZE, &readFDSet, NULL, NULL, NULL);

	  printf("waiting for data\n");

	/*
 	if (FD_ISSET(tunfd, &readFDSet)) {
	  printf("Got a packet from TUN\n");	
	  int main_recv_buffer_len = read(tunfd, buffer, BUFFER_SIZE);
	  printf("TUN:%d %s\n", main_recv_buffer_len, buffer);
	  SSL_write(ssl, buffer, main_recv_buffer_len);
	}
        */
 	  if (FD_ISSET(client_fds[cur][0], &readFDSet)) {
	    printf("Got a packet from Main Process\n");	
	    read_len = read(client_fds[cur][0], buffer, BUFFER_SIZE);
	    printf("FDREAD:%d %s\n", read_len, buffer);
	    SSL_write(ssl, buffer, read_len);
	  }

	  if (FD_ISSET(client_sock, &readFDSet)) {
	  //do {
      	    read_len = SSL_read(ssl, buffer, BUFFER_SIZE - 1);
	    buffer[read_len] = '\0';
	    printf("%d %s\n", read_len, buffer);
	  //} while(read_len > 0);
	    // TODO: Add a lock here or send after having got entire packet
	    write(tunfd, buffer, read_len);
	  }
	  //SSL_write(ssl, "hello", 5);
	  //printf("yo\n");
	}
      } else {
	// Print authentication failed/ with pid/ ip
 	printf("User authentication failed.");
	SSL_write(ssl, "failed", 6);
      }

      client_socks[cur] = -1;
      SSL_shutdown(ssl);
      SSL_free(ssl);
      close(client_sock);
      return 0;
    } 

 	}
      }


  ///////////////////////////////////

  //while(1) {
  //}

  return 0;
}
