#ifndef PROJECT_MAIN_H
#define PROJECT_MAIN_H


void randoms(byte times, uint16_t interval);


void set_all(byte r, byte g, byte b);


void set_all(uint32_t c);


void print_char(unsigned char letter, uint32_t color);


void print(char *string, uint16_t display_time, uint32_t color);


void tic_tac_toe();


bool ttt_make_turn(int8_t **grid, int8_t turn, uint8_t cell);


int8_t **ttt_make_grid();


bool ttt_check_victory(int8_t **grid);


bool ttt_game_over(int8_t **grid);


void ttt_free_grid(int8_t **grid);


void connect_four();


bool c4_make_turn(int8_t **grid, int8_t turn, uint8_t column);


int8_t **c4_make_grid();


bool c4_check_victory(int8_t **grid);


bool c4_game_over(int8_t **grid);


void c4_free_grid(int8_t **grid);


#endif //PROJECT_MAIN_H
