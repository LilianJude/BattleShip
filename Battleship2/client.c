#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>



/*
 * Socket Read Functions
 */

/* Reads a message from the server socket. */
void recv_msg(int sockfd, char * msg)
{
    /* All messages are 3 bytes. */
    memset(msg, 0, 4);
    int n = recv(sockfd, msg, 3, 0);
    
    if (n < 0 || n != 3) /* Not what we were expecting. Server got killed or the other client disconnected. */ 
        perror("ERROR reading message from server socket.");

    #ifdef DEBUG
    printf("[DEBUG] Received message: %s\n", msg);
    #endif 
}

/* Reads an int from the server socket. */
int recv_int(int sockfd)
{
    int msg = 0;
    int n = recv(sockfd, &msg, sizeof(int), 0);
    
    if (n < 0 || n != sizeof(int)) 
        perror("ERROR reading int from server socket");
    
    #ifdef DEBUG
    printf("[DEBUG] Received int: %d\n", msg);
    #endif 
    
    return msg;
}

/*
 * Socket Write Functions
 */

/* Writes an int to the server socket. */
void send_server_int(int sockfd, int msg)
{
    int n = send(sockfd, &msg, sizeof(int), 0);
    if (n < 0)
        perror("ERROR writing int to server socket");
    
    #ifdef DEBUG
    printf("[DEBUG] Wrote int to server: %d\n", msg);
    #endif 
}

void send_server_square(int sockfd, char square[2])
{
    int n = send(sockfd, square, strlen(square), 0);
    if (n < 0)
        perror("ERROR writing square to client socket");
}

/*
 * Connect Functions
 */

/* Sets up the connection to the server. */
int connect_to_server(char * hostname, int portno)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;
 
    /* Get a socket. */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
    if (sockfd < 0) 
        perror("ERROR opening socket for server.");
	
    /* Get the address of the server. */
    server = gethostbyname(hostname);
	
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
	
	/* Zero out memory for server info. */
	memset(&serv_addr, 0, sizeof(serv_addr));

	/* Set up the server info. */
    serv_addr.sin_family = AF_INET;

    serv_addr.sin_port = htons(portno); 

	/* Make the connection. */
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        perror("ERROR connecting to server");

    #ifdef DEBUG
    printf("[DEBUG] Connected to server.\n");
    #endif 
    
    return sockfd;
}

/*
 * Game Functions
 */

/* Draws the game board to stdout. */
void draw_board(char board[][10])
{
    printf("   A | B | C | D | E | F | G | H | I | J | \n");
    printf("   -------------------------------------------\n");
    printf("1 | %c | %c | %c | %c | %c | %c | %c | %c | %c | %c | \n", board[0][0], board[0][1], board[0][2], board[0][3], board[0][4], board[0][5], board[0][6], board[0][7], board[0][8], board[0][9]);
    printf("   -------------------------------------------\n");
    printf("2 | %c | %c | %c | %c | %c | %c | %c | %c | %c | %c | \n", board[1][0], board[1][1], board[1][2], board[1][3], board[1][4], board[1][5], board[1][6], board[1][7], board[1][8], board[1][9]);
    printf("   -------------------------------------------\n");
    printf("3 | %c | %c | %c | %c | %c | %c | %c | %c | %c | %c | \n", board[2][0], board[2][1], board[2][2], board[2][3], board[2][4], board[2][5], board[2][6], board[2][7], board[2][8], board[2][9]);
    printf("   -------------------------------------------\n");
    printf("4 | %c | %c | %c | %c | %c | %c | %c | %c | %c | %c | \n", board[3][0], board[3][1], board[3][2], board[3][3], board[3][4], board[3][5], board[3][6], board[3][7], board[3][8], board[3][9]);
    printf("   -------------------------------------------\n");
    printf("5 | %c | %c | %c | %c | %c | %c | %c | %c | %c | %c | \n", board[4][0], board[4][1], board[4][2], board[4][3], board[4][4], board[4][5], board[4][6], board[4][7], board[4][8], board[4][9]);
    printf("   -------------------------------------------\n");
    printf("6 | %c | %c | %c | %c | %c | %c | %c | %c | %c | %c | \n", board[5][0], board[5][1], board[5][2], board[5][3], board[5][4], board[5][5], board[5][6], board[5][7], board[5][8], board[5][9]);
    printf("   -------------------------------------------\n");
    printf("7 | %c | %c | %c | %c | %c | %c | %c | %c | %c | %c | \n", board[6][0], board[6][1], board[6][2], board[6][3], board[6][4], board[6][5], board[6][6], board[6][7], board[6][8], board[6][9]);
    printf("   -------------------------------------------\n");
    printf("8 | %c | %c | %c | %c | %c | %c | %c | %c | %c | %c | \n", board[7][0], board[7][1], board[7][2], board[7][3], board[7][4], board[7][5], board[7][6], board[7][7], board[7][8], board[7][9]);
    printf("   -------------------------------------------\n");
    printf("9 | %c | %c | %c | %c | %c | %c | %c | %c | %c | %c | \n", board[8][0], board[8][1], board[8][2], board[8][3], board[8][4], board[8][5], board[8][6], board[8][7], board[8][8], board[8][9]);
    printf("   -------------------------------------------\n");
    printf("10| %c | %c | %c | %c | %c | %c | %c | %c | %c | %c | \n", board[9][0], board[9][1], board[9][2], board[9][3], board[9][4], board[9][5], board[9][6], board[9][7], board[9][8], board[9][9]);
}


void boat_placement(int sockfd)
{
    char col, lig;
    char line[20];
    char line2[20];
    
    while (1) { /* Ask until we receive. */ 
        printf("Placez votre porte-avion (4 cases) : \n");
	    printf("Entrez une colonne (A-I): ");
	    fgets(line, 20, stdin);
            printf("Entrez une ligne (0-9) :");
            fgets(line2, 20, stdin);
	    col = line[0];
	    lig = line2[0];
	    int moveLig = lig - '0';
        if (col>='A' && col<='I' && moveLig>=0 && moveLig<=9){
            printf("\n");
            char square[2];
            square[0]=col;
            square[1]=lig;
            /* Send players move to the server. */
            send_server_square(sockfd, square);
            break;
        } 
        else
            printf("\nInvalid input. Try again.\n");
    }
}


/* Get's the players turn and sends it to the server. */
void take_turn(int sockfd)
{
    char buffer[10];
    
    while (1) { /* Ask until we receive. */ 
        printf("A vous de jouer ! \n");
	printf("Entrez une colonne (A-I): ");
	    fgets(buffer, 10, stdin);
	    int move = buffer[0] - '0';
        if (move <= 9 && move >= 0){
            printf("\n");
            /* Send players move to the server. */
            send_server_int(sockfd, move);   
            break;
        } 
        else
            printf("\nInvalid input. Try again.\n");
    }
}

/* Gets a board update from the server. */
void get_update(int sockfd, char board[][10])
{
    /* Get the update. */
    int player_id = recv_int(sockfd);
    int move = recv_int(sockfd);

    /* Update the game board. */
    board[move/10][move%10] = player_id ? 'X' : 'O';    
}

/*
 * Main Program
 */

int main(int argc, char *argv[])
{
    /* Make sure host and port are specified. */
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    /* Connect to the server. */
    int sockfd = connect_to_server(argv[1], atoi(argv[2]));

    /* The client ID is the first thing we receive after connecting. */
    int id = recv_int(sockfd);

    #ifdef DEBUG
    printf("[DEBUG] Client ID: %d\n", id);
    #endif 

    char msg[4];
    char board[10][10] = { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, /* Game Board */ 
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			   {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                           {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, };

    printf("BattleShip\n------------\n");

    /* Wait for the game to start. */
    do {
        recv_msg(sockfd, msg);
        if (!strcmp(msg, "HLD"))
            printf("Waiting for a second player...\n");
    } while ( strcmp(msg, "SRT") );

    /* The game has begun. */
    printf("Game on!\n");
    printf("Your are %c's\n", id ? 'X' : 'O');

    draw_board(board);

    while(1) {
        recv_msg(sockfd, msg);

	if(!strcmp(msg, "PLT")) {
	    boat_placement(sockfd);
	}
	else if (!strcmp(msg, "TRN")) { /* Take a turn. */
	    printf("Your move...\n");
	    take_turn(sockfd);
        }
        else if (!strcmp(msg, "INV")) { /* Move was invalid. Note that a "TRN" message will always follow an "INV" message, so we will end up at the above case in the next iteration. */
            printf("That position has already been played. Try again.\n"); 
        }
        else if (!strcmp(msg, "CNT")) { /* Server is sending the number of active players. Note that a "TRN" message will always follow a "CNT" message. */
            int num_players = recv_int(sockfd);
            printf("There are currently %d active players.\n", num_players); 
        }
        else if (!strcmp(msg, "UPD")) { /* Server is sending a game board update. */
            get_update(sockfd, board);
            draw_board(board);
        }
        else if (!strcmp(msg, "WAT")) { /* Wait for other player to take a turn. */
            printf("Waiting for other players move...\n");
        }
        else if (!strcmp(msg, "WIN")) { /* Winner. */
            printf("You win!\n");
            break;
        }
        else if (!strcmp(msg, "LSE")) { /* Loser. */
            printf("You lost.\n");
            break;
        }
        else if (!strcmp(msg, "DRW")) { /* Game is a draw. */
            printf("Draw.\n");
            break;
        }
        else /* Weird... */
            perror("Unknown message.");
    }
    
    printf("Game over.\n");

    /* Close server socket and exit. */
    close(sockfd);
    return 0;
}
