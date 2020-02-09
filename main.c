#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdbool.h>

char getch(void)
{
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if(tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if(read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if(tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}

int game()
{
    system("clear");

    const int BOARD_X_SIZE = 3;
    const int BOARD_Y_SIZE = 3;

    const char PLAYER_1_CHARACTER = 'X';
    const char PLAYER_2_CHARACTER = 'O';

    // Game state variables
    char currentPlayer = 'X';
    char lastPlayer;

    char ch;
    char board[BOARD_X_SIZE][BOARD_Y_SIZE];
    
    int x, y;

    int turnCount = 1;
    
    int cursorX = 0;
    int cursorY = 0;

    bool illegalMove = false;
    bool turnDone = false;

    // Victory conditions
    bool victoryVertical = false;
    bool victoryHorizontal = false;
    bool victoryDiagonal = false;

    // For counting adjacent grids
    int counter;
    int emptySpace;

    char current_character;
    char checked_character;

    for (y = 0; y < BOARD_Y_SIZE; y++) {
        for (x = 0; x < BOARD_X_SIZE; x++) {
            board[x][y] = '.';
        }
    }

	while(1)
	{
        system("clear");

        // Check for vertical adjacents -- at least 2
        for (y = 0; y < BOARD_Y_SIZE; y++) {
            checked_character = '.';
            for (x = 0; x < BOARD_X_SIZE; x++) {
                current_character = board[y][x];
                if (current_character != checked_character) {
                    counter = 1;
                    checked_character = board[y][x];
                }
                else {
                    counter++;
                    if (current_character != '.' && counter == 3) {
                        victoryVertical = true;
                    }
                }
            }
        }

        // Check for horizontal adjacents -- at least 2
        for (x = 0; x < BOARD_X_SIZE; x++) {
            checked_character = '.';
            for (y = 0; y < BOARD_Y_SIZE; y++) {
                current_character = board[y][x];
                if (current_character != checked_character) {
                    counter = 1;
                    checked_character = board[y][x];
                }
                else {
                    counter++;
                    if (current_character != '.' && counter == 3) {
                        victoryHorizontal = true;
                    }
                }
            }
        }

        // Check if no empty spaces left in the board for draw condition
        emptySpace = 0;

        for (y = 0; y < BOARD_Y_SIZE; y++) {
            for (x = 0; x < BOARD_X_SIZE; x++) {
                if (board[y][x] == '.') {
                    emptySpace++;
                }
            }
        }

        // Prepare for the worst diagonal checking code to ever exist
        for (y = 0; y < BOARD_Y_SIZE; y++) {
            for (x = 0; x < BOARD_X_SIZE; x++) {
                checked_character = board[y][x];
                if (checked_character != '.' && ((x > 0 && x != BOARD_X_SIZE - 1) &&
                        (y > 0 && y != BOARD_Y_SIZE - 1))) {
                    if (board[y][x] == board[y+1][x+1] && 
                        board[y][x] == board[y-1][x-1]) {
                        victoryDiagonal = true;
                    }
                    else if (board[y][x] == board[y+1][x-1] && 
                            board[y][x] == board[y-1][x+1]) {
                        victoryDiagonal = true;
                    }
                }
            }
        }

        printf("\nC99 Tic-Tac-Toe by ArtisanLRO");
        printf("\n--------------------------------\n");

        // Prints the game board
        for (y = 0; y < BOARD_Y_SIZE; y++) {
            for (x = 0; x < BOARD_X_SIZE; x++) {
                if (!turnDone && cursorX == x && cursorY == y) {
                    // Print a cursor where it currently is
                    printf("█ ");
                }
                else {
                    // If not a cursor, print the grid value
                    printf("%c ", board[x][y]);
                }
            }
            printf("\n");
        }

        // Prints any victory messages
        if (victoryVertical) {
            printf("\nPlayer %c Victory: Vertical", lastPlayer);
            printf("\nGame End");
            printf("\n--------------------------------");
            getch();
            system("clear");
            return 0;
        }
        else if (victoryHorizontal) {
            printf("\nPlayer %c Victory: Horizontal", lastPlayer);
            printf("\nGame End");
            printf("\n--------------------------------");
            getch();
            system("clear");
            return 0;
        }
        else if (victoryDiagonal) {
            printf("\nPlayer %c Victory: Diagonal", lastPlayer);
            printf("\nGame End");
            printf("\n--------------------------------");
            getch();
            system("clear");
            return 0;
        }
        else if (emptySpace == 0) {
            printf("\nDraw: No Spaces Left");
            printf("\nGame End");
            printf("\n--------------------------------");
            getch();
            system("clear");
            return 0;
        }

        // If no victory or draw conditions, turn swap
        

        printf("\n--------------------------------");
        printf("\nTurn %d - Player %c", turnCount, currentPlayer);

        // Turn off this message next time
        if (illegalMove) {
            printf("\nMove Arrow Keys To Play - Turn Illegal");
            illegalMove = false;
        }
        else {
            printf("\nMove Arrow Keys To Play");
        }
        
        // Game continues
        if (turnDone) {
            turnDone = false;
        }

        // Keyhandler
        char ch = getch();

        // The most repetitive code you'll ever see
        switch(ch) {
            case 'A':
                if (cursorY == 0) {
                    cursorY = BOARD_Y_SIZE - 1;
                }
                else {
                    cursorY--;
                }
                break;
            case 'B':
                if (cursorY == BOARD_Y_SIZE - 1) {
                    cursorY = 0;
                }
                else {
                    cursorY++;
                }
                break;
            case 'C':
                if (cursorX == BOARD_X_SIZE - 1) {
                    cursorX = 0;
                }
                else {
                    cursorX++;
                }
                break;
            case 'D':
                if (cursorX == 0) {
                    cursorX = BOARD_X_SIZE - 1;
                }
                else {
                    cursorX--;
                }
                break;
            case '\n':
                if (board[cursorX][cursorY] != '.') {
                    illegalMove = true;
                }
                else {
                    board[cursorX][cursorY] = currentPlayer;

                    turnDone = true;
                    turnCount++;

                    lastPlayer = currentPlayer;

                    if (currentPlayer == PLAYER_1_CHARACTER) {
                        currentPlayer = PLAYER_2_CHARACTER;
                    }
                    else {
                        currentPlayer = PLAYER_1_CHARACTER;
                    }
                }
                break;
        }
	}
	return 0;
}

int main() {
    while(1) {
        game();
    }
}