#include <vector>
#include <deque>
#include <map>
#include <list>
#include <string>
#include <strings.h>
#include <iostream>
using namespace std;

/*
* This struct is used in a state machine to determine whether or not to take a branch, start w/ a weak dont take
* 00=strong dont take, 01=weak dont take, 10=weak take, 11=strong take
*/
struct state {
  uint8_t counter;
};


//this is called to update the passed state when we find a state was/wasnt taken
void stateTaken(state* s) {
  if(s->counter < 15){
    s->counter++;
  }
}

void stateNotTaken(state* s) {
  if(s->counter > 0){
    s->counter--;
  }
}

inline bool shouldPredict(state* s) {
    return s->counter >= 4;
}
