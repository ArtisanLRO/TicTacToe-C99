/*
A horrendously simplistic Tic-Tac-Toe written in ANSI C for use in Linux terminals
by ArtisanLRO
@MIT License
*/
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdbool.h>

#define BOARD_SIZE 3

// Keyhandler logic.
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

// Set the current item in a given board position.
void setGridItem(char* board, int x, int y, char item) {
    *(board + y * BOARD_SIZE + x) = item; 
}

// Show the current item in a given board position.
char getGridItem(char* board, int x, int y) {
    return *(board + y * BOARD_SIZE + x);
}

// Display the game board in the terminal.
void renderBoard(char* board, int cursorX, int cursorY, bool cursorHidden) {
    int x, y;
    
    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {\
            if (!cursorHidden && cursorX == x && cursorY == y) {
                // If the player cursor is here, render the cursor.
                printf("█ ");
            }
            
            else {
                // If not, show the contents according to pointer offset.
                printf("%c ", getGridItem(board, x,y)); 
            }
            
        }
        printf("\n");
    }
}

// Check for adjacent verticals.
bool victoryVertical(char* board) {
    int x, y;
    
    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {
            if (y > 0 && y != BOARD_SIZE-1) {
                if (getGridItem(board, x, y) != '.' &&
                        getGridItem(board, x, y) == getGridItem(board, x, y+1) && 
                        getGridItem(board, x, y) == getGridItem(board, x, y-1)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

// Check for adjacent horizontals.
bool victoryHorizontal(char* board) {
    int x, y;

    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {
            if (x > 0 && x != BOARD_SIZE-1) {
                if (getGridItem(board, x, y) != '.' &&
                        getGridItem(board, x, y) == getGridItem(board, x+1, y) && 
                        getGridItem(board, x, y) == getGridItem(board, x-1, y)) {
                    return 1;
                }
            }
        }
    }
    
    return 0;
}

// Check for adjacent diagonals.
bool victoryDiagonal(char* board) {
    int x, y, counter;
    char charBeingChecked;
    
    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {
                charBeingChecked = getGridItem(board, x, y);
            if (charBeingChecked != '.' && 
                    (x > 0 && x != BOARD_SIZE-1) && y > 0 && y != BOARD_SIZE-1) {
                if (getGridItem(board, x, y) == getGridItem(board, x+1, y+1) && 
                        getGridItem(board, x, y) == getGridItem(board, x-1, y-1)) {
                    return 1;
                }
                if (getGridItem(board, x, y) == getGridItem(board, x+1, y-1) && 
                        getGridItem(board, x, y) == getGridItem(board, x-1, y+1)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

bool victoryImpossible(char* board) {
    int x, y;

    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {
            if (getGridItem(board, x, y) == '.') {
                return 0;
            }
        }
    }

    return 1;
}

// Prints any victory messages.
bool victoryHandler(char* board, char lastPlayer) {
    
    if (victoryVertical(board)) {
        printf("\nPlayer %c Victory: Vertical", lastPlayer);
        printf("\nGame End");
        printf("\n--------------------------------");
        getch();
        system("clear");
        return 1;
    }
    else if (victoryHorizontal(board)) {
        printf("\nPlayer %c Victory: Horizontal", lastPlayer);
        printf("\nGame End");
        printf("\n--------------------------------");
        getch();
        system("clear");
        return 1;
    }
    else if (victoryDiagonal(board)) {
        printf("\nPlayer %c Victory: Diagonal", lastPlayer);
        printf("\nGame End");
        printf("\n--------------------------------");
        getch();
        system("clear");
        return 1;
    }
    else if (victoryImpossible(board)) {
        printf("\nDraw: No Spaces Left");
        printf("\nGame End");
        printf("\n--------------------------------");
        getch();
        system("clear");
        return 1;
    }
    return 0;
}

// Initialises game logic, then loops till victory or draw condition.
int game() {

    // Player symbols.
    const char PLAYER_1_CHARACTER = 'X';
    const char PLAYER_2_CHARACTER = 'O';

    // Necessary to record the current player's turn.
    char currentPlayer = 'X';
    char lastPlayer;

    // The game starts on the first turn.
    int turnCount = 1;

    // Initialise cursor position.
    int cursorX, cursorY = 0;

    // Status message helpers.
    bool illegalMove, cursorHidden = false;

    // Dynamically allocates memory for the square grid of the game board.
    char *board = (char *)malloc(BOARD_SIZE * BOARD_SIZE * sizeof(char));

    // Initialise the game board with blank space.
    int x, y; 
    for (y = 0; y < BOARD_SIZE; y++) {
        for (x = 0; x < BOARD_SIZE; x++) {
            setGridItem(board, x, y, '.');
        }
    }     

    // The game loop.
	while(1) {

        // Clear the terminal.
        system("clear");

        // Print the header.
        printf("\nC99 Tic-Tac-Toe by ArtisanLRO");
        printf("\n--------------------------------\n");
        
        // Render the game board.
        renderBoard(board, cursorX, cursorY, cursorHidden);

        // Listen for game end conditions, end the game if any are true.
        if (victoryHandler(board, lastPlayer)) {
            return 1;
        }

        // Print game details.    
        printf("\n--------------------------------");
        printf("\nTurn %d - Player %c's Turn", turnCount, currentPlayer);
        
        if (illegalMove) {
            printf("- Space Occupied");
            illegalMove = false;
        }

        printf("\n");
        printf("\nArrow Keys - NAVIGATE");
        printf("\nEnter - END TURN");
        
        printf("\e[?25l"); // Hide the actual terminal cursor.
        
        if (cursorHidden) {
            cursorHidden = false; // Unhide the game cursor.
        }

        // Keyhandler
        char ch = getch();

        // The most repetitive code you'll ever see
        switch(ch) {
            // If on the edges, loop back to the other side of the board.
            case 'A': // Up
                if (cursorY == 0) cursorY = BOARD_SIZE - 1;
                else cursorY--;
                break;
            case 'B': // Down
                if (cursorY == BOARD_SIZE - 1) cursorY = 0;
                else cursorY++;
                break;
            case 'C': // Right
                if (cursorX == BOARD_SIZE - 1) cursorX = 0;
                else cursorX++;
                break;
            case 'D': // Left
                if (cursorX == 0) cursorX = BOARD_SIZE - 1;
                else cursorX--;
                break;
            // Listen for return carriage, when ENTER is pressed.
            case '\n':
                if (getGridItem(board, cursorX, cursorY) != '.') {
                    illegalMove = true;
                }
                else {
                    setGridItem(board, cursorX, cursorY, currentPlayer);

                    // Hide the cursor to reflect board change.
                    cursorHidden = true;
                    
                    // Update game state.
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
            default: 
                ; // Do nothing, if not any of arrow keys or ENTER.
        }
    }
    
    // Free memory that was dynamically allocated for the board.
    free(board);

    // Kill game loop.
	return 0;
}

// Starts program, loops the game when it ends.
int main() {
    while(1) {
        game();
    }
}