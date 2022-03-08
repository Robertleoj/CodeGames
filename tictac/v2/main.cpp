#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <time.h>

using namespace std;

#define MAX_HEURISTIC 1000
#define PLAY_CLOCK 100
#define START_CLOCK 1000

#define EMPTY 0
#define CROSS 1
#define CIRCLE 2

// typedef char PLAYER;

int CLOCK = 0;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

class TimeException : public exception {
public:
    virtual const char* what() const throw() {
        return "Time is up";
    }
} time_exception;


class State {
public:
    bool turn;
    vector<vector<int>> move_sequence;
    char board[3][3][3][3];
    char board_wins[3][3];

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
        return s;
    }

    pair<int, int> board_idx_to_coord(vector<int> idx){
        return pair<int, int>(idx[0] * 3 + idx[2], idx[1] * 3 + idx[3]);
    }

    vector<int> coord_to_board_idx(int x, int y){
        int outerX = x / 3;
        int outerY = y / 3;
        int innerX = x % 3;
        int innerY = y % 3;
        return vector<int> {outerX, outerY, innerX, innerY};
    }

    char player_nm(){
        return turn ? CROSS : CIRCLE;
    }

    void set_board(vector<int> &idx, char n){
        board[idx[0]][idx[1]][idx[2]][idx[3]] = n;
    }

    pair<bool, int> small_board_is_terminal(char b[3][3]){


        for(int i = 0; i < 3; i++){
            if(b[i][0] == b[i][1] && b[i][1] == b[i][2] && b[i][0] != EMPTY){
                // return the right heuristic
                return pair<bool, int>(true, b[i][0]);
            }

            if(b[0][i] == b[1][i] && b[1][i] == b[2][i] && b[0][i] != EMPTY){
                // return the right heuristic
                return pair<bool, int>(true, b[0][i]);
            }

        }
        if(b[0][0] == b[1][1] && b[1][1] == b[2][2] && b[0][0] != EMPTY){
            // return the right heuristic
            return pair<bool, int>(true, b[0][0]);
        }
        if(b[0][2] == b[1][1] && b[1][1] == b[2][0] && b[0][2] != EMPTY){
            // return the right heuristic
            return pair<bool, int>(true, b[0][2]);
        }

        int num_squares_played = 0;
        int circle_wins = 0;
        int cross_wins = 0;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                // if(b[i][j] != EMPTY){
                //     num_squares_played++;
                // }
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
            if(cross_wins > circle_wins){
                return pair<bool, int>(true, CROSS);
            }
            if(cross_wins < circle_wins){
                return pair<bool, int>(true, CIRCLE);
            }
        }

        return pair<bool, int>(false, 0);
    }

    void move(vector<int> idx){
        set_board(idx, player_nm());
        this->move_sequence.push_back(idx);
        this->turn ^= true;

        pair<bool, int> result = small_board_is_terminal(board[idx[0]][idx[1]]);

        if(result.first){
            board_wins[idx[0]][idx[1]] = result.second;
        }

    }

    void move(int x, int y){
        vector<int> idx = coord_to_board_idx(x, y);
        move(idx);
    }

    void undo_move(vector<int> idx){
        set_board(idx, EMPTY);
        this->move_sequence.pop_back();
        this->turn ^= true;
        board_wins[idx[0]][idx[1]] = EMPTY;
    }

    int term_points(int winning_player){
        return ((winning_player == CROSS) ? MAX_HEURISTIC : -MAX_HEURISTIC) * (turn ? 1 : -1);
    }

    pair<bool, int> is_terminal(){

        // First check if the board is won by either player
        pair<bool, int> big_board_term = small_board_is_terminal(board_wins);
        if(big_board_term.first){
            return pair<bool, int>(true, term_points(big_board_term.second));
        }

        // else check whether the board is full
        // if(move_sequence.size() == 9 * 9){
        //     int cross_wins = 0;
        //     int circle_wins = 0;
        //     for(int i = 0; i < 3; i++){
        //         for(int j = 0; j < 3; j++){
        //             if(board_wins[i][j] == CROSS){
        //                 cross_wins++;
        //             }
        //             if(board_wins[i][j] == CIRCLE){
        //                 circle_wins++;
        //             }
        //         }
        //     }

        //     if(circle_wins > cross_wins){
        //         return pair<bool, int>(true, term_points(CIRCLE));
        //     }
        //     if (cross_wins > circle_wins){
        //         return pair<bool, int>(true, term_points(CROSS));
        //     } 

        //     return pair<bool, int>(true, 0);
        // }

        return pair<bool, int>(false, 0);
    }

    vector<vector<int>> get_available_moves(){
        // cerr << "getting the last move" << endl;

        // cerr << "got the last move" << endl;
        vector<vector<int>> available_moves;

        // Check if we can play in the square played in
        if(move_sequence.size() > 0){
            vector<int> last_move = move_sequence[move_sequence.size() - 1];
            if(board_wins[last_move[2]][last_move[3]] == EMPTY){
                // cerr << "board is available: " << last_move[2] << " " << last_move[3] << endl;
                // We must play in that board
                for(int i = 0; i < 3; i ++ ){
                    for(int j = 0; j < 3; j++){
                        if(board[last_move[2]][last_move[3]][i][j] == EMPTY){
                            available_moves.push_back(vector<int> {last_move[2], last_move[3], i, j});
                            // cout << "YEET"<< last_move[0] << ',' << last_move[1] << ',' << i << ',' << j << endl;
                        }
                    }
                }

                return available_moves;
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
                                available_moves.push_back(vector<int> {i, j, k, l});
                                // cout << "SHIT" << endl;
                            }
                        }
                    }
                }
            }
        }
        return available_moves;

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
        int cross_wins = 0;
        int circle_wins = 0;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                if (s.board_wins[i][j] == CROSS){
                    cross_wins++;
                }else if(s.board_wins[i][j] == CIRCLE){
                    circle_wins++;
                }
            }
        }
        if(cross_wins > circle_wins){
            return s.term_points(CROSS);
        } 
        if(cross_wins < circle_wins){
            return s.term_points(CIRCLE);
        }
        return 0;
    }
};
double elapsed_time_ms(){
    double elapsed = (((double)(clock() - CLOCK)) / (double)CLOCKS_PER_SEC) * 1000;
    // cerr << "Elapsed time: " << elapsed << endl;
    return elapsed;
}

pair<vector<int>,int> negamax(State &state,int depth, int alpha, int beta){
    if(elapsed_time_ms() > PLAY_CLOCK - 5){
        throw TimeException();
    }

    // cerr << "Checking whether terminal" << endl;
    pair<bool, int> term = state.is_terminal();
    // cerr << "Terminal: " << term.first << endl;
    if(term.first){
        // cout << "value: " << -term.second << " " << state.turn << endl;
        // state.print_board();
        return pair<vector<int>,int>(vector<int>{-1,-1,-1,-1}, term.second);
    }
    vector<vector<int>> moves = state.get_available_moves();
    // if(moves.size() == 0){
    //     cerr << "WTF" << endl;
    //     exit(0);
    // }
    if(depth == 0){// || moves.size() == 0){
        return pair<vector<int>,int>(vector<int>{-1,-1,-1,-1}, 0);
    }

    // cerr << "Getting available moves" << endl;
    // if(elapsed_time_ms() > PLAY_CLOCK - 5){
    //     throw TimeException();
    // }

    // cerr << "Got available moves" << endl;
    int value = -MAX_HEURISTIC;
    vector<int> best_move = moves[0];
    for (auto move : moves)
    {
        // cerr << move[0] << ' ' << move[1] << ' ' << move[2] << ' ' << move[3] << endl;
        // best_move = move;
        
        state.move(move);
        pair<vector<int>, int> result = negamax(state, depth - 1, -beta, -alpha);
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
    return pair<vector<int>,int>(best_move, value);
}

pair<int, int> get_move(State state){
    CLOCK = clock();
    vector<int> curr_best_move;
    int depth = 1;
    // cerr << "Copying state" << endl;
    State cpy_state = state.copy();
    // cerr << "done copying" << endl;

    while(true){
        try{
            cerr << "depth: " << depth << endl;
            // cerr << "entering negamax search" << endl;
            pair<vector<int>,int> result = negamax(cpy_state, depth, -MAX_HEURISTIC, MAX_HEURISTIC);
            // cerr << "done negamax search" << endl;
            curr_best_move = result.first;
            if(result.second == MAX_HEURISTIC){
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

int main()
{
    ios_base::sync_with_stdio(0); cin.tie(0);
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
