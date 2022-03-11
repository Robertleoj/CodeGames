#include <iostream>
#include "state.h"
#include "popen2.h"
#include "player.h"

using namespace std;
using std::cout;

int main(int argc, char *argv[]){
    if(argc != 3){
        cout << "Bruh" << endl;
        exit(1);
    }

    string player1_path = argv[1];
    string player2_path = argv[2];

    Player player1(player1_path);
    Player player2(player2_path);

    player1.start();
    player2.start();

    State state;

    pair<int, int> last_move;
    pair<int, int> move;
    pair<bool, int> term;
    int ply = 1;
    while(true){
        cout << "Ply " << ply << endl;
        ply++;
        try{
            last_move = state.get_last_move();
            if(state.turn){
                cout << "Cross's turn" << endl;
                move = player1.turn(last_move.first, last_move.second);
                state.move(move.first, move.second);
                cout << "Received: \n" << move.first << " " << move.second << endl;
            } else {
                cout << "Circle's turn" << endl;
                move = player2.turn(last_move.first, last_move.second);
                cout << "Received: \n" << move.first << " " << move.second << endl;
                state.move(move.first, move.second);
            }

        } catch(exception e){
            cout << "Error: " << e.what() << endl;
            break;
        } catch(const char *e){
            cout << "Error: " << e << endl;
            if(state.turn){
                cout << "Circle wins" << endl;
            } else{
                cout << "Cross wins" << endl;
            }
            break;
        } catch (string e){
            cout << "Error: " << e << endl;
            if(state.turn){
                cout << "Circle wins" << endl;
            } else{
                cout << "Cross wins" << endl;
            }
            break;
        }
        state.print_board();
        cout << "\n====================================\n" << endl;
        term = state.is_terminal();
        if(term.first){
            if(term.second == 1){
                cout << "Cross wins" << endl;
            } else if(term.second == 2){
                cout << "Circle wins" << endl;
            }
            break;
        }
        
    }




}
