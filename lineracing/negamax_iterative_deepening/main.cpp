#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include<queue>
#include<set>

#define BX 30
#define BY 20
int DIRS[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

using namespace std;

vector<string> DIR_STRINGS = {"UP", "RIGHT", "DOWN", "LEFT"};
// vector<string> DIR_STRINGS = {"DOWN", "RIGHT", "UP", "LEFT"};
// vector<string> DIR_STRINGS = {"DOWN", "LEFT", "UP", "RIGHT"};
// vector<string> DIR_STRINGS = {"RIGHT", "UP", "LEFT", "DOWN"};


class State{
public:

    State(int num_players, vector<pair<int, int>> positions){
        this->positions = positions;
        this->num_players = num_players;
    }


    void apply_move(int player_idx, int dir){
        int x = positions[player_idx].first;
        int y = positions[player_idx].second;
        positions[player_idx].first += DIRS[dir][0];
        positions[player_idx].second += DIRS[dir][1];
        board[x][y] = player_idx + 1;
    }

    void kill(int player_idx){
        
        int x = positions[player_idx].first;
        int y = positions[player_idx].second;
        board[x][y] = 0;
    }

    State copy(){
        State s(num_players, positions);
        for(int i = 0; i < BX; i++){
            for(int j = 0; j < BY; j++){
                s.board[i][j] = board[i][j];
            }
        }
        return s;
    }

    vector<int> available_moves(int player_idx){
        vector<int> moves;
        for(int i = 0; i < 4; i++){
            if(is_allowed(positions[player_idx].first, positions[player_idx].second, i)){
                moves.push_back(i);
            }
        }
        return moves;
    }

private:

    bool is_allowed(int x, int y, int dir){
        int dirx = DIRS[dir][0];
        int diry = DIRS[dir][1];
        int nx = x + dirx;
        int ny = y + diry;
        if(nx < 0 || nx >= BX || ny < 0 || ny >= BY){
            return false;
        }
        if(board[nx][ny] == 0){
            return true;
        }
        return false;

    }
    char board[BX][BY];
    vector<pair<int,int>> positions;
    int num_players;
    set<int> dead;
};

class Heuristic{
public:
    int evaluate(State& state){
        return 0;
    }
};


class Agent{

public:
    int num_players;
    State * state;

    Agent(){

    }

    ~Agent(){
        delete state;
    }

    void initialize(int num_players, int player_idx, vector<pair<int, int>>& positions){
        this->num_players = num_players;
        this->state = new State(num_players, positions);
    }

    string get_move(int player_idx){
        return "";
    }

    pair<int,int> negamax(State &state, int depth, int alpha, int beta){
        pair<bool, int> term = state.is_terminal();
        if(term.first){
            // cout << "value: " << -term.second << " " << state.turn << endl;
            // state.print_board();
            return pair<int,int>(-1,-term.second);
        }

        if(depth == 0){
            return pair<int, int>()
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
        
};



int main(){
    bool initialized = false;
    State state = State();
    

    // game loop
    while (1) {
        int n; // total number of players (2 to 4).
        int p; // your player number (0 to 3).

        cin >> n >> p; cin.ignore();

        vector<vector<int>> player_states(n);
        for (int i = 0; i < n; i++) {
            vector<int> player_state(4);
            int x0; // starting X coordinate of lightcycle (or -1)
            int y0; // starting Y coordinate of lightcycle (or -1)
            int x1; // starting X coordinate of lightcycle (can be the same as X0 if you play before this player)
            int y1; // starting Y coordinate of lightcycle (can be the same as Y0 if you play before this player)
            cin >> x0 >> y0 >> x1 >> y1; cin.ignore();
            player_state[0] = x0;
            player_state[1] = BY - y0 - 1;
            player_state[2] = x1;
            player_state[3] = BY - y1 - 1;
            player_states[i] = player_state;
        }

        string move = state.select_move(player_states);

        state.print_board();
        // cerr << state.currx << " " << state.curry << endl;
        cerr << player_states[p][0] << " " << player_states[p][1] << " " << player_states[p][2] << " " << player_states[p][3] << endl;
        cerr << p << endl;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << move << endl; // A single line with UP, DOWN, LEFT or RIGHT
    }
}