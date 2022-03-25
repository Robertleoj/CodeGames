#ifndef STATE_jfdklsjfe09f0jf90dsjf
#define STATE_jfdklsjfe09f0jf90dsjf
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <time.h>

using namespace std;

#define PLAY_CLOCK 100
#define START_CLOCK 1000

#define EMPTY 0
#define CROSS 1
#define CIRCLE 2



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

    pair<bool, int> is_terminal(){

        // First check if the board is won by either player
        pair<bool, int> big_board_term = small_board_is_terminal(board_wins);
        if(big_board_term.first){
            return pair<bool, int>(true, big_board_term.second);
        }

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


    pair<int, int> get_last_move(){
        if(move_sequence.size() > 0){
            vector<int> last_move = move_sequence[move_sequence.size() - 1];
            return board_idx_to_coord(last_move);
        }
        return pair<int, int>(-1, -1);
    }

    void print_board(){
        char cross_win[3][5] = {
            {'\\',  ' ', '/'},
            {' ',   'X', ' ' },
            {'/',   ' ', '\\'}
        };

        char circle_win[3][5] = {
            {' ',  '=',  ' '},
            {'|',  'O',  '|'},
            {' ',  '=',  ' '}
        };

        char print_board[3][3][3][3];
        for(int i = 0; i < 3; i ++){
            for(int j = 0; j < 3; j++){
                if(board_wins[i][j] == CIRCLE){
                    for(int k = 0; k < 3; k++){
                        for(int l = 0; l < 3; l++){
                            print_board[i][j][k][l] = circle_win[k][l];
                        }
                    }
                } else if(board_wins[i][j] == CROSS){
                    for(int k = 0; k < 3; k++){
                        for(int l = 0; l < 3; l++){
                            print_board[i][j][k][l] = cross_win[k][l];
                        }
                    }
                } else{
                    for(int k = 0; k < 3; k++){
                        for(int l = 0; l < 3; l++){
                            char sq;
                            char board_sq = board[i][j][k][l];
                            if(board_sq == CIRCLE){
                                sq = 'O';
                            } else if(board_sq == CROSS){
                                sq = 'X';
                            } else{
                                sq = ' ';
                            }
                            print_board[i][j][k][l] = sq;
                        }
                    } 
                }
            }
        }
        bool circle_square, cross_square;
        int board_symbol;
        // for(int i = 0; i < 3; i++){
        //     for(int j = 0; j < 3; j++){
        //         for(int k = 0; k < 3; k++){
        //             for(int l = 0; l < 5; l++){
        for(int x = 0; x < 9; x++){
            if(x % 3 == 0){
                for(int i = 0; i < 9 + 3 * 2 + 4; i++){
                    cout << '-';
                }
                cout << endl;
            }
            for(int y = 0; y < 9; y++){
                vector<int> coord = coord_to_board_idx(x, y);
                int i = coord[0];
                int j = coord[1];
                int k = coord[2];
                int l = coord[3];

                if(y % 3 == 0){
                    cout << "|";
                }
                circle_square = false;
                cross_square = false;
                board_symbol = board[i][j][k][l];

                if(board_symbol == CIRCLE){
                    circle_square = true;
                } else if(board_symbol == CROSS){
                    cross_square = true;
                }

                if(board_wins[i][j] == CROSS){
                    circle_square = false;
                    cross_square = true;
                }
                if(board_wins[i][j] == CIRCLE){
                    circle_square = true;
                    cross_square = false;
                }

                if(circle_square){
                    cout << "\u001b[34m";
                } else if(cross_square){
                    cout << "\u001b[33m";
                } 
                cout << print_board[i][j][k][l];
                if(circle_square || cross_square){
                    cout << "\u001b[0m";
                }
                if(l != 2){
                    cout << ' ';
                }
                if(y == 8){
                    cout << "|";
                }
            }
            cout << endl;
            if(x == 8){
                for(int i = 0; i < 9 + 3 * 2 + 4; i++){
                    cout << '-';
                }
                cout << endl;
            }
        }
        // cout << endl;
    }
};

#endif
