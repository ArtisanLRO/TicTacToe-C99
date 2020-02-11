/*
A horrendously simplistic Tic-Tac-Toe written in ANSI C for use in Linux terminals
by ArtisanLRO
@MIT License
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <math.h>

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

// A structure that contains necessary game data.
struct gameState 
{
    char* board;
    char currentPlayer;
    char lastPlayer;
    char playerFirst;
    char playerSecond;
    int turnCount;
    int cursorX;
    int cursorY;
    bool cursorHidden;
    bool illegalMove;
};

// Set the current item in a given board position.
void setGridItem(char* board, int x, int y, char item) {
    *(board + y * BOARD_SIZE + x) = item; 
}

void setGridItemByIndex(char* board, int i, char item) {
    *(board + i) = item; 
}

// Show the current item in a given board position.
char getGridItem(char* board, int x, int y) {
    return *(board + y * BOARD_SIZE + x);
}

void writeData(struct gameState state) {
    
    FILE * datFile;

    datFile = fopen ("./save.dat", "w+");
    fprintf(datFile, "[TicTacToe-C99 Save Data]\n");

    fprintf(datFile, "%d\n", BOARD_SIZE * BOARD_SIZE);

    fprintf(datFile, "%d\n", state.turnCount);

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            fprintf(datFile, "%c", getGridItem(state.board, x, y));
        }
    }

    fprintf(datFile, "\n");

    fprintf(datFile, "%c", state.currentPlayer);
    fprintf(datFile, "%c", state.lastPlayer);
    fprintf(datFile, "%c", state.playerFirst);
    fprintf(datFile, "%c", state.playerSecond);
    fprintf(datFile, "%d", state.cursorHidden);
    fprintf(datFile, "%d", state.illegalMove);

    fclose(datFile);

    printf("\n\nGame saved to file.");
}

void readData(struct gameState state)
{
    // Start from the first line on the first iteration.
    int i = 0;

    // Maximum line length of 150.
    char line[150];
    FILE * datFile = fopen("save.dat", "r");

    bool validHeader;
    bool validMapSize;
    bool validMapContents;
    bool validParameters;
    
    int mapSize;

    while (1) {
        // If end of file has not yet been reached, continue.
        if (fgets(line, 150, datFile) == NULL) break;
        i++;

        switch(i) {
            // On the first iteration, verify that line is equal
            // through a string compare.
            case 1:
                if (strcmp(line, "[TicTacToe-C99 Save Data]")) {
                    //printf("\n\nSave file header is valid.");
                    validHeader = true;
                }
                break;
            // We parse the next line and pass it as an integer.
            // Check if map size is a valid square.
            case 2:
                ;
                mapSize = atoi(line);
                
                float testFloat = sqrt((double)mapSize);
                int testInt = testFloat;

                if (testFloat == testInt) {
                    //printf("\nMap size is %d squares. Valid.", mapSize);
                    validMapSize = true;
                }
                else {
                    //printf("\nMap size is %d squares. Invalid.", mapSize);
                }
                break;
            case 3:
                ;
                int turnCount = atoi(line);
                state.turnCount = turnCount;

                //printf("%d : Turn Count ", turnCount);
                break;
            case 4:
                ;
                int mapLineLength = strlen(line);
                if (mapSize == mapLineLength - 1) {
                    //printf("\nMap line length and size are equal.");
                    validMapContents = true;

                    // Note, this don't bother to check map integrity.
                    // This just loads stuff into the map.
                    size_t c = 0;
                    while (line[c] != '\0') {
                        printf("%c", line[c]);
                        setGridItemByIndex(state.board, c, line[c]);
                        c++;
                    }
                }
                break;
            case 5:
                ;
                int gameParameters = strlen(line);
                if (gameParameters == 6) {
                    //printf("\nGame parameters line length looks okay.");
                    validParameters = true;

                    size_t c = 0;
                    while (line[c] != '\0') {
                        c++;
                        switch(c) {
                            case 1:
                                state.currentPlayer = line[c];
                                break;
                            case 2:
                                state.lastPlayer = line[c];
                                break;
                            case 3:
                                state.playerFirst = line[c];
                                break;
                            case 4:
                                state.playerSecond = line[c];
                                break;
                            case 5:
                                state.cursorHidden = line[c];
                                break;
                            case 6:
                                state.illegalMove = line[c];
                                break;
                            default:
                                ;
                        }
                    }
                }
                break;
            default:
                ;
        }

    }
    if (validHeader && validMapSize && validMapContents && validParameters) {
        if (mapSize == BOARD_SIZE) {
            printf("\n\nGame loaded from save.");
        }
        else {
            printf("\n\nMap size mismatch. Rebuild program with proper dimensions.");
        }
    }

    //printf("%d %d %d %d", validHeader, validMapSize, validMapContents, validParameters);
    fclose(datFile);
}   

// Display the game board in the terminal.
void renderBoard(char* board, int cursorX, int cursorY, bool cursorHidden) {
    
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {\
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
    
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
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

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
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
    int counter;
    char charBeingChecked;
    
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
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

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
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

int printGame(struct gameState state) {
    // Clear the terminal.
    system("clear");

    // Print the header.
    printf("\nC99 Tic-Tac-Toe by ArtisanLRO");
    printf("\n--------------------------------\n");
    
    // Render the game board.
    renderBoard(
        state.board, 
        state.cursorX, 
        state.cursorY, 
        state.cursorHidden);

    // Listen for game end conditions, end the game if any are true.
    if (victoryHandler(state.board, state.lastPlayer)) {
        return 0;
    }

    // Print game details.    
    printf("\n--------------------------------");
    printf("\nTurn %d - Player %c's Turn", state.turnCount, state.currentPlayer);
    
    if (state.illegalMove) {
        printf(" - Space Occupied");
        state.illegalMove = false;
    }

    printf("\n");
    printf("\nArrow Keys - NAVIGATE");
    printf("\nEnter - END TURN");
    
    printf("\e[?25l"); // Hide the actual terminal cursor.

    return 1;
}

void gameLoop(struct gameState state) {
    // The game loop.
	while(1) {
        printGame(state);

        // Keyhandler
        char ch = getch();

        // The most repetitive code you'll ever see
        switch(ch) {
            // If on the edges, loop back to the other side of the board.
            case 'A': // Up
                if (state.cursorY == 0) state.cursorY = BOARD_SIZE - 1;
                else state.cursorY--;
                state.cursorHidden = false;
                break;
            case 'B': // Down
                if (state.cursorY == BOARD_SIZE - 1) state.cursorY = 0;
                else state.cursorY++;
                state.cursorHidden = false;
                break;
            case 'C': // Right
                if (state.cursorX == BOARD_SIZE - 1) state.cursorX = 0;
                else state.cursorX++;
                state.cursorHidden = false;
                break;
            case 'D': // Left
                if (state.cursorX == 0) state.cursorX = BOARD_SIZE - 1;
                else state.cursorX--;
                state.cursorHidden = false;
                break;
            case 'l':
            case 'L':
                // Start from the first line on the first iteration.
                ;
                int i = 0;

                // Maximum line length of 150.
                char line[150];
                FILE * datFile = fopen("save.dat", "r");

                bool validHeader;
                bool validMapSize;
                bool validMapContents;
                bool validParameters;

                int mapSize;

                while (1) {
                    // If end of file has not yet been reached, continue.
                    if (fgets(line, 150, datFile) == NULL) break;
                    i++;

                    switch(i) {
                        // On the first iteration, verify that line is equal
                        // through a string compare.
                        case 1:
                            if (strcmp(line, "[TicTacToe-C99 Save Data]")) {
                                printf("\n\nSave file header is valid.");
                                validHeader = true;
                            }
                            break;
                        // We parse the next line and pass it as an integer.
                        // Check if map size is a valid square.
                        case 2:
                            ;
                            mapSize = atoi(line);
                            
                            float testFloat = sqrt((double)mapSize);
                            int testInt = testFloat;

                            if (testFloat == testInt) {
                                printf("\nMap size is %d squares. Valid.", mapSize);
                                validMapSize = true;
                            }
                            else {
                                printf("\nMap size is %d squares. Invalid.", mapSize);
                            }
                            break;
                        case 3:
                            ;
                            int turnCount = atoi(line);
                            state.turnCount = turnCount;

                            printf("%d : Turn Count ", turnCount);
                            break;
                        case 4:
                            ;
                            int mapLineLength = strlen(line);
                            if (mapSize == mapLineLength - 1) {
                                printf("\nMap line length and size are equal.");
                                validMapContents = true;

                                // Note, this don't bother to check map integrity.
                                // This just loads stuff into the map.
                                size_t c = 0;
                                while (line[c] != '\0') {
                                    printf("%c", line[c]);
                                    setGridItemByIndex(state.board, c, line[c]);
                                    c++;
                                }
                            }
                            break;
                        case 5:
                            ;
                            int gameParameters = strlen(line);
                            if (gameParameters == 6) {
                                printf("\nGame parameters line length looks okay.");
                                validParameters = true;

                                size_t c = 0;
                                while (line[c] != '\0') {
                                    c++;
                                    switch(c) {
                                        case 1:
                                            state.currentPlayer = line[c];
                                            break;
                                        case 2:
                                            state.lastPlayer = line[c];
                                            break;
                                        case 3:
                                            state.playerFirst = line[c];
                                            break;
                                        case 4:
                                            state.playerSecond = line[c];
                                            break;
                                        case 5:
                                            state.cursorHidden = line[c];
                                            break;
                                        case 6:
                                            state.illegalMove = line[c];
                                            break;
                                        default:
                                            ;
                                    }
                                }
                            }
                            break;
                        default:
                            ;
                    }

                }
                if (validHeader && validMapSize && validMapContents && validParameters) {
                    printf("\n\nGame loaded from save.");
                }

                printf("%d %d %d %d", validHeader, validMapSize, validMapContents, validParameters);
                fclose(datFile);
                break;
            case 's':
            case 'S':
                writeData(state);
                break;
            // Listen for return carriage, when ENTER is pressed.
            case '\n':
                if (getGridItem(state.board, state.cursorX, state.cursorY) != '.') {
                    state.illegalMove = true;
                }
                else {
                    setGridItem(state.board, state.cursorX, state.cursorY, state.currentPlayer);

                    // Hide the cursor to reflect board change.
                    state.cursorHidden = true;
                    
                    // Update game state.
                    state.turnCount++;
                    state.lastPlayer = state.currentPlayer;

                    if (state.currentPlayer == state.playerFirst) {
                        state.currentPlayer = state.playerSecond;
                    }
                    else {
                        state.currentPlayer = state.playerFirst;
                    }
                }
                break;
            default: 
                ; // Do nothing, if not any of arrow keys or ENTER.
        }
    }
}

// Initialises game logic, then loops till victory or draw condition.
int initialiseGame() {

    // Player symbols.
    char playerFirst = 'X';
    char playerSecond = 'O';

    // Necessary to record the current player's turn.
    char currentPlayer = 'X';
    char lastPlayer = '.';

    // The game starts on the first turn.
    int turnCount = 0;

    // Initialise cursor position.
    int cursorX = 0;
    int cursorY = 0;

    // Status message helpers.
    bool illegalMove = false;
    bool cursorHidden = false;

    // Dynamically allocates memory for the square grid of the game board.
    char *board = (char *)malloc(BOARD_SIZE * BOARD_SIZE * sizeof(char));

    // Initialise the game board with blank space.
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            setGridItem(board, x, y, '.');
        }
    }

    // Initialise a game state structure.
    struct gameState state = {
        board,
        currentPlayer,
        lastPlayer,
        playerFirst,
        playerSecond,
        turnCount,
        cursorX,
        cursorY,
        cursorHidden,
        illegalMove
    };    

    // The game loop.
	gameLoop(state);

    // Free memory dynamically allocated for the game board at end of game.
    free(board);
}

// Starts program, loops the game when it ends.
int main() {
    while(1) {
        initialiseGame();
    }
}