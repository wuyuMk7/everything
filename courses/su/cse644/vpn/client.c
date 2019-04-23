#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "tun.h"

#define BUFFER_SIZE 1024
#define CA_DIR "cert_client"

char* extern_target_net = "192.168.60.0";
char* extern_target_mask = "255.255.255.0";

int setupTCPClient(const char *host, int port) 
{
  struct sockaddr_in server_addr;
  struct hostent *hp = gethostbyname(host);
  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  
  memset(&server_addr, '\0', sizeof(server_addr));
  memcpy(&(server_addr.sin_addr.s_addr), hp->h_addr, hp->h_length);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

  return sockfd;
}

int verify_callback(int ok, X509_STORE_CTX *x509_ctx)
{
  char buf[300];

  X509 *cert = X509_STORE_CTX_get_current_cert(x509_ctx);
  X509_NAME_oneline(X509_get_subject_name(cert), buf, 300);
  printf("subject= %s\n", buf);

  if (ok == 1) {
    printf("Certificate verification passed.\n"); 
  } else {
    printf("Certificate verification failed: %s.\n", 
	X509_verify_cert_error_string(X509_STORE_CTX_get_error(x509_ctx)));
  }

  return ok;
}

SSL* setupTLSClient(const char *hostname)
{
  SSL_library_init();
  SSL_load_error_strings();
  SSLeay_add_ssl_algorithms();

  SSL_METHOD *meth = (SSL_METHOD *)TLSv1_2_method();
  SSL_CTX *ctx = SSL_CTX_new(meth);
  SSL *ssl;

  SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
  if (SSL_CTX_load_verify_locations(ctx, NULL, CA_DIR) < 1) {
    printf("Error setting the verify locations. \n");
    exit(0);
  }
  ssl = SSL_new(ctx);
  
  X509_VERIFY_PARAM *vpm = SSL_get0_param(ssl);
  X509_VERIFY_PARAM_set1_host(vpm, hostname, 0);

  return ssl;
}

int main(int argc, char* argv[])
{
  int port = 12605;
  char *hostname = "vpnlabserver-he.com";
  char *username = "seed", *password = "dees";

  SSL *ssl = setupTLSClient(hostname);
  int tunfd, sockfd = setupTCPClient(hostname, port);

  SSL_set_fd(ssl, sockfd);
  if ((SSL_connect(ssl)) < 1) {
    ERR_print_errors_fp(stderr);
    exit(2);
  }

  printf("SSL connection is successful.\n");
  printf("SSL connection using %s\n", SSL_get_cipher(ssl));

  int read_len = 0;
  char recv_buffer[BUFFER_SIZE] = {'\0'}, send_buffer[BUFFER_SIZE] = {'\0'};
  char local_tun_ip[BUFFER_SIZE] = {'\0'};

  // Send username and password, then receive IP address
  sprintf(send_buffer, "%s %s", username, password);
  SSL_write(ssl, send_buffer, strlen(send_buffer));
  read_len = SSL_read(ssl, recv_buffer, BUFFER_SIZE - 1); 
  printf("Received an IP from the VPN server: %s\n", recv_buffer);
  if (read_len == 6 && strncmp(recv_buffer, "failed", 6) == 0) {
    close(sockfd);
    printf("Authentication failed. VPN connection terminated.\n");
    exit(0);
  } else {
    strncpy(local_tun_ip, recv_buffer, read_len);
    tunfd = createTunDevice(local_tun_ip, "255.255.255.0", 1);
    while(1) {
      fd_set readFDSet;

      FD_ZERO(&readFDSet);
      FD_SET(sockfd, &readFDSet);
      FD_SET(tunfd, &readFDSet);
      select(FD_SETSIZE, &readFDSet, NULL, NULL, NULL);

      if (FD_ISSET(tunfd, &readFDSet)) {
	printf("Got a packet from tun\n");
        printf("tunbe %d %s\n", read_len, send_buffer);
        read_len = read(tunfd, send_buffer, BUFFER_SIZE - 1);
        printf("tun %d %s\n", read_len, send_buffer);
        send_buffer[read_len] = '\0';
        SSL_write(ssl, send_buffer, read_len);
      }
    
      if (FD_ISSET(sockfd, &readFDSet)) {
        read_len = SSL_read(ssl, recv_buffer, BUFFER_SIZE - 1);
	if (read_len > 0) {
          recv_buffer[read_len] = '\0';
          write(tunfd, recv_buffer, read_len);
          printf("vpn %d\n", read_len);
	} else if (read_len == 0) {
	  printf("Server disconnected.\n");
          break;
	} else {
          printf("SSL read error. Program terminated.\n");
          break;
        }
      }
    }
  }
}
