#ifndef STATE_jfdklsjfe09f0jf90dsjf
#define STATE_jfdklsjfe09f0jf90dsjf
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <time.h>
#include "game_static.h"
#include "../static.h"


class State {
public:
    Turn turn;
    std::vector<board_idx> move_sequence;
    Condition ****board;
    Condition **board_wins;

    State();
    ~State();

    void set_board(board_idx &idx, Condition c);

    std::pair<bool, Condition> board_is_terminal(Condition **b, bool big_board);

    void move(board_idx idx);

    std::pair<bool, Condition> is_terminal();

    std::pair<std::vector<board_idx>, std::pair<int, int>> get_available_moves();

    Condition player_cond();

    void switch_turns();
};

#endif
