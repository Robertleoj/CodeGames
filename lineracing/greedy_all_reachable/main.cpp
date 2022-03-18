#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include<queue>
#include<set>
#include<math.h>

#define BX 30
#define BY 20
int DIRS[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};


#define DIST_POWER 1.5
#define WALL_DIST_MULTIPLIER 1

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
        positions = vector<pair<int, int>>(num_players);
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

    int num_reachable(int x, int y, int dir){
        int dirx = DIRS[dir][0];
        int diry = DIRS[dir][1];
        int nx = x + dirx;
        int ny = y + diry;
        int count = 0;

        queue<pair<int, int>> q;

        // run dfs to see how many squares are reachable
        // q.push_back(make_pair(nx, ny));
        q.push(make_pair(nx, ny));
        set<pair<int, int>> visited;
        
        pair<int, int> pos;
        while(q.size() > 0){
            
            pos = q.front();
            int cur_x = pos.first;
            int cur_y = pos.second;
            q.pop();

            // if(board[cur_x][cur_y] == 0 && visited.find(pos) == visited.end()){
            //     count++;
            // }

            for(int i = 0; i < 4; i++){
                int nx2 = cur_x + DIRS[i][0];
                int ny2 = cur_y + DIRS[i][1];
                if(in_bounds(cur_x, cur_y, i) && visited.find(make_pair(nx2, ny2)) == visited.end() && (nx2 != nx || ny2 != ny)) {
                    q.push(make_pair(nx2, ny2));
                    visited.insert(make_pair(nx2, ny2));
                    count++;
                }
            }
        }

        return count;
    }


    int min_dist_to_other(int x, int y, int player_idx){
        int min_dist = 10000;
        for(int i = 0; i < num_players; i++){
            if(i == player_idx){
                continue;
            }
            int dist = abs(positions[i].first - x) + abs(positions[i].second - y);
            if(dist < min_dist){
                min_dist = dist;
            }
        }
        return min_dist;
    }


    int max_distance_to_block(int x, int y){
        int max_dist = 0;
        // check positive x
        int dst = 0;
        for(int i = x + 1; i < BX; i++){
            if(board[i][y] != 0){
                break;
            }
            dst++;
        }
        max_dist = max(max_dist, dst);
        // check negative x
        dst = 0;
        for(int i = x - 1; i >= 0; i--){
            if(board[i][y] != 0){
                break;
            }
            dst++;
        }
        max_dist = max(max_dist, dst);
        // check positive y
        dst = 0;
        for(int i = y + 1; i < BY; i++){
            if(board[x][i] != 0){
                break;
            }
            dst++;
        }
        max_dist = max(max_dist, dst);

        // check negative y
        dst = 0;
        for(int i = y - 1; i >= 0; i--){
            if(board[x][i] != 0){
                break;
            }
            dst++;
        }

        max_dist = max(max_dist, dst);
        return max_dist;

    }

    int enemy_reachable(pair<int,int> &new_position, int player_idx){
        // Get the direction the enemy is facing
        int x = new_position.first;
        int y = new_position.second;
        int dir_x = x - positions[player_idx].first;
        int dir_y = y - positions[player_idx].second;

        int dir_idx = -1;
        for(int i = 0; i < 4; i++){
            if(DIRS[i][0] == dir_x && DIRS[i][1] == dir_y){
                dir_idx = i;
                break;
            }
        }
        int max_reachable = -1;
        int reachable = 0;
        int idx = 0;
        for(int i = -1; i < 2; i++){
            idx = ((dir_idx + i) % 4 + 4) % 4;
            if(in_bounds(x,y, idx)){
                reachable = num_reachable(x, y, idx);
                if(reachable > max_reachable){
                    max_reachable = reachable;
                }
            }
        }
        return max_reachable;
    }

    string select_move(vector<vector<int>> player_states){

        vector<pair<int, int>> new_positions = vector<pair<int, int>>(num_players);
        vector<int> dead = {};
        for(int i = 0; i < num_players; i++){
            if(player_states[i][0] == -1){
                dead.push_back(i);
            }
            else{
                board[player_states[i][2]][player_states[i][3]] = i + 1;
                board[player_states[i][0]][player_states[i][1]] = i + 1;
                new_positions[i] = make_pair(player_states[i][2], player_states[i][3]);
            }
        }

        kill_dead(dead);


        int newx = new_positions[player_idx].first;
        int newy = new_positions[player_idx].second;

        int currx = positions[player_idx].first;
        int curry = positions[player_idx].second;

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
            int max_reachable = 1 << 31;
            for(int i = -1; i < 2; i++){
                int check_dir = ((dir_idx + i) % 4 + 4) % 4;
                cerr << "Checking " << DIR_STRINGS[check_dir] << endl;
                
                if(in_bounds(newx, newy, check_dir)){
                    int score = num_reachable(newx, newy, check_dir);
                    int hypo_x = newx + DIRS[check_dir][0];
                    int hypo_y = newy + DIRS[check_dir][1];
                    board[hypo_x][hypo_y] = player_idx + 1;
                    for(int i = 0; i < num_players; i++){
                        if(i != player_idx){
                            score -= enemy_reachable(new_positions[i], i);
                        }
                        // score -= enemy_reachable()
                    }

                    int mdist = max_distance_to_block(hypo_x, hypo_y);

                    board[hypo_x][hypo_y] = 0;

                    
                    // score -= pow(mdist,DIST_POWER) ;
                    score += mdist * WALL_DIST_MULTIPLIER;

                    cerr << "score: " << score << endl;
                    if(score > max_reachable){
                        max_reachable = score;
                        ret_idx = check_dir;
                    }
                    // if(board[newx + DIRS[check_dir][0]][newy + DIRS[check_dir][1]] == 0){
                    // ret_idx =  check_dir;
                }
            }
        }
        // currx = newx;// + DIRS[check_dir][0];
        // curry = newy;// + DIRS[check_dir][1];
        for(int i = 0; i < num_players; i++){
            positions[i] = new_positions[i];
        }

        if(ret_idx == -1){
            ret_idx = 0;
        }

        cerr << "Expected next square = " << (int)currx + DIRS[ret_idx][0] << " " << (int)curry + DIRS[ret_idx][1] << endl;
        return DIR_STRINGS[ret_idx];




    }



    int num_players;
    int player_idx;

    bool initialized = false;
    // int currx;
    // int curry;
    // vector<pair
    vector<pair<int,int>> positions;

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
        // cerr << state.currx << " " << state.curry << endl;
        cerr << player_states[p][0] << " " << player_states[p][1] << " " << player_states[p][2] << " " << player_states[p][3] << endl;
        cerr << p << endl;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << move << endl; // A single line with UP, DOWN, LEFT or RIGHT
    }

}