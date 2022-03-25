#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

class State {
public:
    bool turn;

    char board[3][3];


    State(){
        turn = true;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                board[i][j] = 0;
            }
        }
    }

    void move(int x, int y){
        if(board[x][y] == 0){
            board[x][y] = turn ? 'X' : 'O';
            turn = !turn;
        }
    }

    void undo_move(int x, int y){
        board[x][y] = 0;
        turn = !turn;
    }

    void sig_score(){

    }

    pair<bool, int> is_terminal(){
        for(int i = 0; i < 3; i++){
            if(board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != 0){
                return pair<bool, int>(true, 100);
            }
            if(board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != 0){
                return pair<bool, int>(true, 100);
            }
        }
        if(board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != 0){
            return pair<bool, int>(true, 100);
        }
        if(board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != 0){
            return pair<bool, int>(true, 100);
        }

        for (int i = 0; i< 3; i++){
            for (int j = 0; j< 3; j++){
                if (board[i][j] == 0){
                    return pair<bool, int>(false, 0);
                }
            }
        }

        return pair<bool, int>(true, 0);
    }

    vector<pair<int, int>> get_available_moves(){
        vector<pair<int, int>> moves;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                if(board[i][j] == 0){
                    moves.push_back(make_pair(i, j));
                }
            }
        }
        return moves;
    }

    void print_board(){
        for(int i = 0; i < 3; i++){
            cout << "|";
            for(int j = 0; j < 3; j++){
                if(board[i][j] != 0){
                    cout << board[i][j];
                } else {
                    cout << " ";
                }
                cout << '|';
            }
            cout << endl;
        }
    }
};

pair<pair<int, int>,int> negamax(State &state, int alpha, int beta){
    pair<bool, int> term = state.is_terminal();
    if(term.first){
        // cout << "value: " << -term.second << " " << state.turn << endl;
        // state.print_board();
        return pair<pair<int, int>,int>(make_pair(-1, -1),-term.second);
    }


    vector<pair<int, int>> moves = state.get_available_moves();
    int value = -100;
    pair<int, int> best_move;
    for(auto &child: moves){
        state.move(child.first, child.second);
        pair<pair<int, int>,int> result = negamax(state, -beta, -alpha);
        state.undo_move(child.first, child.second);
        if(-result.second > value){
            value = -result.second;
            best_move = child;
        }
        alpha = max(alpha, value);
        if(alpha >= beta){
            break;
        }
    }

    // cout << endl;
    // state.print_board();
    // cout << endl;


    return pair<pair<int, int>,int>(best_move, value);
    
}

pair<int, int> get_move(State state){
    auto ret =  negamax(state, -100, 100);
    // cout <<"Score: " << ret.second << endl;
    return ret.first;
}

int main()
{

    State state = State();
    // game loop
    while (1) {
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

        // state.print_board();
        // cout << endl;

        pair<int, int> best_move = get_move(state);

        state.move(best_move.first, best_move.second);
        // state.print_board();
        // cout << endl;


        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << best_move.first << " " << best_move.second << endl;
    }
}
