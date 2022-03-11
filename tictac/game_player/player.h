#ifndef PLAYER_jfdkljfdf09dfjdo
#define PLAYER_jfdkljfdf09dfjdo
#include "popen2.h"
#include <string>
#include <time.h>

#define PLAY_CLOCK 100
#define START_CLOCK 1000

using namespace std;
class Player {
public:
    string player_path;
    int ifp;
    int ofp;
    int pid;
    int timer;
    bool first;
    char buf[100];
    string zero_string;
    Player(string program_path){
        player_path ="./" +  program_path + " 2> "+ program_path + ".err";
        zero_string = "0\n";
    }

    ~Player(){
        close(ifp);
        close(ofp);
    }

    void start(){
        if(pid = popen2(player_path.c_str(), &ofp, &ifp) < 0){
            throw "error";
        }
    }

    double time_ms(){
        return  1000 * (double)(clock() - this->timer) / ((double)CLOCKS_PER_SEC);
    }

    pair<int, int> turn(int prevx, int prevy){
        string s = to_string(prevx) + " " + to_string(prevy) + "\n";
        sleep(0.1);
        cout << "Sending: \n" 
            << s
            << zero_string << flush;
        write(ofp, s.c_str(), s.length());

        write(ofp, zero_string.c_str(), zero_string.length());
        this->timer = clock();
        int n = 0;
        while(n <= 0){
            if(time_ms()  > (first?START_CLOCK:PLAY_CLOCK)){
                throw "timeout: " + to_string(time_ms());
                // cout  << time_ms() << "ms" << endl;
            }
            n = read(ifp, buf, 100);
        }

        // for(int i = 0; i < n; i++){
        //     cout <<(int ) buf[i];
        // }
        // cout << n << endl;
        // cout << buf << endl;

        int x=0, y=0;
        int i = 0;
        bool foundx = false,foundy=false;

        while(buf[i] != ' '){
            foundx = true;
            x = x * 10 + buf[i] - '0';
            i++;
            if(i > n){
                throw "error: not enough input";
            }
        }
        i++;
        while(buf[i] != '\n'){
            foundy = true;
            y = y * 10 + buf[i] - '0';
            i++;
            if(i > n){
                throw "error: not enough input";
            }
        }

        if(!foundx || !foundy){
            throw "error: not enough input";
        }
        first = false;
        return make_pair(x, y);
    }

};

#endif