#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include<queue>
#include<set>
#include<time.h>
#include<exception>
#include<unordered_set>

#define BX 30
#define BY 20

// We throw an exception when elapsed + time_slack is more than play clock
#define PLAY_CLOCK 100.
#define TIME_SLACK 5.

#define MIN_HEURISTIC -10000
#define MAX_HEURISTIC 10000

#define SHOW_LINES 0

#define ERROR_CHECKS 0

struct int_pair_hash
{
    std::size_t operator () (std::pair<int, int> const &v) const
    {
        return v.first * 31 + v.second;
    }
};


using namespace std;

// The direction mappings
int DIRS[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
vector<string> DIR_STRINGS = {"UP", "RIGHT", "DOWN", "LEFT"};

// placeholder for an invalid move for dying
pair<int, int> die_move(){
    return make_pair(-3, -3);
}

// Placeholder move for passing in the negamax function
pair<int, int> null_move(){
    return make_pair(-5, -5);
}

// Check if an element is present in a set
template <class T>
bool el_in_set(set<T> s, T e){
    return s.find(e) != s.end();
}

// Global timer so we can throw a TimeOutException anywhere in the program

int timer;
// The exception we throw when we run out of time
class TimeOutException : public exception {
    virtual const char* what() const throw() {
        return "TimeOutException";
    }
} timeOutException;

// time elapsed in ms since we started the timer
double time_elapsed(int t){
    return (double) ( 1000 *(clock() - t)) / CLOCKS_PER_SEC;
}

// Check if we have timed out
bool time_out(int t){
    return (time_elapsed(t) + TIME_SLACK) > PLAY_CLOCK;
}

void check_time(int t){
    if (time_out(t)){
        throw timeOutException;
    }
}



class State{
public:

    // The moves of each agent, so we can undo moves
    vector<vector<pair<int, int>>> move_sequences;

    // The board
    char board[BX][BY];

    // The current positions of the agents
    vector<pair<int,int>> positions;

    // The number of agents
    int num_players;

    // Set of dead agents
    set<int> dead;

    State(int num_players, vector<vector<int>> &player_states){
        // Constructor we use for initial state

        // Initialize num players, positions, and move sequences
        this-> num_players = num_players;
        this->positions = vector<pair<int, int>>();
        this->move_sequences = vector<vector<pair<int, int>>>(num_players);

        // go throuh each player's state
        for(int i = 0; i < num_players; i++){
            // Initialize vector in move_sequences
            this->move_sequences[i] = vector<pair<int, int>>();

            // Get coordinates
            int fst_x = player_states[i][0];
            int fst_y = player_states[i][1];
            int snd_x = player_states[i][2];
            int snd_y = player_states[i][3];

            // Add first coordinate to the move sequence
            this->move_sequences[i].push_back(make_pair(fst_x, fst_y));

            // If the second coordinate is different, add it to the move sequence
            if(fst_x != snd_x || fst_y != snd_y){
                this->move_sequences[i].push_back(make_pair(snd_x, snd_y));
            }

            // Set the position to the second coordinate
            this->positions.push_back(make_pair(snd_x, snd_y));
            

            // Also initialize the board
            board[fst_x][fst_y] = i + 1;
            board[snd_x][snd_y] = i + 1;
        }

    }

    State(int num_players, vector<pair<int, int>> &positions){
        // constructor we use internally to copy the state and such

        // initialize positions, num players, and move sequences
        this->positions = vector<pair<int, int>>();
        this->num_players = num_players;
        this->move_sequences = vector<vector<pair<int, int>>>(num_players);

        // Copy positions
        for(int i = 0; i < num_players; i++){
            this->move_sequences[i] = vector<pair<int, int>>();
        }
        for(auto pos: positions){
            this->positions.push_back(pos);
        }
    }

    State(int num_players, vector<pair<int, int>> &positions, char board[BX][BY]): State(num_players, positions){
        // Another internally used constructor that also copies the board

        for(int i = 0; i < BX; i++){
            for(int j = 0; j < BY; j++){
                this->board[i][j] = board[i][j];
            }
        }
    }


    State copy(){
        // Copy a state

        // Initialize state with the number of players and positions, and the board
        State s(num_players, positions, board);

        // Copy the dead set
        for(auto p : dead){
            s.dead.insert(p);
        }

        // copy move sequence
        for(int i = 0; i < num_players; i++){
            for(auto m : move_sequences[i]){
                s.move_sequences[i].push_back(m);
            }
        }
        return s;
    }

    bool in_bounds(pair<int, int> move){
        // Check if a move is in bounds of the board

        int x = move.first, y = move.second;
        return in_bounds(x, y);
    }


    bool in_bounds(int x, int y){
        // Check if a move is in bounds of the board
        return (
            0 <= x && x < BX && 0 <= y && y < BY
        );
    }

    bool suicide_move(pair<int,int> move){
        // Move is illegal if it is out of bounds, or the square is not free
        if(!in_bounds(move)){
            return true;
        }
        return board[move.first][move.second] != 0;
    }

    bool suicide_move(int x, int y){
        return suicide_move(make_pair(x, y));
    }


    int num_reachable(pair<int, int> pos){
        // Number of reachable squares for an agent
        // Not really used, but we keep it around

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
        // simultaneous bfs from all the agent's positions
        // No two agents can claim the same square

        // Queues for each agent's bfs
        vector<queue<pair<int, int>>> qs(num_players);

        // We just need one set of visited squares
        unordered_set<pair<int, int>, int_pair_hash> visited;

        // Number of reachable squares for each agent
        vector<int> counts(num_players);

        // Initialize the queues with the agent's current position
        // If they aren not dead
        for(int i = 0; i < num_players; i++){
            if(!is_dead(i)){
                qs[i].push(positions[i]);

                // Also initialize the count vector and reached set
                visited.insert(positions[i]);
                counts[i] = 0;
            }
        }

        // Terminate when all queues are empty
        bool all_empty = false;


        while(!all_empty){
            // Check for time out
            // if(time_out()){
            //     throw timeOutException;
            // }

            all_empty = true;

            // Go through each player
            for(int i = 0; i < num_players; i++){
                // If an agent is dead, this will never return true, as the queue initialized empty
                if(qs[i].size() > 0){
                    all_empty = false;

                    // pop from queue
                    pair<int, int> c_pos = qs[i].front();
                    int cur_x = c_pos.first;
                    int cur_y = c_pos.second;
                    qs[i].pop();

                    // Add squares in all directions
                    for(int j = 0; j < 4; j++){

                        int nx = cur_x + DIRS[j][0];
                        int ny = cur_y + DIRS[j][1];

                        // The move needs to be valid, and not in visited
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
        // Apply a move by an agent to the state

        // If the player is dead, there is nothing to do
        if(is_dead(player_idx)){
            return;
        }

        // Check if the move is an invalid move
        if(move == die_move()){
            this->kill(player_idx); 
            return;
        }

        if(move.first == -1 || move.second == -1){
            // The game might be telling us that another agent just died
            // So if it is not dead, we kill it

            if(!is_dead(player_idx)){
                this->kill(player_idx);
            }
        
            cerr << "Weird move made" << endl;
            return;
        }
        

        if(suicide_move(move)){

            // If the move is invalid, we give a warning, and then kill it
            cerr << "Suicide move made" << endl;

            this->kill(player_idx);

        } else {

            // Actually perform a regular move

            // Set the agent's new position
            this->positions[player_idx] = move;

            // Set the new square to the player's id
            this->board[move.first][move.second] = player_idx + 1;

            // for(int i = 0; i < this->num_players; i++){
                // cerr << this-> move_sequences[i].size() << " ";
            //
            // cerr << endl;
            // Add the move to the move sequence
            this->move_sequences[player_idx].push_back(move);
        }
    }

    void revive(int player_idx){
        // Revive a dead player (to undo a move)

        // Add all the positions of the player into the board
        for(auto &p : this->move_sequences[player_idx]){
            if(p == die_move()){
                break;
            }
            this->board[p.first][p.second] = player_idx + 1;
        }

        // Delete the die move from the end of the move sequence
        this->move_sequences[player_idx].pop_back();

        // Remove the player from the dead set
        this->dead.erase(player_idx);
    }

    void undo_move(int player_idx){
        // Undo the last move made by an agent

        // Get the last move of the agent
        auto last_move = this->move_sequences[player_idx].back();

        auto d_move = die_move();

        if(last_move == d_move && !is_dead(player_idx)){
            // This should not happen, so we throw an exception
            cerr << "Last move a die move but player is not dead" << endl;
            throw runtime_error("Last move a die move but player is not dead");
        }

        if(last_move != d_move && is_dead(player_idx)){
            // This should not happen, so we throw an exception
            cerr << "Last move is not a die move but player is dead" << endl;
            throw runtime_error("Last move is not a die move but player is dead");
        }

        // If the agent died in its last move, we revive it
        if(last_move == d_move && is_dead(player_idx)){
            this->revive(player_idx);

        } else {
            // Otherwise, we remove the last move from the move sequence
            // And then clear the square from the board

            // Remove the last move from the board
            this->board[last_move.first][last_move.second] = 0;

            // Remove the move from the move sequence
            this->move_sequences[player_idx].pop_back();

            // set the agent's position to the last move
            this->positions[player_idx] = this->move_sequences[player_idx].back();
        }
    }


    void kill(int player_idx){

        // If the agent is dead, we shouldnt be calling this function
        if(el_in_set(dead, player_idx)){
            throw runtime_error("Already dead");
        }

        // Insert the player into the dead set
        dead.insert(player_idx);

        // Insert a dead move into the move sequence
        move_sequences[player_idx].push_back(die_move());

        // Remove the agent from the board
        // TODO: check if we can simply go through the move sequence instead
        // for(int i = 0; i < BX; i++){
            // for(int j = 0; j < BY; j++){
                // if(board[i][j] == player_idx + 1){
                    // board[i][j] = 0;
                // }
            // }
        // }

        for(auto &p : move_sequences[player_idx]){
            if(p == die_move()){
                break;
            }
            board[p.first][p.second] = 0;
        }
    }


    // bool is_allowed(int x, int y, int dir){
        
    //     return !suicide_move(x + DIRS[dir][0], y + DIRS[dir][1]);
    // }


    pair<bool, vector<int>> is_terminal(){
        // Is the state a terminal state?
        // This only happens when there is only one player left alive.

        // The first element in the pair is whether the state is terminal
        // The second element is heuristic values for each player
        // MIN_HEURISTIC for each losing player and MAX_HEURISTIC for each winning player

        check_time(timer);
        if(dead.size() >= num_players - 1){
            // If there is only one player left alive, we return true


            // Get the heuristic value
            vector<int> ret_vect = vector<int>(num_players);

            // Go through each player and see if they are dead
            for(int i = 0; i < num_players; i++){
                if(is_dead(i)){
                    ret_vect[i] = MIN_HEURISTIC;
                } else {
                    ret_vect[i] = MAX_HEURISTIC;
                }
            }
            
            // Return true along with the heuristic values
            return make_pair(true, ret_vect);

        } else {
            // Otherwise we return false
            return make_pair(false, vector<int>(num_players, 0));
        }
    }

    bool is_dead(int player_idx){
        // Simply check whether the player is in the dead set
        return el_in_set(dead, player_idx);
    }

    void print_board(){
        // print the board to cerr
        for(int i = 0; i < BX; i++){
            for(int j = 0; j < BY; j++){
                cerr <<(int) board[i][j];
            }
            cerr << endl;
        }
        cerr << endl;
    }

    vector<pair<int, int>> get_available_moves(int player_idx){
        // Get all the squares a player can move to


        if(is_dead(player_idx)){
            // This should not happen
            cerr << "Finding moves for dead player" << endl;
            throw runtime_error("Finding moves for dead player");
        }

        // Vector of moves
        vector<pair<int, int>> moves;

        // Go through each direction
        for(int i = 0; i < 4; i++){

            // Get the next square
            int nx = positions[player_idx].first + DIRS[i][0];
            int ny = positions[player_idx].second + DIRS[i][1];

            // If the move is not a suicide move, it is legal, and we add it
            if(!suicide_move(nx, ny)){
                moves.push_back(make_pair(nx, ny));
            }
        }

        // If there are no moves, we add the die move

        if(moves.size() == 0){
            moves.push_back(die_move());
        }

        return moves;
    }

};

class Heuristic{
public:
    vector<int> evaluate(State& state){
        // Get the reachable squares for each player
        vector<int> ret = state.num_reachable2();

        // Each dead player gets the minimum heuristic
        for(auto p: state.dead){
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
        // Agent is initialized in the initialize function
    }

    ~Agent(){
        delete state;
        delete h;
    }

    void initialize(int num_players, vector<vector<int>> &states){
        // Initialize the agent - make state and heuristic
        this->num_players = num_players;
        this->state = new State(num_players, states);
        this->h = new Heuristic();
    }


    void update(vector<vector<int>> &states){
        // Given the states in an iteration, update the agent's state

        // Simply apply all the moves
        for(int i = 0; i < states.size(); i ++ ){
            this->state->apply_move(pair<int,int>(states[i][2], states[i][3]), i);
        }
    }

    string move_str(pair<int, int> move, int player_idx){
        // Turn a square that a player is moving to into a string

        // If the move is a die move, we just lost the game. 
        // Instead of returning some invalid output though, we simply return up
        if(move == die_move()){
            return "UP";
        }

        // get coordinates
        int mvx = move.first, mvy = move.second;

        // Get the current position of the agent
        int posx = this->state->positions[player_idx].first, posy = this->state->positions[player_idx].second;

        // Get the offset of the move and the current position
        int offsetx = mvx - posx;
        int offsety = mvy - posy;
        

        // Find the move corresponding to the offset
        for(int i = 0; i < 4; i++){
            if(offsetx == DIRS[i][0] && offsety == DIRS[i][1]){
                // Return the string corresponding to the move
                return DIR_STRINGS[i];
            }
        }

        // We should not get here. The move must either be a valid move or a die move
        cerr << "Invalid move. Player:  " 
             << player_idx  
             << " moving from "
             << state->positions[player_idx].first 
             <<" " 
             << state->positions[player_idx].second
             << " to "
             << move.first << " " << move.second << endl;
        cerr << "On board" << endl;
        this->state->print_board();

        throw runtime_error("Invalid move");
    }



    pair<pair<int, int>,vector<int>> minimax_mult(State &s, int depth, int player_idx, int main_player){
        // Minimax search for the best move

        check_time(timer);

        // We first check if the state is terminal
        // If it is, the state gives us our heuristic
        // And we return a null move
        pair<bool, vector<int>> term = s.is_terminal();
        if(term.first){
            return pair<pair<int, int>,vector<int>>(null_move(), term.second);
        }
        
        // If the main player is dead, we don't want to search more - we lost
        if(s.is_dead(main_player)){
            // let the heuristic evaluate the state
            return pair<pair<int, int>,vector<int>>(null_move(), h->evaluate(s));
        }

        // If we are at maximum depth, we evaluate the state
        if(depth == 0){
            auto evaluated = h->evaluate(s);
            return pair<pair<int, int>, vector<int>>(null_move(), evaluated);
        }

        // If the current player is dead, we continue on to the next player
        // This does not increase the branching factor, so we keep the depth the same
        if(s.is_dead(player_idx)){
            return minimax_mult(s, depth, (player_idx + 1) % num_players, main_player);
        }
        check_time(timer);

        // Get all the available moves
        vector<pair<int, int>> moves = s.get_available_moves(player_idx);
        // cerr << "Got available moves for player " << player_idx << endl;
        // for (auto m : moves){
            // cerr << m.first << " " << m.second << endl;
        // }
        // cerr  << "from " << s.positions[player_idx].first << " " << s.positions[player_idx].second << endl;

        // cerr << "On board" << endl;
        // s.print_board();

        // There should always be a move, as we use a die move

        // if(moves.size() == 0){
        //     cerr << "Found no moves" << endl;
        //     s.apply_move(die_move(), player_idx);
        //     auto res = minimax_mult(s, depth - 1, (player_idx + 1) % num_players, main_player);
        //     s.undo_move(player_idx);
        //     return make_pair(die_move(), res.second);
        // }

        // Do the minimax search for each move
        int value = MIN_HEURISTIC - 1; // Initialize value at impossibly low value

        // Make the state value vector
        vector<int> value_vec;

        // Initialize best move
        pair<int, int> best_move;

        best_move = moves[0]; // Make sure we actually return something

        // For each move
        for(auto move: moves){

            // We apply the move, run the search, and undo the move
            s.apply_move(move, player_idx);
            pair<pair<int, int>,vector<int>> result = minimax_mult(
                s,
                depth - 1, 
                (player_idx + 1) % num_players,
                main_player
            );
            check_time(timer);
            s.undo_move(player_idx);

            // If the result is better than the current value, update the value and the best move
            if(result.second[player_idx] > value){
                value = result.second[player_idx];
                value_vec = result.second;
                best_move = move;
            }
            check_time(timer);
        }
        // for(auto v: value_vec){
        //     if(abs(v) > MAX_HEURISTIC){
        //         cerr << "Invalid heuristic : minimax_mult heuristic" << endl;
        //         throw runtime_error("Invalid heuristic");
        //     }
        // }

        return pair<pair<int, int>,vector<int>>(best_move, value_vec);
    }

    string get_move(int player_idx){
        // Run the search algorithm and get it's output move, until the time runs out
        // We don't initialize the timer here as we do that in the main function
        timer = clock();
        string best_move;

        // Start in depth 1
        int depth = 1;

        string bst = "";
        // cerr << "REAL BOARD" << endl;
        // this->state->print_board();
        State cpy = this->state->copy();
        // run an infinite while loop until we get a time out
        while(1){
            try{
                check_time(timer);
                // Copy the state, as it might get screwed when we time out

                // Send the copy state into a minimax search, telling the search we are searching for our player
                pair<pair<int, int>,vector<int>> result = minimax_mult(cpy, depth, player_idx, player_idx);

                check_time(timer);

                // cerr << "AFTER RUNNING" << endl;
                // cpy.print_board();

                // Print somee info
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
                check_time(timer);
                cerr << endl;
                cerr << "Dead: " << state->dead.size() << endl;

                // Get the string value of the best move
                best_move = move_str(result.first, player_idx);

                // If the heuristic value is at max or min, we know we won or lost
                // No need to search further
                if(abs(result.second[player_idx]) == h->max_heuristic()){
                    break;
                }

                cerr << "Time elapsed: " << time_elapsed(timer) << endl;


                depth++;
            }
            catch(TimeOutException){
                cerr << "Time out!" << endl;
                cerr << "Time elapsed: " << time_elapsed(timer) << endl;

                return best_move;
            }

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
            cerr << "Player " << i << ": " << player_state[0] << " " << player_state[1] << " " << player_state[2] << " " << player_state[3] << endl;
        }

        cerr << n << " " << p << endl;

        if(!initialized){
            agent.initialize(n, player_states);
            initialized = true;
        } else {
            agent.update(player_states);
        }

        string move = agent.get_move(p);

        // agent.state->print_board();
        cerr << player_states[p][0] << " " << player_states[p][1] << " " << player_states[p][2] << " " << player_states[p][3] << endl;
        cerr << p << endl;

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        cout << move << endl; // A single line with UP, DOWN, LEFT or RIGHT
        // exit(0);
    }
}