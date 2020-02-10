# TicTacToe-C99
![image](https://i.imgur.com/i7gc3MM.png)

A horrendously simplistic Tic-Tac-Toe written in ANSI C for use in Linux terminals. Written to apply memory management, pointers and structures and basic C knowledge in preparation for CSSE2310.

## Installation Commands
```
git clone https://github.com/ArtisanLRO/TicTacToe-C99
cd TicTacToe-C99
gcc -std=c99 main.c -o main -lm
chmod +x ./main
./main
```
## Done So Far
- Placing symbols and taking turns, victory by horizontal, vertical or diagonal - game functions as it should
- Board can be resized to any square grid size
- Malloc management for the game board, passes the board by reference to helper functions
- Structure holds the game data and allows it to be passed to save/load helper functions
- Game saves and loads to a save.dat file by pressing the 'S' or 'L' buttons respectively, restoring game variables from previous state

## Possible Extensions
- Main game loop function can be broken down into more helper functions
- Convert load portion of game loop into its own separate function, by passing game variable pointers as argument to a proper load function
- Main menu can allow player to select game modes or select their own board size, dynamically allowing the map to change by editing the text file and loading without restarting and rebuilding the program
- Game code can be extended to add more game modes, multiple players, etc. easily
