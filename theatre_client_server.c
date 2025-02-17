#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_TICKETS 5

typedef struct {
    char movie_name[50];
    float price;
    char date[20];
} Ticket;

Ticket tickets[MAX_TICKETS] = {
    {"Film A", 10.0, "2025-01-05"},
    {"Film B", 12.0, "2025-01-06"},
    {"Film C", 8.0, "2025-01-07"},
    {"Film D", 15.0, "2025-01-08"},
    {"Film E", 9.0, "2025-01-09"}
};

void handle_client(int new_socket) {
    char buffer[1024] = {0};
    char response[1024];
    int ticket_choice;

    // Stampa la lista dei biglietti disponibili
    for (int i = 0; i < MAX_TICKETS; i++) {
        if (strlen(tickets[i].movie_name) > 0) {
            sprintf(response, "Biglietto %d: %s - $%.2f - Data: %s\n", i + 1, tickets[i].movie_name, tickets[i].price, tickets[i].date);
            send(new_socket, response, strlen(response), 0);
        }
    }

    // Ricevi la scelta del biglietto
    read(new_socket, buffer, 1024);
    ticket_choice = atoi(buffer) - 1;

    // Rimuove il biglietto scelto dai biglietti disponibili
    if (ticket_choice >= 0 && ticket_choice < MAX_TICKETS && strlen(tickets[ticket_choice].movie_name) > 0) {
        sprintf(response, "Hai comprato un Biglietto per %s\n", tickets[ticket_choice].movie_name);
        send(new_socket, response, strlen(response), 0);
        strcpy(tickets[ticket_choice].movie_name, "");
    } else {
        sprintf(response, "Scelta non valida\n");
        send(new_socket, response, strlen(response), 0);
    }

    close(new_socket);
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("la socket non è riuscita");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("il collegamento non è riuscito");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("ascolto");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accetta");
            exit(EXIT_FAILURE);
        }
        handle_client(new_socket);
    }

    return 0;
}
