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
#define TIME_SLACK 5

#define MIN_HEURISTIC -1000
#define MAX_HEURISTIC 1000

int DIRS[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

using namespace std;

vector<string> DIR_STRINGS = {"UP", "RIGHT", "DOWN", "LEFT"};
// vector<string> DIR_STRINGS = {"DOWN", "RIGHT", "UP", "LEFT"};
// vector<string> DIR_STRINGS = {"DOWN", "LEFT", "UP", "RIGHT"};
// vector<string> DIR_STRINGS = {"RIGHT", "UP", "LEFT", "DOWN"};

template <class T>
bool el_in_set(set<T> s, T e){
    return s.find(e) != s.end();
}


class TimeOutException : public exception {
    virtual const char* what() const throw() {
        return "TimeOutException";
    }
} timeOutException;



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
        this->positions = positions;
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
        // int dirx = DIRS[dir][0];
        // int diry = DIRS[dir][1];
        // int nx = x + dirx;
        // int ny = y + diry;
        int count = 0;

        queue<pair<int, int>> q;

        // run dfs to see how many squares are reachable
        // q.push_back(make_pair(nx, ny));
        q.push(make_pair(x, y));
        set<pair<int, int>> visited;
        
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
                if(!suicide_move(nx2, ny2) && visited.find(make_pair(nx2, ny2)) == visited.end() && (nx2 != x || ny2 != y)) {
                    q.push(make_pair(nx2, ny2));
                    visited.insert(make_pair(nx2, ny2));
                    count++;
                }
            }
        }
        // cerr << "Count: " << count << endl;
        return count;
    }



    void apply_move(pair<int, int> move,int player_idx){
        if(el_in_set(dead, player_idx)){
            throw runtime_error("Agent already dead");
        }
        if(move.first == -1 && move.second == -1){
            return;
        }
        if(suicide_move(move)){
            cerr << "Sucide move made" << endl;
            this->kill(player_idx);
        } else {
            this->positions[player_idx] = move;
            this->board[move.first][move.second] = player_idx + 1;
        }
    }


    void kill(int player_idx){
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

    State copy(){
        State s(num_players, positions);
        for(int i = 0; i < BX; i++){
            for(int j = 0; j < BY; j++){
                s.board[i][j] = board[i][j];
            }
        }

        for(auto p : dead){
            s.dead.insert(p);
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


    pair<bool, vector<int>> is_terminal(){
        if(dead.size() == num_players - 1){
            vector<int> ret_vect = vector<int>(num_players, MIN_HEURISTIC);
            for(int i = 0; i < num_players; i++){
                if(!el_in_set(dead, i)){
                    ret_vect[i] = MAX_HEURISTIC;
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
            if(is_allowed(positions[player_idx].first, positions[player_idx].second, i)){
                moves.push_back(make_pair(positions[player_idx].first + DIRS[i][0], positions[player_idx].second + DIRS[i][1]));
            }
        }
        return moves;
    }


};

class Heuristic{
public:
    // int max_h = 1000;
    // int min_h= -1000;

    vector<int> evaluate(State& state){
        vector<int> vec(state.num_players, 0);
        vector<int> reachable(state.num_players, 0);
        for(int i = 0; i < state.num_players; i++){
            if(!state.is_dead(i)){
                reachable[i] = state.num_reachable(state.positions[i]);
            }
        }

        // cerr << "Reachable: ";
        // for(int i = 0; i < state.num_players; i++){
        //     cerr << reachable[i] << " ";
        // }

        for(int i = 0; i < state.num_players; i++){
            if(!state.is_dead(i)){
                for(int j = 0; j < state.num_players; j++){
                    if(i != j && !state.is_dead(j)){
                        vec[i] -= reachable[j];
                    } else if (i == j){
                        vec[i] += reachable[j];
                    }
                }
            } else {
                vec[i] = MIN_HEURISTIC;
            }
        }

        return vec;

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

    int timer;

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

    void update(vector<vector<int>> states){
        for(int i = 0; i < states.size(); i ++ ){
            this->state->apply_move(pair<int,int>(states[i][2], states[i][3]), i);
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
        throw runtime_error("Invalid move");
    }

    pair<int, int> null_move(){
        return make_pair(-1, -1);
    }

    double time_elapsed(){
        return (double) ( 1000 *(clock() - timer)) / CLOCKS_PER_SEC;
    }

    bool time_out(){
        return (time_elapsed() - TIME_SLACK) > PLAY_CLOCK;
    }

    pair<pair<int, int>,vector<int>> minimax_mult(State state, int depth, int player_idx, int main_player){

        if(time_out()){
            throw timeOutException;
        }

        // cerr << __LINE__ << endl;
        pair<bool, vector<int>> term = state.is_terminal();
        if(term.first){
            return pair<pair<int, int>,vector<int>>(null_move(), term.second);
        }

        // cerr << __LINE__ << endl;
        if(depth == 0 || state.is_dead(main_player)){
            // cerr << __LINE__ << endl;
            if(time_out()){
                throw timeOutException;
            }
            return pair<pair<int, int>, vector<int>>(null_move(), h->evaluate(state));
        }

        // cerr << __LINE__ << endl;
        if(state.is_dead(player_idx)){
            return minimax_mult(state, depth, (player_idx + 1) % num_players, main_player);
        }


        // cerr << __LINE__ << endl;
        vector<pair<int, int>> moves = state.get_available_moves(player_idx);
        if(moves.size() == 0){
            state.kill(player_idx);
            return minimax_mult(state, depth, (player_idx + 1) % num_players, main_player);
        }

        // cerr << __LINE__ << endl;
        int value = h->min_heuristic();
        vector<int> value_vec = vector<int>(moves.size(), value);
        pair<int, int> best_move;
        best_move = moves[0];
        // cerr << __LINE__ << endl;
        for(auto &move: moves){
            pair<pair<int, int>,vector<int>> result = minimax_mult(
                state.move_applied(move, player_idx), 
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

        return pair<pair<int, int>,vector<int>>(best_move, value_vec);
    }

    string get_move(int player_idx){
        string best_move = "";

        timer = clock();
        int depth = 1;

        while(1){
            // cerr << "Depth: " << depth << endl;
            try{
                // cerr << __LINE__ << endl;
                pair<pair<int, int>,vector<int>> result = minimax_mult(state->copy(), depth, player_idx, player_idx);
                // cerr << __LINE__ << endl;
                cerr << "Depth: " << depth << " " << result.first.first << " " << result.first.second << endl;
                cerr << "Values: ";
                for(int i = 0; i < num_players; i++){
                    cerr << result.second[i] << " ";
                }
                cerr << endl;
                cerr << "Dead: " << state->dead.size() << endl;

                try{
                    best_move = move_str(result.first, player_idx);
                }catch(...){
                    if(best_move == ""){
                        best_move = "UP";
                    }
                }
                if(abs(result.second[player_idx]) == h->max_heuristic()){
                    break;
                }

                depth++;
            }
            catch(TimeOutException){
                break;
            }
        }
        if(best_move == ""){
            best_move = "UP";
        }
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
        }

        // cerr << __LINE__ << endl;

        if(!initialized){
            vector<pair<int, int>> positions;
            for(int i = 0; i < n; i++){
                positions.push_back(make_pair(player_states[i][0], player_states[i][1]));
            }
            agent.initialize(n, positions);
            initialized = true;
        }

        agent.update(player_states);
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
    }
}