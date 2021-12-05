/**
 * Andrea Princic, 1837592
 * Tic Tac Toe and Connect Four simulator
 * start of the project: 12/11/2021
 * end of the project: 16/11/2021
 */
#include "Arduino.h"
#include "libs/matrix/NS_Rainbow.h"
#include "libs/keypad/Keypad.h"
#include "main.h"
#include "characters.h"


//keyboard's params
#define KROWS 4
#define KCOLS 4

//matrix params
#define MPIN 11
#define MROWS 8
#define MCOLS 8
#define N_CELL (MROWS * MCOLS)
//transform a pair (i, j) of coordinates to a single number for matrix
#define TO_M_CELL(i, j) ((i) * MCOLS + (j))

//color constants
#define RED 0x00ff0000
#define GREEN 0x0000ff00
#define BLUE 0x000000ff
#define BLACK 0x00000000
#define WHITE 0x00ffffff
#define PLAYER1 0x0006a800
#define PLAYER2 0x00d60000
#define TO_PLAYER(t) (t == 1 ? PLAYER1 : PLAYER2)

//game constants
#define TTT_ROWS 3
#define TTT_COLS 3

#define C4_ROWS 6
#define C4_COLS 7


//CODE FROM DOCS http://69.195.111.207/tutorial/ELEGOO/01%20Arduino%20Starter%20Kit%20Tutorial/01%20Uno/03%20Complete/ELEGOO%20The%20Most%20Complete%20Starter%20Kit%20for%20UNO%20V1.0.2021.05.13.zip
//define the symbols on the buttons of the keypads
char hexaKeys[KROWS][KCOLS] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
};
byte rowPins[KROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[KCOLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, KROWS, KCOLS);

//CODE FROM DOCS http://www.nulsom.com/source/NS_Rainbow.zip
// Parameter 1 = number of cells (max: 512)
// Parameter 2 = Arduino pin number (default pin: 9)
NS_Rainbow matrix = NS_Rainbow(N_CELL, MPIN);


void setup() {
    randomSeed(analogRead(0));
    matrix.begin();
    matrix.setBrightness(50);
    //clear matrix if previously used
    matrix.clear();
    matrix.show();
}


void loop() {
    print("select the game. 1: tic tac toe. 2: connect 4.    ", 400, RED);
    char input;
    //wait for user to select a game
    while(!(input = keypad.getKey()))
        delay(10);
    //if an invalid game is selected, ignore and repeat loop()
    switch(input) {
        case '1':
            tic_tac_toe();
            break;
        case '2':
            connect_four();
            break;
        default:
            break;
    }
    matrix.clear();
    matrix.show();
    delay(1000);
}


/**
 * print random colors on the matrix for a given number of times waiting for the given number of ms before the next cycle
 * @param times number of cycles to execute
 * @param interval time to wait between two cycles
 */
void randoms(byte times, uint16_t interval) {
    for(byte time = 0; time < times; ++time) {
        for(byte i = 0; i < MROWS; ++i) {
            for(byte j = 0; j < MCOLS; ++j) {
                //random color cannot be black
                matrix.setColor(
                        TO_M_CELL(i, j),
                        random(1, 256),
                        random(1, 256),
                        random(1, 256)
                );
            }
        }
        matrix.show();
        delay(interval);
    }
}


/**
 * set all the pixels on the matrix to the same color
 * @param r red component
 * @param g green component
 * @param b blue component
 */
void set_all(byte r, byte g, byte b) {
    for(byte i = 0; i < MROWS; ++i) {
        for(byte j = 0; j < MCOLS; ++j) {
            matrix.setColor(TO_M_CELL(i, j), r, g, b);
        }
    }
    matrix.show();
}


/**
 * same as set_all(r, g, b) but use single number notation (only needed for compatibility with color constants)
 * @param c color, comprehensive of all (r, g, b) parts
 */
void set_all(uint32_t c) {
    set_all((uint8_t)(c >> 16), (uint8_t)(c >> 8), (uint8_t)c);
}


/**
 * print a given single char on the matrix
 * @param letter
 * @param display_time
 * @param color
 */
void print_char(unsigned char letter, uint32_t color) {
    static const unsigned char last_letter = 'z' - 'a', last_number = 'z' - 'a' + 1 + '9' - '0';
    //if letter is uppercase, subtract 65, so it can be used as an index
    if(letter >= 'A' && letter <= 'Z')
        letter -= 'A';
        //if letter is lowercase, subtract 97, so it can be used as an index
    else if(letter >= 'a' && letter <= 'z')
        letter -= 'a';
        //if letter is a number, subtract 48 and add one more than the index of the last letter, so it can be used as an index
    else if(letter >= '0' && letter <= '9')
        letter = last_letter + letter - '0' + 1;
    else
        //if letter is not an alphanumeric character, try to match a symbol
        switch(letter) {
            case '?':
                letter = last_number + 1;
                break;
            case ':':
                letter = last_number + 2;
                break;
            case '.':
                letter = last_number + 3;
                break;
            default:
                letter = last_number + 4;
        }
    matrix.clear();
    unsigned char *display = alphabet[letter];
    //for each row of the character from alphabet
    for(byte i = 0; i < 8; ++i) {
        //for each column of the row
        for(byte j = 0, b = B10000000; j < 8; ++j, b >>= 1) {
            //display the pixel if needed
            if(display[i] & b)
                matrix.setColor(TO_M_CELL(i, j), color);
        }
    }
    matrix.show();
}


/**
 * print a whole string on the matrix, with given interval between single characters
 * @param string string to print
 * @param display_time interval between characters
 * @param color color of the string
 */
void print(char *string, uint16_t display_time, uint32_t color) {
    for(unsigned long long i = 0; i < strlen(string); ++i) {
        print_char(string[i], color);
        delay(display_time);
    }
}


/**
 * manage a whole tic tac toe match
 */
void tic_tac_toe() {
    //print 10 random waves, then clear the matrix
    randoms(10, 50);
    set_all(0, 0, 0);
    //create the grid in the memory and on the matrix
    //turn is 1 for player 1 and -1 for player 2
    int8_t **grid = ttt_make_grid(), turn = 1;
    char key;
    //cycle until the game is over (victory or draft)
    while(!ttt_game_over(grid)) {
        key = keypad.waitForKey();
        //'*' quits the current match
        if(key == '*')
            break;
        //if the button is valid, check if the selected cell is free and make the move
        if('1' <= key && key <= '9' && ttt_make_turn(grid, turn, key - '1'))
            turn = -turn;
    }
    delay(1000);
    //if the match didn't end in a draft, print the winner's color on the matrix
    if(ttt_check_victory(grid)) {
        turn = -turn;
        set_all(TO_PLAYER(turn));
    } else
        //for a draft, set the color to white
        set_all(WHITE);
    ttt_free_grid(grid);
    delay(1000);
}


/**
 * make a player's move, if valid
 * @param grid grid for this match
 * @param turn current turn
 * @param cell selected cell (in single number)
 * @return true if the move succeeds, false otherwise
 */
bool ttt_make_turn(int8_t **grid, int8_t turn, uint8_t cell) {
    //convert single cell number into a pair (i, j) of coordinates
    byte i = cell / TTT_COLS, j = cell % TTT_COLS;
    //if the chosen cell is already occupied, the move fails
    if(grid[i][j] != 0)
        return false;
    //else assign the turn to the cell
    grid[i][j] = turn;
    //convert (i, j) into the correct coordinates on the matrix and display the player's color
    byte mi = i * 2 + i, mj = j * 2 + j;
    matrix.setColor(TO_M_CELL(mi, mj), TO_PLAYER(turn));
    matrix.setColor(TO_M_CELL(mi, mj) + 1, TO_PLAYER(turn));
    matrix.setColor(TO_M_CELL(mi, mj) + MCOLS, TO_PLAYER(turn));
    matrix.setColor(TO_M_CELL(mi, mj) + MCOLS + 1, TO_PLAYER(turn));
    matrix.show();
    return true;
}


/**
 * create the grid for a tic tac toe match and display it on the matrix
 * @return allocated memory for the grid
 */
int8_t **ttt_make_grid() {
    for(byte i = 2; i <= 5; i += 3) {
        for(byte j = 0; j < MCOLS; ++j) {
            matrix.setColor(TO_M_CELL(i, j), WHITE);
            matrix.setColor(TO_M_CELL(j, i), WHITE);
            matrix.show();
            delay(65);
        }
    }
    int8_t **grid = (int8_t **)(calloc(TTT_ROWS, sizeof(int8_t *)));
    for(byte i = 0; i < TTT_ROWS; ++i)
        grid[i] = (int8_t *)(calloc(TTT_COLS, sizeof(int8_t)));
    return grid;
}


/**
 * check if the match is in a victory state
 * a victory state is when 3 adjacent horizontal, vertical or diagonal cells have the same symbol
 * this is represented in the grid by having the sum of their values equal to 3 or -3
 * @param grid grid for this match
 * @return true if the game is a victory, false otherwise
 */
bool ttt_check_victory(int8_t **grid) {
    int8_t sum_rows = 0, sum_cols = 0, sum_diag1 = 0, sum_diag2 = 0;
    for(byte i = 0; i < TTT_ROWS; ++i, sum_rows = 0, sum_cols = 0) {
        //sum the values of both the i-th row and the i-th column
        for(byte j = 0; j < TTT_COLS; ++j) {
            sum_rows += grid[i][j];
            sum_cols += grid[j][i];
        }
        //sum the values of both the first and the second diagonals
        sum_diag1 += grid[i][i];
        sum_diag2 += grid[i][TTT_ROWS - i - 1];
        //check if current row or column is in a winning state
        if(sum_rows == 3 || sum_rows == -3 || sum_cols == 3 || sum_cols == -3)
            return true;
    }
    //check if diagonals are in a winning state
    return sum_diag1 == 3 || sum_diag1 == -3 || sum_diag2 == 3 || sum_diag2 == -3;
}


/**
 * check if the game is over (victory or draft)
 * @param grid grid for this match
 * @return true if the match is over, false otherwise
 */
bool ttt_game_over(int8_t **grid) {
    for(byte i = 0; i < TTT_ROWS; ++i)
        for(byte j = 0; j < TTT_COLS; ++j)
            //check if at least one of the cells is empty
            // if so, return if the game is over by a victory or not
            if(grid[i][j] == 0)
                return ttt_check_victory(grid);
    //if no cell is empty the game is over
    return true;
}


/**
 * free the memory for a tic tac toe grid
 * @param grid grid for this match
 */
void ttt_free_grid(int8_t **grid) {
    for(byte i = 0; i < TTT_ROWS; ++i)
        free(grid[i]);
    free(grid);
}


/**
 * manage a whole connect four match
 */
void connect_four() {
    //print 10 random waves, then clear the matrix
    randoms(10, 50);
    set_all(0, 0, 0);
    //create the grid in the memory and on the matrix
    //turn is 1 for player 1 and -1 for player 2
    int8_t **grid = c4_make_grid(), turn = 1;
    char key;
    //cycle until the game is over (victory or draft)
    while(!c4_game_over(grid)) {
        key = keypad.waitForKey();
        //'*' quits the current match
        if(key == '*')
            break;
        //if the button is valid, check if the selected column is free and make the move
        if('1' <= key && key <= '7' && c4_make_turn(grid, turn, key - '1'))
            turn = -turn;
    }
    delay(1000);
    //if the match didn't end in a draft, print the winner's color on the matrix
    if(c4_check_victory(grid)) {
        turn = -turn;
        set_all(TO_PLAYER(turn));
    } else
        //for a draft, set the color to white
        set_all(WHITE);
    c4_free_grid(grid);
    delay(1000);
}


/**
 * make a player's move, if valid
 * @param grid grid for this match
 * @param turn current turn
 * @param column selected column
 * @return true if the move succeeds, false otherwise
 */
bool c4_make_turn(int8_t **grid, int8_t turn, uint8_t column) {
    int8_t row = C4_ROWS - 1;
    //find the first free row in the given column
    // if the column is full, the move fails
    while(row >= 0 && grid[row][column] != 0)
        --row;
    if(row < 0)
        return false;
    //else assign the turn to the cell
    grid[row][column] = turn;
    //show dropping animation
    for(byte i = 0; i < row; ++i) {
        matrix.setColor(TO_M_CELL(i, column), TO_PLAYER(turn));
        matrix.show();
        delay(100);
        matrix.setColor(TO_M_CELL(i, column), BLACK);
        matrix.show();
    }
    //convert (i, column) into the correct coordinates on the matrix and display the player's color
    matrix.setColor(TO_M_CELL(row, column), TO_PLAYER(turn));
    matrix.show();
    return true;
}


/**
 * create the grid for a connect four match and display it on the matrix
 * @return allocated memory for the grid
 */
int8_t **c4_make_grid() {
    for(byte i = 0; i < MROWS; ++i) {
        matrix.setColor(TO_M_CELL(i, 7), WHITE);
        matrix.setColor(TO_M_CELL(6, i), WHITE);
        matrix.setColor(TO_M_CELL(7, i), WHITE);
        matrix.show();
        delay(65);
    }
    int8_t **grid = (int8_t **)(calloc(C4_ROWS, sizeof(int8_t *)));
    for(byte i = 0; i < C4_ROWS; ++i)
        grid[i] = (int8_t *)(calloc(C4_COLS, sizeof(int8_t)));
    return grid;
}


/**
 * check if the match is in a victory state
 * a victory state is when 4 adjacent horizontal, vertical or diagonal cells have the same symbol
 * this is represented in the grid by having the sum of their values equal to 4 or -4
 * @param grid grid for this match
 * @return true if the game is a victory, false otherwise
 */
bool c4_check_victory(int8_t **grid) {
    int8_t sum, diag;
    //victory check for rows
    for(byte i = 0; i < C4_ROWS; ++i) {
        sum = grid[i][0] + grid[i][1] + grid[i][2];
        for(byte j = 3; j < C4_COLS; ++j) {
            sum += grid[i][j];
            if(sum == 4 || sum == -4)
                return true;
            sum -= grid[i][j - 3];
        }
    }
    //victory check for columns
    for(byte j = 0; j < C4_COLS; ++j) {
        sum = grid[0][j] + grid[1][j] + grid[2][j];
        for(byte i = 3; i < C4_ROWS; ++i) {
            sum += grid[i][j];
            if(sum == 4 || sum == -4)
                return true;
            sum -= grid[i - 3][j];
        }
    }
    //victory check for columns
    for(byte i = 0; i < C4_ROWS - 3; ++i) {
        for(byte j = 0; j < C4_COLS - 3; ++j) {
            diag = grid[i][j] + grid[i + 1][j + 1] + grid[i + 2][j + 2] + grid[i + 3][j + 3];
            if(diag == 4 || diag == -4)
                return true;
            diag = grid[C4_ROWS - i - 1][j] + grid[C4_ROWS - i - 2][j + 1] + grid[C4_ROWS - i - 3][j + 2] +
                   grid[C4_ROWS - i - 4][j + 3];
            if(diag == 4 || diag == -4)
                return true;
        }
    }
    return false;
}


/**
 * check if the game is over (victory or draft)
 * @param grid grid for this match
 * @return true if the match is over, false otherwise
 */
bool c4_game_over(int8_t **grid) {
    for(byte i = 0; i < C4_ROWS; ++i)
        for(byte j = 0; j < C4_COLS; ++j)
            //check if at least one of the cells is empty
            // if so, return if the game is over by a victory or not
            if(grid[i][j] == 0)
                return c4_check_victory(grid);
    //if no cell is empty the game is over
    return true;
}


/**
 * free the memory for a connect four grid
 * @param grid grid for this match
 */
void c4_free_grid(int8_t **grid) {
    for(byte i = 0; i < C4_ROWS; ++i)
        free(grid[i]);
    free(grid);
}
