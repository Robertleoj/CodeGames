#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

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
    State(){
        for (int i = 0; i < BX; i++){
            for (int j = 0; j < BY; j++){
                board[i][j] = 0;
            }
        }

    }
    
    void initialize(int num_players, int player_idx){
        this->num_players = num_players;
        this->player_idx = player_idx;
    }   

    void print_board(){
        for(int i = 0; i < BX; i++){
            for(int j = 0; j < BY; j++){
                cerr <<(int) board[i][j];
            }
            cerr << endl;
        }
        cerr << endl;
    }

    void kill_dead(vector<int>& dead){
        if(dead.size() == 0){
            return;
        }
        for(int i = 0; i < BX; i++){
            for(int j = 0; j < BY; j++){
                for(int k = 0; k < dead.size(); k++){
                    if(board[i][j] == dead[k] + 1){
                        board[i][j] = 0;
                    }
                    break;
                }
            }
        }
    }

    bool in_bounds(int x, int y, int dir){
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

    string select_move(vector<vector<int>> player_states){

        vector<int> dead = {};
        for(int i = 0; i < num_players; i++){
            if(player_states[i][0] == -1){
                dead.push_back(i);
            }
            else{
                board[player_states[i][2]][player_states[i][3]] = i + 1;
                board[player_states[i][0]][player_states[i][1]] = i + 1;
            }
        }

        kill_dead(dead);


        int newx = player_states[player_idx][2];
        int newy = player_states[player_idx][3];
        // cout << "newx: " << newx << " newy: " << newy << endl;

        int ret_idx = -1;

        if(!initialized || (newx == currx && newy == curry)){
            // Find any direction
            initialized = true;
            for(int i = 0; i< 4; i++){
                if(in_bounds(newx, newy, i)){
                    // cout << currx << " " << curry << endl;
                    ret_idx =  i;
                }
            }
        } else {
            // Use an allowed direction
            int dir_x = newx - currx;
            int dir_y = newy - curry;
            // int dir_x = currx - newx ;
            // int dir_y = curry - newy;

            // cout << dir_x << " " << dir_y << endl;
            // Find the direction index
            int dir_idx = -1;
            for(int i = 0; i < 4; i++){
                if(DIRS[i][0] == dir_x && DIRS[i][1] == dir_y){
                    dir_idx = i;
                    break;
                }
            }

            cerr << "facing dir " << DIR_STRINGS[dir_idx] << ": [" <<dir_x << " " << dir_y << "]" << endl;
            // cerr <<  << endl;
            for(int i = -1; i < 2; i++){
                
                int check_dir = ((dir_idx + i) % 4 + 4) % 4;
                cerr << "Checking " << DIR_STRINGS[check_dir] << endl;
                if(in_bounds(newx, newy, check_dir)){
                // if(board[newx + DIRS[check_dir][0]][newy + DIRS[check_dir][1]] == 0){
                    ret_idx =  check_dir;
                }
            }
        }
        currx = newx;// + DIRS[check_dir][0];
        curry = newy;// + DIRS[check_dir][1];

        if(ret_idx == -1){
            ret_idx = 0;
        }

        cerr << "Expected next square = " << (int)currx + DIRS[ret_idx][0] << " " << (int)curry + DIRS[ret_idx][1] << endl;
        return DIR_STRINGS[ret_idx];




    }



    int num_players;
    int player_idx;

    bool initialized = false;
    int currx;
    int curry;

    char board[BX][BY];
};

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

int main()
{
    bool initialized = false;
    State state = State();
    

    // game loop
    while (1) {
        int n; // total number of players (2 to 4).
        int p; // your player number (0 to 3).

        cin >> n >> p; cin.ignore();
        if(!initialized){
            state.initialize(n, p);
            initialized = true;
        }

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
        cerr << state.currx << " " << state.curry << endl;
        cerr << player_states[p][0] << " " << player_states[p][1] << " " << player_states[p][2] << " " << player_states[p][3] << endl;
        cerr << p << endl;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << move << endl; // A single line with UP, DOWN, LEFT or RIGHT
    }

}