#include <unordered_map>
#include <vector>
#include <exception>
#include <functional>
#include <stdexcept>
#include <string>
#include <iostream>
#include <time.h>
#include <chrono>

/////////////////////////////////////////////////////////
// Agent static
/////////////////////////////////////////////////////////
template <
    class result_t   = std::chrono::milliseconds,
    class clock_t    = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds
>
auto since(std::chrono::time_point<clock_t, duration_t> const& start)
{
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}


// Create timeout exception
class TimeoutException : public std::exception {
    virtual const char* what() const throw() {
        return "TimeoutException";
    }
};

///////////////////////////////////////////////////////////
// Static
///////////////////////////////////////////////////////////


typedef std::vector<int> board_idx;

enum Condition
{
    EMPTY,
    CROSS,
    CIRCLE,
    DRAW
};

enum Turn {
    CROSS_TURN,
    CIRCLE_TURN
};



struct StateData{
    Condition ****board;
    Condition **win_boards;
    Turn turn;
    std::vector<board_idx> allowed_moves;
    std::pair<int, int> active_board; // -1, -1 if all are active
    std::pair<bool, Condition> terminal;
};

///////////////////////////////////////////////////////////
// Move hpp
///////////////////////////////////////////////////////////


class Move{
public:
    char x1;
    char y1;
    char x2;
    char y2;

    Move(){
        x1 = 0;
        y1 = 0;
        x2 = 0;
        y2 = 0;
    }

    Move(char x1, char y1, char x2, char y2){
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
    }

    Move(const Move &m){
        this->x1 = m.x1;
        this->y1 = m.y1;
        this->x2 = m.x2;
        this->y2 = m.y2;
    }

    char operator[](int idx) const;

    bool operator==(const Move &m) const {
        return (x1 == m.x1 && y1 == m.y1 && x2 == m.x2 && y2 == m.y2);
    }

    bool operator!=(const Move &m) const {
        return !(*this == m);
    }

};

///////////////////////////////////////////////////////////
// Move cpp
///////////////////////////////////////////////////////////

char Move::operator[](int idx) const {
    switch(idx){
        case 0:
            return x1;
        case 1:
            return y1;
        case 2:
            return x2;
        case 3:
            return y2;
    };
    throw std::runtime_error("Index out of range");
}
// make hash function for move
namespace std{
    
    template<>
    struct hash<Move>{
        size_t operator()(const Move &m) const{
            return hash<int>()(m[0] * 3) ^ hash<int>()(m[1] * 5) ^ hash<int>()(m[2] * 7) ^ hash<int>()(m[3] * 11);
        }
    };
}

///////////////////////////////////////////////////////////
// AgentState hpp
///////////////////////////////////////////////////////////

class AgentState {
public:
    Turn turn;

    int num_moves;
    Move ** move_sequence;
    Condition ****board;
    Condition **board_wins;

    Move *****all_moves;

    AgentState();

    ~AgentState();

    AgentState copy();

    void set_board(Move *idx, Condition c);

    std::pair<bool, Condition> board_is_terminal(Condition **b, bool big_board);

    void move(Move *idx);

    void undo_move();

    std::pair<bool, Condition> is_terminal();

    int get_available_moves(Move ** move_buffer);

    Condition player_cond();

    void switch_turns();
};

///////////////////////////////////////////////////////////
// AgentState cpp
///////////////////////////////////////////////////////////

AgentState::AgentState(){
    turn = CROSS_TURN;
    board = new Condition ***[3];
    board_wins = new Condition *[3];
    all_moves = new Move ****[3];
    for(int i = 0; i < 3; i++){
        board[i] = new Condition **[3];
        board_wins[i] = new Condition [3];
        all_moves[i] = new Move ***[3];
        for(int j = 0; j < 3; j++){
            board[i][j] = new Condition *[3];
            all_moves[i][j] = new Move **[3];
            board_wins[i][j] = EMPTY;

            for(int k = 0; k < 3; k++){
                board[i][j][k] = new Condition [3];
                all_moves[i][j][k] = new Move *[3];
                for(int l = 0; l < 3; l++){
                    board[i][j][k][l] = EMPTY;
                    all_moves[i][j][k][l] = new Move(i, j, k, l);
                }
            }
        }
    }
    
    this->num_moves = 0;
    this->move_sequence = new Move *[81];
}

AgentState::~AgentState(){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            for(int k = 0; k < 3; k++){
                delete [] board[i][j][k];
            }
            delete [] board[i][j];
        }
        delete [] board[i];
    }
    delete [] board;
    delete [] board_wins;
}


void AgentState::set_board(Move *m, Condition c){
    board[m->x1][m->y1][m->x2][m->y2] = c;
}

std::pair<bool, Condition> AgentState::board_is_terminal(Condition **b, bool big_board){


    for(int i = 0; i < 3; i++){
        if(b[i][0] == b[i][1] && b[i][1] == b[i][2] && b[i][0] != EMPTY && b[i][0] != DRAW){
            // return the right heuristic
            
            return std::pair<bool, Condition>(
                true, 
                b[i][0]
            );
        }

        if(b[0][i] == b[1][i] && b[1][i] == b[2][i] && b[0][i] != EMPTY && b[0][i] != DRAW){
            // return the right heuristic
            return std::pair<bool, Condition>(
                true, 
                b[0][i]
            );
        }

    }
    if(b[0][0] == b[1][1] && b[1][1] == b[2][2] && b[0][0] != EMPTY && b[0][0] != DRAW){
        // return the right heuristic
        return std::pair<bool, Condition>(
            true, b[0][0]
        );
    }
    if(b[0][2] == b[1][1] && b[1][1] == b[2][0] && b[0][2] != EMPTY && b[0][2] != DRAW){
        // return the right heuristic
        return std::pair<bool, Condition>(true, b[0][2]);
    }

    int num_squares_played = 0;
    int circle_wins = 0;
    int cross_wins = 0;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
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
        if(big_board){
            if(cross_wins > circle_wins){
                return std::make_pair(true, CROSS);
            }
            if(cross_wins < circle_wins){
                return std::make_pair(true, CIRCLE);
            }
        }
        return std::make_pair(true, DRAW);
    }

    return std::make_pair(false, EMPTY);
}

Condition AgentState::player_cond(){
    if(turn == CROSS_TURN){
        return CROSS;
    }
    return CIRCLE;
}

void AgentState::switch_turns(){
    if(turn == CROSS_TURN){
        turn = CIRCLE_TURN;
    } else{
        turn = CROSS_TURN;
    }
}

void AgentState::move(Move *m){
    
    set_board(m, player_cond());

    this->move_sequence[num_moves] = m;
    this->num_moves++;

    switch_turns();

    std::pair<bool, Condition> result = board_is_terminal(board[m->x1][m->y1], false);

    if(result.first){
        board_wins[m->x1][m->y1] = result.second;
    }

}

std::pair<bool, Condition> AgentState::is_terminal(){

    // First check if the board is won by either player
    auto big_board_term = board_is_terminal(board_wins, true);
    if(big_board_term.first){
        return std::make_pair(true, big_board_term.second);
    }

    return std::make_pair(false, EMPTY);
}

void AgentState::undo_move(){
    // Get last move
    if(num_moves == 0){
        throw std::runtime_error("No moves have been played!");
    }
    Move * last_move = move_sequence[num_moves - 1];

    // change board_wins
    board_wins[last_move->x1][last_move->y1] = EMPTY;

    // change board
    set_board(last_move, EMPTY);

    // change turn
    switch_turns();

    // change move_sequence
    num_moves--;
}

int AgentState::get_available_moves(Move ** move_buffer){

    // Check if we can play in the square played in
    int cnt = 0;
    if(num_moves > 0){
        Move *last_move = move_sequence[num_moves - 1];

        if(board_wins[last_move->x2][last_move->y2] == EMPTY){
            // We must play in that board
            for(int i = 0; i < 3; i ++ ){
                for(int j = 0; j < 3; j++){
                    if(board[last_move->x2][last_move->y2][i][j] == EMPTY){
                        move_buffer[cnt] = all_moves[last_move->x2][last_move->y2][i][j];
                        cnt++;
                    }
                }
            }

            return cnt;
        }
    } 

    // We can play in any board
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(board_wins[i][j] == EMPTY){
                for(int k = 0; k < 3; k++){
                    for(int l = 0; l < 3; l++){
                        if(board[i][j][k][l] == EMPTY){
                            move_buffer[cnt] = all_moves[i][j][k][l];
                            cnt++;
                        }
                    }
                }
            }
        }
    }
    return cnt;

}

AgentState AgentState::copy(){
    AgentState new_state;
    new_state.turn = turn;
    new_state.move_sequence = move_sequence;
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            new_state.board_wins[i][j] = board_wins[i][j];
            for(int k = 0; k < 3; k++){
                for(int l = 0; l < 3; l++){
                    new_state.board[i][j][k][l] = board[i][j][k][l];
                }
            }
        }
    }
    return new_state;
}

////////////////////////////////////////////////////////////////////////////////
// MCNode hpp
////////////////////////////////////////////////////////////////////////////////

class MCNode{
public:
    int plays;
    double wins;
    MCNode * parent;
    bool is_terminal;

    std::unordered_map<Move*, MCNode*> children;

    ~MCNode(){}

    MCNode(MCNode * parent=nullptr) {
        this->parent = parent;
        this->plays = 0;
        this->wins = 0;
        is_terminal = false;
    }
}; 

////////////////////////////////////////////////////////////////////////////////
// MCTree hpp
////////////////////////////////////////////////////////////////////////////////

class MCTree {
public:
    MCNode * root;

    MCTree(){
        root = new MCNode(nullptr);
    }

    void delete_tree(MCNode * node, MCNode * new_root);

    void move(Move *m);
};

////////////////////////////////////////////////////////////////////////////////
// MCTree cpp
////////////////////////////////////////////////////////////////////////////////


void MCTree::delete_tree(MCNode * node, MCNode * new_root){
    // base case
    if(node == nullptr){
        return;
    }

    // recursively delete children
    for(auto &c: node->children){
        if (c.second == new_root) {
            continue;
        }
        else {
            delete_tree(c.second, new_root);
        }
    }

    // delete the node
    delete node;
}

void MCTree::move(Move *m){

    auto child_map = this->root->children;

    if(child_map.find(m) == child_map.end() ){
        // Delete root and all children
        delete_tree(this->root, nullptr);

        // Create new node to be root
        this->root = new MCNode(nullptr);

    } else {
        // Get the new root
        MCNode *new_root = child_map[m];

        // Remove new root from root children
        child_map.erase(m);

        // Remove parent from new root
        new_root->parent = nullptr;

        // Delete rest of tree
        delete_tree(this->root, new_root);

        // replace root
        this->root = new_root;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Agent hpp
////////////////////////////////////////////////////////////////////////////////

class Agent {
public:

    Agent(int play_clock, int start_clock);

    void update(board_idx move);
    void update(Move *move);
    board_idx get_move();
    Move * board_idx_to_move(board_idx b);

private:

    int start_clock;
    std::chrono::_V2::steady_clock::time_point timer;

    int play_clock;

    bool first_turn;

    Move ** move_buffer;
    
    MCTree * tree;
    AgentState * state;

    board_idx move_to_board_idx(Move m);

    void start_timer();

    auto time_elapsed();

    bool time_out();

    void check_time();

    MCNode * selection();

    Condition simulation(MCNode * node);

    void backpropagation(MCNode *node, Condition sim_res);

    Move * get_current_best_move();

    double UCT(MCNode *node, MCNode * childnode);



};


////////////////////////////////////////////////////////////////////////////////
// Agent cpp
////////////////////////////////////////////////////////////////////////////////


#include <math.h>


Agent::Agent(int play_clock, int start_clock){
    this->state = new AgentState();
    this->tree = new MCTree();
    this->play_clock = play_clock;
    this->start_clock = start_clock;
    this->first_turn = true;
    this->move_buffer = new Move*[81];
}

void Agent::update(Move * move){
    state->move(move);
    tree->move(move);
}

void Agent::update(board_idx move){
    // Update state
    Move* m = this->state->all_moves[move[0]][move[1]][move[2]][move[3]];
    this->update(m);
}

double Agent::UCT(MCNode * node, MCNode * childnode){
    /*
    UCT(node, move) = 
        (w / n) + c * sqrt(ln(N) / n)

    w = wins for child node after move
    n = number of simulations for child node
    N = simulations for current node
    c = sqrt(2)
    */
    if(childnode == nullptr){
        throw std::runtime_error("No child node");
    }

    double w = childnode->wins;
    double n = childnode->plays;
    double N = node->plays;
    double c = sqrt(2);
    
    return (w / n) + c * sqrt(log(N) / n);

}

MCNode * Agent::selection(){

    MCNode * current_node = tree->root;
    double max_uct;
    double uct;
    MCNode * next_node = nullptr;
    Move *best_move;

    std::unordered_map<Move *, MCNode *> *child_map;
    MCNode * child_node = nullptr;

    while(true){
        // find best node
        if(current_node->is_terminal){
            return current_node;
        }

        max_uct = -100000;
        int num_moves = state->get_available_moves(move_buffer);

        child_map = &current_node->children;

        for(int i = 0; i < num_moves; i++){
            Move * m = move_buffer[i];
            if(child_map->find(m) == child_map->end()){
                // Make new node 
                MCNode * new_node = new MCNode(current_node);
                current_node->children[m] = new_node;
                
                // update state
                state->move(m);

                // return new node
                return new_node;
    
            } else {
                
                // Get uct value
                child_node = child_map->at(m);
                uct = UCT(current_node, child_node);

                // Update 
                if(uct > max_uct){
                    max_uct = uct;
                    next_node = child_node;
                    best_move = m;
                }
            }
        }
        state->move(best_move);
        current_node = next_node;
        next_node = nullptr;
        child_node = nullptr;
    }
}

Condition Agent::simulation(MCNode *selected_node) {
    int cnt = 0;
    std::pair<bool, Condition> res;
    int num_moves;
    int rand_idx;

    while (true) {
        res = state->is_terminal();
        
        if(res.first){
            // if counter == 0, update selected node to be terminal
            if(cnt == 0) {
                selected_node->is_terminal = true;
            }
            break;
        }

        // Select random move
        num_moves = state->get_available_moves(move_buffer);
        rand_idx = rand() % num_moves;
        state->move(move_buffer[rand_idx]);
        cnt ++;
    }

    for(int i = 0; i < cnt; i++){
        state->undo_move();
    }

    return res.second;
};


void Agent::backpropagation(MCNode * node, Condition sim_res){
    while(true){
        node->plays++;
        switch(sim_res){
            case DRAW:
                node->wins += 0.5;
                break;

            case CIRCLE:
                node->wins += (state->turn == CIRCLE_TURN ? 0 : 1);
                break;
            
            case CROSS:
                node->wins += (state->turn == CROSS_TURN ? 0 : 1);
                break;
        }


        if (node->parent == nullptr) {
            break;
        } else {
            state->undo_move();
            node = node->parent;
        }
    }
}

Move * Agent::get_current_best_move(){
    double highest = -10000;
    Move *best_move;
    auto child_map = tree->root->children;
    double score;
    
    for(auto &p : child_map){
        score = p.second->wins / p.second->plays;
        if(score > highest){
            highest = score;
            best_move = p.first;
        }
    }

    return best_move;
}

board_idx Agent::get_move(){
    start_timer();

    int i = 0;
    while(!time_out()){
        i++;

        MCNode * selected_node = selection();

        Condition sim_res = simulation(selected_node);

        backpropagation(selected_node, sim_res);
    }

    // Get best move
    Move * ret_move = get_current_best_move();
    update(ret_move);
    first_turn = false;
    return move_to_board_idx(*ret_move);

}

Move * Agent::board_idx_to_move(board_idx b){
    return this->state->all_moves[b[0]][b[1]][b[2]][b[3]];
}

board_idx Agent::move_to_board_idx(Move m){
    return board_idx {m.x1, m.y1, m.x2, m.y2};
}


// time elapsed in ms since we started the timer
auto Agent::time_elapsed(){
    return since(timer).count();
}

// Check if we have timed out
bool Agent::time_out(){
    if(first_turn){
        return time_elapsed() > start_clock;
    } else {
        return time_elapsed() > play_clock;
    }
}

void Agent::check_time(){
    if(time_out()){
        throw TimeoutException();
    }
}

void Agent::start_timer(){
    timer = std::chrono::steady_clock::now();
}

///////////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////////

std::pair<int, int> coord_from_move(Move *m){
    int x = 3 * m->x1 + m->x2;
    int y = 3 * m->y1 + m->y2;

    return std::make_pair(x, y);
}

board_idx b_idx_from_2d_coord(int x, int y){
    int x1 = x / 3;
    int x2 = x % 3;
    int y1 = y / 3;
    int y2 = y % 3;

    return {x1, y1, x2, y2};
}


int main(int argc, char *argv[])
{
    Agent agent = Agent(99, 990);

    while (2) {
        // i++;
        int opponent_row;
        int opponent_col;
        std::cin >> opponent_row >> opponent_col; std::cin.ignore();
        int valid_action_count;

        std::cin >> valid_action_count; std::cin.ignore();

        for (int i = 0; i < valid_action_count; i++) {
            int row;
            int col;
            std::cin >> row >> col; std::cin.ignore();
        }

        if(opponent_row != -1){
            auto b_idx = b_idx_from_2d_coord(opponent_row, opponent_col);
            agent.update(b_idx);
        }

        Move * m = agent.board_idx_to_move(agent.get_move());
        auto coord = coord_from_move(m);

        std::cout << coord.first << " " << coord.second << std::endl;
    }
}
