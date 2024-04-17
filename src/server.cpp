#include <algorithm>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <ostream>
#include <regex>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT_NUM 4221
#define SEPARARTOR "=================================================="

std::string okMsg = "HTTP/1.1 200 OK\r\n\r\n";
std::string notFoundMsg = "HTTP/1.1 404 Not Found\r\n\r\n";
char requestBuffer[2048];

int main(int argc, char **argv) {
  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment this block to pass the first stage

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  // Since the tester restarts your program quite often, setting REUSE_PORT
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) <
      0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT_NUM);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
      0) {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }

  std::cout << "Success Binding To Port: " << PORT_NUM << "\n";

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  std::cout << "Waiting for a client to connect...\n";

  int client_socket = accept(server_fd, (struct sockaddr *)&client_addr,
                             (socklen_t *)&client_addr_len);

  std::string clientRequestFirstLine;
  if (recv(client_socket, requestBuffer, sizeof(requestBuffer), 0) > 0) {
    std::string clientRequestMessage = std::string(requestBuffer);
    clientRequestFirstLine =
        clientRequestMessage.substr(0, clientRequestMessage.find('\n'));

    std::cout << "Got request\n\n" << clientRequestMessage << "\n";
    std::cout << "First Line: " << clientRequestFirstLine << "\n";
  }

  // if clientRequestFirstLine not empty and
  // not match path after first slash
  std::regex firstPathQuery(R"(\s/\w\s*)");
  std::smatch res;
  std::regex_search(clientRequestFirstLine, res, firstPathQuery);
  std::cout << res[0];

  if (res[0].str().empty()) {
    send(client_socket, okMsg.c_str(), okMsg.size(), 0);
  } else {
    send(client_socket, notFoundMsg.c_str(), notFoundMsg.size(), 0);
  }

  close(server_fd);
  close(client_socket);

  return 0;
}
