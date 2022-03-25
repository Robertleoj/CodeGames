#include "game.h"


Game::Game(){
    this->state = new State();
}

Game::~Game(){
    delete state;
}

void Game::move(board_idx move){
    this->state->move(move);
}




StateData Game::get_state_data(){
    StateData ret;
    ret.board = state->board;
    ret.win_boards = state->board_wins;
    ret.turn = state->turn;

    auto moves = state->get_available_moves();
    ret.allowed_moves = moves.first;
    ret.active_board = moves.second;

    ret.terminal = state->is_terminal();

    return ret;

}



