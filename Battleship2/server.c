#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MYPORT 4321   // Port du point de connexion

int player_count = 0;
pthread_mutex_t mutexcount;

void error(const char *msg)
{
    perror(msg);
    pthread_exit(NULL);
}

/*
 * Socket Read Functions
 */

/* Reads an int from a client socket. */
int recv_int(int cli_sockfd)
{
    int msg = 0;
    int n = recv(cli_sockfd, &msg, sizeof(int), 0);

    if (n < 0 || n != sizeof(int)) /* Not what we were expecting. Client likely disconnected. */
        return -1;

    return msg;
}

/*
 * Socket Send Functions
 */

/* Sends a message to a client socket. */
void send_client_msg(int cli_sockfd, char * msg)
{
    int n = send(cli_sockfd, msg, strlen(msg), 0);
    if (n < 0)
        perror("ERROR writing msg to client socket");
}

/* Sends an int to a client socket. */
void send_client_int(int cli_sockfd, int msg)
{
    int n = send(cli_sockfd, &msg, sizeof(int), 0);
    if (n < 0)
        perror("ERROR writing int to client socket");
}

/* Sends a message to both client sockets. */
void send_clients_msg(int * cli_sockfd, char * msg)
{
    send_client_msg(cli_sockfd[0], msg);
    send_client_msg(cli_sockfd[1], msg);
}

/* Sends an int to both client sockets. */
void send_clients_int(int * cli_sockfd, int msg)
{
    send_client_int(cli_sockfd[0], msg);
    send_client_int(cli_sockfd[1], msg);
}


/* Cette fonction permet de se mettre en attente jusqu'à ce que deux clients se connectent sur le serveur de jeu pour ainsi créer une salle. */
void get_clients(int lis_sockfd, int * cli_sockfd)
{
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    /* On attend les deux clients */
    int num_conn = 0;
    while(num_conn < 2)
    {

        /* On écoute jusqu'à ce qu'un client se connecte */
		 if((listen(lis_sockfd, 253 - player_count)) < 0)
			 perror("ERROR: listen");

        clilen = sizeof(cli_addr);

	    /* On accepte la connexion du client. */

        cli_sockfd[num_conn] = accept(lis_sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (cli_sockfd[num_conn] < 0)
            perror("ERROR accepting a connection from a client.");

        /* On envoie au client son identifiant. */

        send(cli_sockfd[num_conn], &num_conn, sizeof(int), 0);

        /* On utilise un mutex afin d'incrémenter le nombre de joueurs connectés. */
        pthread_mutex_lock(&mutexcount);
        player_count++;
        printf("Il y a désormais %d joueurs connecté(s).\n", player_count);
        pthread_mutex_unlock(&mutexcount);

        if (num_conn == 0) {
            /* Send "HLD" to first client to let the user know the server is waiting on a second client. */
            send_client_msg(cli_sockfd[0],"HLD");
        }

        num_conn++;
    }
}

/*
 * Game Functions
 */

/* Gets a move from a client. */
int get_player_move(int cli_sockfd)
{

    /* Tell player to make a move. */
    send_client_msg(cli_sockfd, "TRN");

    /* Get players move. */
    return recv_int(cli_sockfd);
}

/* Checks that a players move is valid. */
int check_move(char board[][10], int move, int player_id)
{
    if ((move == 9) || (board[move/10][move%10] == ' ')) { /* Move is valid. */
        
        #ifdef DEBUG
        printf("[DEBUG] Player %d's move was valid.\n", player_id);
        #endif

        return 1;
   }
   else { /* Move is invalid. */
       return 0;
   }
}

/* Updates the board with a new move. */
void update_board(char board[][10], int move, int player_id)
{
    board[move/10][move%10] = player_id ? 'X' : 'O';
}

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

/* Sends a board update to both clients. */
void send_update(int * cli_sockfd, int move, int player_id)
{
    /* Signal an update */
    send_clients_msg(cli_sockfd, "UPD");

    /* Send the id of the player that made the move. */
    send_clients_int(cli_sockfd, player_id);

    /* Send the move. */
    send_clients_int(cli_sockfd, move);
}

/* Sends the number of active players to a client. */
void send_player_count(int cli_sockfd)
{
    send_client_msg(cli_sockfd, "CNT");
    send_client_int(cli_sockfd, player_count);
}

/* Checks the board to determine if there is a winner. */
int check_board(char board[][10], int last_move)
{
   
    int row = last_move/10;
    int col = last_move%10;


    if ( board[row][0] == board[row][1] && board[row][1] == board[row][2] ) { /* Check the row for a win. */

        return 1;
    }
    else if ( board[0][col] == board[1][col] && board[1][col] == board[2][col] ) { /* Check the column for a win. */

        return 1;
    }
    else if (!(last_move % 2)) { /* If the last move was at an even numbered position we have to check the diagonal(s) as well. */
        if ( (last_move == 0 || last_move == 4 || last_move == 8) && (board[1][1] == board[0][0] && board[1][1] == board[2][2]) ) {  /* Check backslash diagonal. */
            printf("Win by backslash diagonal.\n");
            return 1;
        }
        if ( (last_move == 2 || last_move == 4 || last_move == 6) && (board[1][1] == board[0][2] && board[1][1] == board[2][0]) ) { /* Check frontslash diagonal. */
            printf("Win by frontslash diagonal.\n");
            return 1;
        }
    }

    /* No winner, yet. */
    return 0;
}

/* Runs a game between two clients. */
void *run_game(void *thread_data)
{
    int *cli_sockfd = (int*)thread_data; /* Client sockets. */

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
    printf("Game on!\n");

    /* Send the start message. */
    send_clients_msg(cli_sockfd, "SRT");

    draw_board(board);

    int prev_player_turn = 1;
    int player_turn = 0;
    int game_over = 0;
    int turn_count = 0;
    while(!game_over) {
        /* Tell other player to wait, if necessary. */
        if (prev_player_turn != player_turn)
            send_client_msg(cli_sockfd[(player_turn + 1) % 2], "WAT");

        int valid = 0;
        int move = 0;
        while(!valid) { /* We need to keep asking for a move until the player's move is valid. */
            move = get_player_move(cli_sockfd[player_turn]);
            if (move == -1) break; /* Error reading client socket. */

            printf("Player %d played position %d\n", player_turn, move);

            valid = check_move(board, move, player_turn);
            if (!valid) { /* Move was invalid. */
                printf("Move was invalid. Let's try this again...\n");
                send_client_msg(cli_sockfd[player_turn], "INV");
            }
        }

	    if (move == -1) { /* Error reading from client. */
            printf("Player disconnected.\n");
            break;
        }
        else if (move == 9) { /* Send the client the number of active players. */
            prev_player_turn = player_turn;
            send_player_count(cli_sockfd[player_turn]);
        }
        else {
            /* Update the board and send the update. */
            update_board(board, move, player_turn);
            send_update( cli_sockfd, move, player_turn );

            /* Re-draw the board. */
            draw_board(board);

            /* Check for a winner/loser. */
            game_over = check_board(board, move);

            if (game_over == 1) { /* We have a winner. */
                send_client_msg(cli_sockfd[player_turn], "WIN");
                send_client_msg(cli_sockfd[(player_turn + 1) % 2], "LSE");
                printf("Player %d won.\n", player_turn);
            }
            else if (turn_count == 8) { /* There have been nine valid moves and no winner, game is a draw. */
                printf("Draw.\n");
                send_clients_msg(cli_sockfd, "DRW");
                game_over = 1;
            }

            /* Move to next player. */
            prev_player_turn = player_turn;
            player_turn = (player_turn + 1) % 2;
            turn_count++;
        }
    }

    printf("Game over.\n");

	/* Close client sockets and decrement player counter. */
    close(cli_sockfd[0]);
    close(cli_sockfd[1]);

    pthread_mutex_lock(&mutexcount);
    player_count--;
    printf("Number of players is now %d.", player_count);
    player_count--;
    printf("Number of players is now %d.", player_count);
    pthread_mutex_unlock(&mutexcount);

    free(cli_sockfd);

    pthread_exit(NULL);
}

/*
 * Main Program
 */

int main(int argc, char *argv[])
{

    int sockfd;
    struct sockaddr_in serv_addr;

    /* Donne une socket à écouter */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("ERROR opening listener socket.");

    /* Etablissement des informations du serveur */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(MYPORT);

    /* Donne les informations du serveur à la socket d'écoute. */
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        perror("ERROR binding listener socket.");

    int lis_sockfd = sockfd; /* Socket d'écoute. */

    pthread_mutex_init(&mutexcount, NULL);

    while (1) {
        if (player_count <= 252) { /* On lance une nouvelle partie seulement s'il y a des salles disponibles (c'est à dire moins de 252 joueurs en même temps). */
            int *cli_sockfd = (int*)malloc(2*sizeof(int)); /* Sockets des clients */

            /* On attend deux clients. */
            get_clients(lis_sockfd, cli_sockfd);

            pthread_t thread;

	    /* On lance une nouvelle partie. */
            int result = pthread_create(&thread, NULL, run_game, (void *)cli_sockfd);

            if (result){
                printf("Thread creation failed with return code %d\n", result);
                exit(-1);
            }

        }
    }

    close(lis_sockfd);

    pthread_mutex_destroy(&mutexcount);
    pthread_exit(NULL);
}
