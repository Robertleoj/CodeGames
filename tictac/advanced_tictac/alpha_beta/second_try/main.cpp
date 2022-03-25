#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <random>
#include <time.h>
#include <fstream>
#include <set>
#include <map>



using namespace std;

#define MAX_HEURISTIC 1000
#define PLAY_CLOCK 100
#define START_CLOCK 1000

#define SHUFFLE_MOVES false

#define EMPTY 0
#define CROSS 1
#define CIRCLE 2
#define FULL 3

typedef int sq_state;

// typedef char PLAYER;

int CLOCK = 0;


template <class T>
void shuffle_vector(vector<T> &v) {
    auto rng = default_random_engine{time(NULL)};
    shuffle(begin(v), end(v), rng);
}

class TimeException : public exception {
public:
    virtual const char* what() const throw() {
        return "Time is up";
    }
} time_exception;

class Move{
public:
    Move(){};
    Move(int outer_x, int outer_y, int inner_x, int inner_y){
        this->outer_x = outer_x;
        this->outer_y = outer_y;
        this->inner_x = inner_x;
        this->inner_y = inner_y;
    };

    int outer_x;
    int outer_y;
    int inner_x;
    int inner_y;
};

Move nullmv() {
    return Move(-1, -1, -1, -1);
}


class State {
public:
    bool turn;
    vector<Move> move_sequence;

    char board[3][3][3][3];
    char board_wins[3][3];

    int circle_wins;
    int cross_wins;
    int draws;

    State(){
        turn = true;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                board_wins[i][j] = EMPTY;
                for(int k = 0; k < 3; k++){
                    for(int l = 0; l < 3; l++){
                        board[i][j][k][l] = EMPTY;
                    }
                }
            }
        }
        circle_wins = 0;
        cross_wins = 0;
    }

    State copy(){
        State s;
        s.turn = turn;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                s.board_wins[i][j] = board_wins[i][j];
                for(int k = 0; k < 3; k++){
                    for(int l = 0; l < 3; l++){
                        s.board[i][j][k][l] = board[i][j][k][l];
                    }
                }
            }
        }
        for(auto v : move_sequence){
            s.move_sequence.push_back(v);
        }
        s.circle_wins = this->circle_wins;
        s.cross_wins = this->cross_wins;
        return s;
    }


    pair<int, int> board_idx_to_coord(Move idx){
        return pair<int, int>(idx.outer_x * 3 + idx.inner_x, idx.outer_y * 3 + idx.inner_y);
    }

    Move coord_to_board_idx(int x, int y){
        Move m;
        m.outer_x = x / 3;
        m.outer_y = y / 3;
        m.inner_x = x % 3;
        m.inner_y = y % 3;
        return m;
    }

    char player_nm(){
        return turn ? CROSS : CIRCLE;
    }

    void set_board(Move &idx, char n){
        board[idx.outer_x][idx.outer_y][idx.inner_x][idx.inner_y] = n;
    }

    pair<bool, sq_state> small_board_is_terminal(char b[3][3]){


        for(int i = 0; i < 3; i++){
            if(b[i][0] == b[i][1] && b[i][1] == b[i][2] && b[i][0] != EMPTY && b[i][0] != FULL){
                // return the right heuristic
                return pair<bool, sq_state>(true, b[i][0]);
            }

            if(b[0][i] == b[1][i] && b[1][i] == b[2][i] && b[0][i] != EMPTY && b[0][i] != FULL){
                // return the right heuristic
                return pair<bool, sq_state>(true, b[0][i]);
            }

        }
        if(b[0][0] == b[1][1] && b[1][1] == b[2][2] && b[0][0] != EMPTY && b[0][0] != FULL){
            // return the right heuristic
            return pair<bool, sq_state>(true, b[0][0]);
        }
        if(b[0][2] == b[1][1] && b[1][1] == b[2][0] && b[0][2] != EMPTY && b[0][2] != FULL){
            // return the right heuristic
            return pair<bool, sq_state>(true, b[0][2]);
        }

        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                if(b[i][j] == EMPTY){
                    return pair<bool, sq_state>(false, EMPTY);
                }
            }
        }

        // int num_squares_played = 0;
        // int circle_wins = 0;
        // int cross_wins = 0;
        // for(int i = 0; i < 3; i++){
        //     for(int j = 0; j < 3; j++){
        //         // if(b[i][j] != EMPTY){
        //         //     num_squares_played++;
        //         // }
        //         num_squares_played += (b[i][j] != EMPTY);
        //         if(b[i][j] == CIRCLE){
        //             circle_wins++;
        //         }
        //         if(b[i][j] == CROSS){
        //             cross_wins++;
        //         }
        //     }
        // }
        // if(num_squares_played == 9){
        //     if(cross_wins > circle_wins){
        //         return pair<bool, int>(true, CROSS);
        //     }
        //     if(cross_wins < circle_wins){
        //         return pair<bool, int>(true, CIRCLE);
        //     }
        // }

        return pair<bool, sq_state>(true, FULL);
    }

    void move(Move &idx){
        set_board(idx, player_nm());
        this->move_sequence.push_back(idx);
        this->turn ^= true;

        pair<bool, int> result = small_board_is_terminal(board[idx.outer_x][idx.outer_y]);

        if(result.first){
            board_wins[idx.outer_x][idx.outer_y] = result.second;
            if(result.second == CIRCLE){
                circle_wins++;
            } else if(result.second == CROSS){
                cross_wins++;
            } else {
                draws++;
            }
        }

    }

    void move(int x, int y){
        Move idx = coord_to_board_idx(x, y);
        move(idx);
    }

    void undo_move(Move &idx){
        set_board(idx, EMPTY);
        this->move_sequence.pop_back();
        this->turn ^= true;
        sq_state w_state = board_wins[idx.outer_x][idx.outer_y];
        if(w_state == CIRCLE){
            circle_wins--;
        } else if(w_state == CROSS){
            cross_wins--;
        } else if(w_state == FULL){
            draws--;
        }
        board_wins[idx.outer_x][idx.outer_y] = EMPTY;
    }

    int term_points(int winning_player){
        return ((winning_player == CROSS) ? MAX_HEURISTIC : -MAX_HEURISTIC) * (turn ? 1 : -1);
    }

    int correct_sign(int score){
        return (turn ? 1 : -1) * score;
    }

    pair<bool, int> is_terminal(){

        // First check if the board is won by either player
        pair<bool, int> big_board_term = small_board_is_terminal(board_wins);
        if(big_board_term.first){
            if(big_board_term.second != FULL){
                return pair<bool, int>(true, term_points(big_board_term.second));
            }
        }

        // Else check whether the board is full
        // for(int i = 0; i < 3; i++){
        //     for(int j = 0; j < 3; j++){
        //         if(board_wins[i][j] != FULL){
        //             return pair<bool, int>(false, 0);
        //         }
        //     }
        // }

        if(circle_wins + cross_wins + draws == 9){
            if(circle_wins == cross_wins){
                return pair<bool, int>(true, 0);
            }
            return pair<bool, int>(true, term_points(circle_wins > cross_wins ? CIRCLE : CROSS));
        }

        return pair<bool, int>(false, 0);
    }

    void get_available_moves(vector<Move> &available_moves){
        // cerr << "getting the last move" << endl;

        // cerr << "got the last move" << endl;


        // Check if we can play in the square played in
        if(move_sequence.size() > 0){
            Move last_move = move_sequence[move_sequence.size() - 1];
            sq_state win_val = board_wins[last_move.inner_x][last_move.inner_y];
            if(board_wins[last_move.inner_x][last_move.inner_y] == EMPTY){


                // pair<int, int> normal_solution = normal_tic_tac_solution(board[last_move.inner_x][last_move.inner_y]);
                // available_moves.push_back
                // available_moves.push_back(Move(last_move.inner_x, last_move.inner_y,normal_solution.first, normal_solution.second));
                // cerr << "board is available: " << last_move[2] << " " << last_move[3] << endl;
                // We must play in that board
                for(int i = 0; i < 3; i ++ ){
                    for(int j = 0; j < 3; j++){
                        if(board[last_move.inner_x][last_move.inner_y][i][j] == EMPTY){
                            available_moves.push_back(Move(last_move.inner_x, last_move.inner_y, i, j));
                            // cout << "YEET"<< last_move[0] << ',' << last_move[1] << ',' << i << ',' << j << endl;
                        }
                    }
                }
                return;
            }
        } 

        // We can play in any board
        // cerr << "Board is not available" << endl;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                if(board_wins[i][j] == EMPTY){
                    for(int k = 0; k < 3; k++){
                        for(int l = 0; l < 3; l++){
                            if(board[i][j][k][l] == EMPTY){
                                available_moves.push_back(Move(i, j, k, l));
                                // cout << "SHIT" << endl;
                            }
                        }
                    }
                }
            }
        }
        if(SHUFFLE_MOVES){
            shuffle_vector(available_moves);
        }
        return;

    }

    void print_won_board(){
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                cerr << (int)board_wins[i][j] << " ";
            }
            cerr << endl;
        }
    }
};

class Heuristic{
public:
    int evaluate(State s){
        // int cross_wins = 0;
        // int circle_wins = 0;
        // int max_cross_squares = 0;
        // int max_circle_squares = 0;
        // for(int i = 0; i < 3; i++){
        //     for(int j = 0; j < 3; j++){
        //         if (s.board_wins[i][j] == CROSS){
        //             cross_wins++;
        //         }else if(s.board_wins[i][j] == CIRCLE){
        //             circle_wins++;
        //         } 
        //         else {
        //             int cross_sq;
        //             int circle_sq;
        //             for(int k = 0; k < 3; k++){
        //                 for(int l = 0; l < 3; l++){
        //                     if(s.board[i][j][k][l] == CROSS){
        //                         cross_sq++;
        //                     } else if(s.board[i][j][k][l] == CIRCLE){
        //                         circle_sq++;
        //                     }
        //                 }
        //             }


        //             max_cross_squares = max(max_cross_squares, cross_sq);
        //             max_circle_squares = max(max_circle_squares, circle_sq);
        //         }


        //     }
        // }
        // if(cross_wins > circle_wins){
            // return s.term_points();
        // } 
        // if(cross_wins < circle_wins){
            // return s.term_points(CIRCLE);
        // }
        // return s.correct_sign(10*(cross_wins - circle_wins) + max_cross_squares - max_circle_squares);
        return s.correct_sign(s.cross_wins - s.circle_wins);
    }
};
double elapsed_time_ms(){
    double elapsed = (((double)(clock() - CLOCK)) / (double)CLOCKS_PER_SEC) * 1000;
    // cerr << "Elapsed time: " << elapsed << endl;
    return elapsed;
}

pair<Move,int> negamax(State &state,int depth, int alpha, int beta, Heuristic &heuristic){
    if(elapsed_time_ms() > PLAY_CLOCK - 2){
        throw TimeException();
    }

    // cerr << "Checking whether terminal" << endl;
    pair<bool, int> term = state.is_terminal();
    // cerr << "Terminal: " << term.first << endl;
    if(term.first){
        // cout << "value: " << -term.second << " " << state.turn << endl;
        // state.print_board();
        return pair<Move,int>(nullmv(), term.second);
    }
    if(depth == 0){// || moves.size() == 0){
        return pair<Move,int>(nullmv(), heuristic.evaluate(state));
    }
    vector<Move> moves;
    state.get_available_moves(moves);
    if(moves.size() == 0){
        return pair<Move,int>(nullmv(), 0);
    }
    // if(moves.size() == 0){
    //     cerr << "WTF" << endl;
    //     exit(0);
    // }


    // cerr << "Getting available moves" << endl;
    // if(elapsed_time_ms() > PLAY_CLOCK - 5){
    //     throw TimeException();
    // }

    // cerr << "Got available moves" << endl;
    int value = -MAX_HEURISTIC;
    Move best_move = moves[0];
    for (auto &move : moves)
    {
        // cerr << move[0] << ' ' << move[1] << ' ' << move[2] << ' ' << move[3] << endl;
        // best_move = move;
        
        state.move(move);
        pair<Move, int> result = negamax(state, depth - 1, -beta, -alpha, heuristic);
        state.undo_move(move);
        if(-result.second > value){
            value = -result.second;
            best_move = move;
        }
        alpha = max(alpha, value);
        if(alpha >= beta){
            break;
        }
    }
    return pair<Move,int>(best_move, value);
}

pair<int, int> get_move(State state){


    // Move lst_move = state.move_sequence[state.move_sequence.size() - 1];
    // if(state.board_wins[lst_move.inner_x][lst_move.inner_y] == EMPTY){
    //     pair<int, int> normal_solution = state.normal_tic_tac_solution(state.board[lst_move.inner_x][lst_move.inner_y]);
    //     return state.board_idx_to_coord(Move(lst_move.inner_x, lst_move.inner_y,normal_solution.first, normal_solution.second));
    // }

    CLOCK = clock();
    Move curr_best_move;
    int depth = 4;
    // cerr << "Copying state" << endl;
    State cpy_state = state.copy();
    // cerr << "done copying" << endl;
    Heuristic h = Heuristic();

    while(depth <= 40){
        try{
            cerr << "depth: " << depth << endl;
            // cerr << "entering negamax search" << endl;
            pair<Move,int> result = negamax(cpy_state, depth, -MAX_HEURISTIC, MAX_HEURISTIC, h);
            // cerr << "done negamax search" << endl;
            curr_best_move = result.first;
            if(abs(result.second) == MAX_HEURISTIC){
                break;
            }
            depth++;
        }catch(TimeException &e){
            cerr << e.what() << endl;
            break;
        }
    }
    // auto ret =  negamax(state, 5, -MAX_HEURISTIC, MAX_HEURISTIC);
    // cout <<"Score: " << ret.second << endl;
    return state.board_idx_to_coord(curr_best_move);
}

int main(int argc, char *argv[])
{
    // srand(time(NULL));
    
    // ifstream cin(argv[1]);
    // ofstream cout(argv[2]);

    // // optional performance optimizations    
    // ios_base::sync_with_stdio(false);
    // std::cin.tie(0);

    // std::cin.rdbuf(cin.rdbuf());
    // std::cout.rdbuf(cout.rdbuf());
    // if(argc == 3){
    //     freopen(argv[1], "w", stdout);
    //     freopen(argv[2], "r", stdin);
    // }
    // ios_base::sync_with_stdio(0); cin.tie(0);



    State state = State();
    // game loop
    // int i = 0;
    while (2) {
        // i++;
        int opponent_row;
        int opponent_col;
        cin >> opponent_row >> opponent_col; cin.ignore();
        int valid_action_count;
        // /*
        cin >> valid_action_count; cin.ignore();

        for (int i = 0; i < valid_action_count; i++) {
            int row;
            int col;
            cin >> row >> col; cin.ignore();
        }
        // */

        if(opponent_row != -1){
            state.move(opponent_row, opponent_col);
        }
        // Print state move sequence
        // cerr << "Move sequence after opponent move" << endl;
        // for(auto &move: state.move_sequence){
        //     pair<int, int> coords = state.board_idx_to_coord(move);
        //     cerr << coords.first << " " << coords.second << endl;
        // }

        // state.print_board();
        
        // cerr << "getting best move" << endl;
        state.print_won_board();
        pair<int, int> best_move = get_move(state);
        // cerr << "Out of getting best move" << endl;
        state.print_won_board();
        state.move(best_move.first, best_move.second);

        // cerr << "Move sequence after my move" << endl;
        // for(auto &move: state.move_sequence){
        //     pair<int, int> coords = state.board_idx_to_coord(move);
        //     cerr << coords.first << " " << coords.second << endl;
        // }
        // state.print_board();


        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        cout << best_move.first << " " << best_move.second << endl;
    }
}
