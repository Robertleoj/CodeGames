#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <time.h>
#include "game_static.h"
#include "state.h"
#include "../static.h"


State::State(){
    turn = CROSS_TURN;
    board = new Condition ***[3];
    board_wins = new Condition *[3];
    for(int i = 0; i < 3; i++){
        board[i] = new Condition **[3];
        board_wins[i] = new Condition [3];
        for(int j = 0; j < 3; j++){
            board[i][j] = new Condition *[3];
            board_wins[i][j] = EMPTY;

            for(int k = 0; k < 3; k++){
                board[i][j][k] = new Condition [3];
                for(int l = 0; l < 3; l++){
                    board[i][j][k][l] = EMPTY;
                }
            }
        }
    }
}

State::~State(){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            for(int k = 0; k < 3; k++){
                delete [] board[i][j][k];
            }
            delete [] board[i][j];
        }
        delete [] board[i];
    }
    delete [] board;
    delete [] board_wins;
}


void State::set_board(board_idx &idx, Condition c){
    board[idx[0]][idx[1]][idx[2]][idx[3]] = c;
}

std::pair<bool, Condition> State::board_is_terminal(Condition **b, bool big_board){


    for(int i = 0; i < 3; i++){
        if(b[i][0] == b[i][1] && b[i][1] == b[i][2] && b[i][0] != EMPTY && b[i][0] != DRAW){
            // return the right heuristic
            
            return std::pair<bool, Condition>(
                true, 
                b[i][0]
            );
        }

        if(b[0][i] == b[1][i] && b[1][i] == b[2][i] && b[0][i] != EMPTY && b[0][i] != DRAW){
            // return the right heuristic
            return std::pair<bool, Condition>(
                true, 
                b[0][i]
            );
        }

    }
    if(b[0][0] == b[1][1] && b[1][1] == b[2][2] && b[0][0] != EMPTY && b[0][0] != DRAW){
        // return the right heuristic
        return std::pair<bool, Condition>(
            true, b[0][0]
        );
    }
    if(b[0][2] == b[1][1] && b[1][1] == b[2][0] && b[0][2] != EMPTY && b[0][2] != DRAW){
        // return the right heuristic
        return std::pair<bool, Condition>(true, b[0][2]);
    }

    int num_squares_played = 0;
    int circle_wins = 0;
    int cross_wins = 0;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            num_squares_played += (b[i][j] != EMPTY);
            if(b[i][j] == CIRCLE){
                circle_wins++;
            }
            if(b[i][j] == CROSS){
                cross_wins++;
            }
        }
    }
    if(num_squares_played == 9){
        if(big_board){
            if(cross_wins > circle_wins){
                return std::make_pair(true, CROSS);
            }
            if(cross_wins < circle_wins){
                return std::make_pair(true, CIRCLE);
            }
        }
        return std::make_pair(true, DRAW);
    }

    return std::make_pair(false, EMPTY);
}

Condition State::player_cond(){
    if(turn == CROSS_TURN){
        return CROSS;
    }
    return CIRCLE;
}

void State::switch_turns(){
    if(turn == CROSS_TURN){
        turn = CIRCLE_TURN;
    } else{
        turn = CROSS_TURN;
    }
}

void State::move(board_idx idx){
    set_board(idx, player_cond());
    this->move_sequence.push_back(idx);
    switch_turns();

    std::pair<bool, Condition> result = board_is_terminal(board[idx[0]][idx[1]], false);

    if(result.first){
        board_wins[idx[0]][idx[1]] = result.second;
    }

}

std::pair<bool, Condition> State::is_terminal(){

    // First check if the board is won by either player
    auto big_board_term = board_is_terminal(board_wins, true);
    if(big_board_term.first){
        return std::make_pair(true, big_board_term.second);
    }

    return std::make_pair(false, EMPTY);
}

std::pair<std::vector<board_idx>, std::pair<int, int>> State::get_available_moves(){
    // cerr << "getting the last move" << endl;

    // cerr << "got the last move" << endl;
    std::vector<board_idx> available_moves;

    // Check if we can play in the square played in
    if(move_sequence.size() > 0){
        board_idx last_move = move_sequence[move_sequence.size() - 1];

        if(board_wins[last_move[2]][last_move[3]] == EMPTY){
            // cerr << "board is available: " << last_move[2] << " " << last_move[3] << endl;
            // We must play in that board
            for(int i = 0; i < 3; i ++ ){
                for(int j = 0; j < 3; j++){
                    if(board[last_move[2]][last_move[3]][i][j] == EMPTY){
                        available_moves.push_back(board_idx {last_move[2], last_move[3], i, j});
                    }
                }
            }

            return std::make_pair(available_moves, std::make_pair(last_move[2], last_move[3]));
        }
    } 

    // We can play in any board
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board_wins[i][j] == EMPTY){
                for(int k = 0; k < 3; k++){
                    for(int l = 0; l < 3; l++){
                        if(board[i][j][k][l] == EMPTY){
                            available_moves.push_back(board_idx {i, j, k, l});
                        }
                    }
                }
            }
        }
    }
    return std::make_pair(available_moves, std::make_pair(-1, -1));

}
