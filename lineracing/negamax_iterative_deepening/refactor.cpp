#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include<queue>
#include<set>
#include<time.h>
#include<exception>

#define BX 30
#define BY 20

#define PLAY_CLOCK 100
#define TIME_SLACK 25

#define MIN_HEURISTIC -10000
#define MAX_HEURISTIC 10000

#define ERROR_CHECKS 0

int DIRS[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

using namespace std;

vector<string> DIR_STRINGS = {"UP", "RIGHT", "DOWN", "LEFT"};


template <class T>
bool el_in_set(set<T> s, T e){
    return s.find(e) != s.end();
}

int timer;

class TimeOutException : public exception {
    virtual const char* what() const throw() {
        return "TimeOutException";
    }
} timeOutException;

double time_elapsed(){
    return (double) ( 1000 *(clock() - timer)) / CLOCKS_PER_SEC;
}

bool time_out(){
    return (time_elapsed() + TIME_SLACK) > PLAY_CLOCK;
}



class State{
public:

    vector<vector<pair<int, int>>> move_sequences;
    char board[BX][BY];
    vector<pair<int,int>> positions;
    int num_players;
    set<int> dead;

    State(int num_players, vector<pair<int, int>> positions){
        this->positions = positions;
        this->num_players = num_players;
        // this->move_sequences = vector<vector<pair<int, int>>>(num_players);
    }

    State(int num_players, vector<pair<int, int>> positions, char board[BX][BY]){
        for(auto pos: positions){
            this->positions.push_back(pos);
        }
        this->num_players = num_players;
        for(int i = 0; i < BX; i++){
            for(int j = 0; j < BY; j++){
                this->board[i][j] = board[i][j];
            }
        }
    }

    State move_applied(pair<int, int> move,int player_idx){

        // State s = State(this->num_players, this->positions, this->board);
        State s = this->copy();
        s.apply_move(move, player_idx);
        return s;
    }

    State copy(){
        State s(num_players, positions, board);
        for(auto p : dead){
            s.dead.insert(p);
        }
        return s;
    }

    bool in_bounds(pair<int, int> move){
        int x = move.first, y = move.second;

        return in_bounds(x, y);
    }


    bool in_bounds(int x, int y){
        return (
            0 <= x && x < BX && 0 <= y && y < BY
        );
    }
    bool suicide_move(pair<int,int> move){
        // cerr << "Sucide move made" << endl;
        if(!in_bounds(move)){
            return true;
        }
        else {
            return board[move.first][move.second] != 0;
        }
    }

    bool suicide_move(int x, int y){
        return suicide_move(make_pair(x, y));
    }


    int num_reachable(pair<int, int> pos){

        int x = pos.first, y = pos.second;
        // Take the max count over all directions we can move in

        int count = 0;
        queue<pair<int, int>> q;

        // run dfs to see how many squares are reachable
        // q.push_back(make_pair(nx, ny));
        q.push(make_pair(x, y));
        set<pair<int, int>> visited;
        visited.insert(make_pair(x, y));
        
        pair<int, int> c_pos;
        while(q.size() > 0){
            
            c_pos = q.front();
            int cur_x = c_pos.first;
            int cur_y = c_pos.second;
            q.pop();

            // if(board[cur_x][cur_y] == 0 && visited.find(pos) == visited.end()){
            //     count++;
            // }

            for(int i = 0; i < 4; i++){
                int nx2 = cur_x + DIRS[i][0];
                int ny2 = cur_y + DIRS[i][1];
                if(!suicide_move(nx2, ny2) && visited.find(make_pair(nx2, ny2)) == visited.end() ) {
                    q.push(make_pair(nx2, ny2));
                    visited.insert(make_pair(nx2, ny2));
                    count++;
                }
            }
        }
        // cerr << "Count: " << count << endl;
        return count;
    }

    vector<int> num_reachable2(){
        // simultaneous bfs

        vector<queue<pair<int, int>>> qs(num_players);
        set<pair<int, int>> visited;
        vector<int> counts(num_players);

        for(int i = 0; i < num_players; i++){
            if(!is_dead(i)){
                qs[i].push(positions[i]);
                visited.insert(positions[i]);
                counts[i] = 0;
            }
        }

        bool all_empty = false;


        while(!all_empty){
            if(time_out()){
                throw timeOutException;
            }
            all_empty = true;
            for(int i = 0; i < num_players; i++){
                if(qs[i].size() > 0){
                    all_empty = false;
                    pair<int, int> c_pos = qs[i].front();
                    int cur_x = c_pos.first;
                    int cur_y = c_pos.second;
                    qs[i].pop();

                    for(int j = 0; j < 4; j++){
                        int nx = cur_x + DIRS[j][0];
                        int ny = cur_y + DIRS[j][1];
                        if(!suicide_move(nx, ny) && visited.find(make_pair(nx, ny)) == visited.end()){
                            auto p = make_pair(nx, ny);
                            qs[i].push(p);
                            visited.insert(p);
                            counts[i] ++;
                        }
                    }
                }
            }
        }


        return counts;

    }



    void apply_move(pair<int, int> move,int player_idx){

        if(move.first == -1 || move.second == -1){
            if(!is_dead(player_idx)){
                this->kill(player_idx);
            }
            return;
        }
        if(is_dead(player_idx)){
            return;
        }

        if(el_in_set(dead, player_idx)){
            throw runtime_error("Agent already dead");
        }

        if(suicide_move(move)){
            cerr << "Suicide move made" << endl;
            this->kill(player_idx);
        } else {
            this->positions[player_idx] = move;
            this->board[move.first][move.second] = player_idx + 1;
        }
    }


    void kill(int player_idx){
        // if(ERROR_CHECKS){
        if(el_in_set(dead, player_idx)){
            throw runtime_error("Already dead");
        }
        dead.insert(player_idx);
        for(int i = 0; i < BX; i++){
            for(int j = 0; j < BY; j++){
                if(board[i][j] == player_idx + 1){
                    board[i][j] = 0;
                }
            }
        }
    }


    bool is_allowed(int x, int y, int dir){
        return !suicide_move(x + DIRS[dir][0], y + DIRS[dir][1]);
    }


    pair<bool, vector<int>> is_terminal(){
        if(dead.size() >= num_players - 1){
            vector<int> ret_vect = vector<int>(num_players);
            for(int i = 0; i < num_players; i++){
                if(is_dead(i)){
                    ret_vect[i] = MIN_HEURISTIC;
                } else {
                    ret_vect[i] = MAX_HEURISTIC;
                }
            }
            if(ERROR_CHECKS){
                for(auto i: ret_vect){
                    if(abs(i) >  MAX_HEURISTIC){
                        cerr << "Invalid heuristic: terminal heuristic" << endl;
                        throw runtime_error("Invalid heuristic");
                    }
                }
            }
            return make_pair(true, ret_vect);
        } else {
            return make_pair(false, vector<int>(num_players, 0));
        }
    }

    bool is_dead(int player_idx){
        return el_in_set(dead, player_idx);
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

    vector<pair<int, int>> get_available_moves(int player_idx){
        if(is_dead(player_idx)){
            return vector<pair<int, int>>();
        }
        vector<pair<int, int>> moves;
        for(int i = 0; i < 4; i++){
            int nx = positions[player_idx].first + DIRS[i][0];
            int ny = positions[player_idx].second + DIRS[i][1];
            if(!suicide_move(nx, ny)){
                moves.push_back(make_pair(nx, ny));
            }
        }
        return moves;
    }


    void fill_square(pair<int, int> pos, int player_idx){
        if(pos.first == -1 || pos.second == -1){
            return;
        }
        this->board[pos.first][pos.second] = player_idx + 1;
    }


};

class Heuristic{
public:
    // int max_h = 1000;
    // int min_h= -1000;

    // vector<int> evaluate(State& state){
    //     vector<int> vec(state.num_players, 0);
    //     vector<int> reachable(state.num_players, 0);
    //     for(int i = 0; i < state.num_players; i++){
    //         if(!state.is_dead(i)){
    //             reachable[i] = state.num_reachable(state.positions[i]);
    //         } else {
    //             reachable[i] = 0;
    //         }
    //     }

    //     // cerr << "Reachable: ";
    //     // for(int i = 0; i < state.num_players; i++){
    //     //     cerr << reachable[i] << " ";
    //     // }

    //     // cerr << "Reachable: ";
    //     for(int i = 0; i < state.num_players; i++){
    //         if(!state.is_dead(i)){
    //             for(int j = 0; j < state.num_players; j++){
    //                 if(i != j && !state.is_dead(j)){
    //                     vec[i] -= reachable[j];
    //                 } else if (i == j){
    //                     vec[i] += reachable[j];
    //                 }
    //             }
    //             if(abs(vec[i]) > MAX_HEURISTIC){
    //                 cerr << "Invalid heuristic : reachable heuristic" << endl;
    //                 throw runtime_error("Invalid heuristic");
    //             }
    //         } else {
    //             vec[i] = MIN_HEURISTIC;
    //         }
    //         // cerr << reachable[i] << " ";
    //     }
    //     // cerr << endl;

    //     return vec;

    // }

    vector<int> evaluate(State& state){
        vector<int> ret = state.num_reachable2();
        for(auto &p: state.dead){
            ret[p] = MIN_HEURISTIC;
        }

        return ret;
    }

    int min_heuristic(){
        return MIN_HEURISTIC;
    }

    int max_heuristic(){
        return MAX_HEURISTIC;
    }
};


class Agent{

public:
    int num_players;
    State * state;
    Heuristic *h;


    Agent(){

    }

    ~Agent(){
        delete state;
        delete h;
    }

    void initialize(int num_players, vector<pair<int, int>>& positions){
        this->num_players = num_players;
        this->state = new State(num_players, positions);
        this->h = new Heuristic();
    }

    // void fill_squares(vector<pair<int, int>> positions){
        // this->state->fill_positions(positions);
    // }

    void update(vector<vector<int>> states){
        for(int i = 0; i < states.size(); i ++ ){
            this->state->apply_move(pair<int,int>(states[i][2], states[i][3]), i);
        }
        for(int i = 0; i < states.size(); i ++){
            this->state->fill_square(pair<int, int>(states[i][0], states[i][1]), i);
        }
    }

    string move_str(pair<int, int> move, int player_idx){
        int mvx = move.first, mvy = move.second;
        int posx = this->state->positions[player_idx].first, posy = this->state->positions[player_idx].second;

        int offsetx = mvx - posx;
        int offsety = mvy - posy;
        

        for(int i = 0; i < 4; i++){
            if(offsetx == DIRS[i][0] && offsety == DIRS[i][1]){
                return DIR_STRINGS[i];
            }
        }
        cerr << "Invalid move" << endl;
        throw runtime_error("Invalid move");
    }

    pair<int, int> null_move(){
        return make_pair(-1, -1);
    }

    pair<pair<int, int>,vector<int>> minimax_mult(State s, int depth, int player_idx, int main_player){
        // cout << "Depth: " << depth << endl;
        // cout << "Player: " << player_idx << endl;
        // s.print_board();
        if(time_out()){
            throw timeOutException;
        }

        // cerr << __LINE__ << endl;
        pair<bool, vector<int>> term = s.is_terminal();
        if(term.first){
            for(auto v: term.second){
                if(abs(v) > MAX_HEURISTIC){
                    cerr << "Invalid heuristic: terminal heuristic" << endl;
                    throw runtime_error("Invalid heuristic");
                }
            }
            return pair<pair<int, int>,vector<int>>(null_move(), term.second);
        }

        if(s.is_dead(main_player)){
            vector<int> ret_vect = vector<int>(s.num_players);
            for(int i = 0; i < s.num_players; i++){
                if(s.is_dead(i)){
                    ret_vect[i] = MIN_HEURISTIC;
                } else {
                    ret_vect[i] = MAX_HEURISTIC;
                }
                if(abs(ret_vect[i]) > MAX_HEURISTIC){
                    cerr << "Invalid heuristic : dead heuristic" << endl;
                    throw runtime_error("Invalid heuristic");
                }
            }

            return pair<pair<int, int>,vector<int>>(null_move(), ret_vect);
        }

        // cerr << __LINE__ << endl;
        if(depth == 0){
            // cerr << __LINE__ << endl;
            if(time_out()){
                throw timeOutException;
            }
            auto evaluated = h->evaluate(s);
            // for(auto v : evaluated){
            //     if(abs(v) > MAX_HEURISTIC){
            //         cerr << "Invalid heuristic : evaluate heuristic" << endl;
            //         throw runtime_error("Invalid heuristic");
            //     }
            // }
            return pair<pair<int, int>, vector<int>>(null_move(), evaluated);
        }

        // cerr << __LINE__ << endl;
        if(s.is_dead(player_idx)){
            return minimax_mult(s, depth - 1, (player_idx + 1) % num_players, main_player);
        }


        // cerr << __LINE__ << endl;
        vector<pair<int, int>> moves = s.get_available_moves(player_idx);
        if(moves.size() == 0){
            cerr << "Found no moves" << endl;
            s.kill(player_idx);
            return minimax_mult(s, depth - 1, (player_idx + 1) % num_players, main_player);
        }


        // cerr << __LINE__ << endl;
        int value = MIN_HEURISTIC - 1;
        vector<int> value_vec = vector<int>(moves.size());
        pair<int, int> best_move;
        best_move = moves[0];
        // cerr << __LINE__ << endl;
        for(auto &move: moves){
            pair<pair<int, int>,vector<int>> result = minimax_mult(
                s.move_applied(move, player_idx), 
                depth - 1, 
                (player_idx + 1) % num_players,
                main_player
            );
            if(result.second[player_idx] > value){
                value = result.second[player_idx];
                value_vec = result.second;
                best_move = move;
            }
        }
        for(auto v: value_vec){
            if(abs(v) > MAX_HEURISTIC){
                cerr << "Invalid heuristic : minimax_mult heuristic" << endl;
                throw runtime_error("Invalid heuristic");
            }
        }

        return pair<pair<int, int>,vector<int>>(best_move, value_vec);
    }

    string get_move(int player_idx){
        string best_move = "";

        // timer = clock();
        int depth = 1;

        // bool found = false;
        string bst = "";
        while(1){
            try{
                pair<pair<int, int>,vector<int>> result = minimax_mult(state->copy(), depth, player_idx, player_idx);
                cerr << "Depth: " << depth << " " << result.first.first << " " << result.first.second << endl;
                cerr << "Values: ";
                for(int i = 0; i < num_players; i++){
                    cerr << result.second[i] << " ";
                    if(ERROR_CHECKS){
                        if(result.second[i] > MAX_HEURISTIC){
                            state->print_board();
                            cerr << "invalid heuristic" << endl;
                            throw "Invalid Heuristic";
                        }
                    }
                }
            
                cerr << endl;
                cerr << "Dead: " << state->dead.size() << endl;

                best_move = move_str(result.first, player_idx);

                if(abs(result.second[player_idx]) == h->max_heuristic()){
                    break;
                }

                depth++;
            }
            catch(TimeOutException){
                break;
            }

        }
        // bst = best_move;
        // if(!found){
            // best_move = "UP";
        // }
        return best_move;
    }
};



int main(){
    bool initialized = false;
    Agent agent = Agent();

    // game loop
    while (1) {
        int n; // total number of players (2 to 4).
        int p; // your player number (0 to 3).

        // cerr << __LINE__ << endl;
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
            cerr << "Player " << i << ": " << player_state[0] << " " << player_state[1] << " " << player_state[2] << " " << player_state[3] << endl;
        }

        cerr << n << " " << p << endl;


        // cerr << __LINE__ << endl;
        timer = clock();

        if(!initialized){
            vector<pair<int, int>> positions;
            vector<pair<int, int>> initial_pos;
            for(int i = 0; i < n; i++){
                initial_pos.push_back(make_pair(player_states[i][0], player_states[i][1]));
                positions.push_back(make_pair(player_states[i][2], player_states[i][3]));
            }
            
            agent.initialize(n, positions);
            // agent.fill_squares(initial_pos);
            initialized = true;
        }
            agent.update(player_states);

        // agent.fill_squares(initial_pos);
        // cerr << __LINE__ << endl;
        string move = agent.get_move(p);
        // cerr << __LINE__ << endl;

        agent.state->print_board();
        // cerr << state.currx << " " << state.curry << endl;
        cerr << player_states[p][0] << " " << player_states[p][1] << " " << player_states[p][2] << " " << player_states[p][3] << endl;
        cerr << p << endl;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << move << endl; // A single line with UP, DOWN, LEFT or RIGHT
        // exit(0);
    }
}