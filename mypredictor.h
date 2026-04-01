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
  uint8_t counter=4;
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


/*
* Stride: holds running stride for last 3 values, calculate stride from last val
*/
/*
struct stride {
  uint64_t lastVal=0;
  uint64_t s1;//newest
  uint64_t s2;
  uint64_t s3;//oldest
};
*/
/*
* Use this to find the median of 3 values.  will use this to determine which of the
* possible strides to use.
*/
/*
uint64_t median_of_three(uint64_t a, uint64_t b, uint64_t c) {
    if ((b > a) == (a > c)) return a;
    if ((a > b) == (b > c)) return b;
    return c;
}

void strideUpdate(stride* s, uint64_t value){
  s->s3=s->s2;
  s->s2=s->s1;
  s->s1=value-s->lastVal;
  s->lastVal = value;
}

uint64_t getStride(stride* s){
  return median_of_three(s->s1,s->s2,s->s3);
}*/

/*
* Stride: holds running stride history for last 5 values, calculate stride from last val
* lastVal must remain present.
*/
struct stride {
  uint64_t lastVal = 0;

  // newest -> oldest
  int64_t s1 = 0;
  int64_t s2 = 0;
  int64_t s3 = 0;
  int64_t s4 = 0;
  int64_t s5 = 0;

  // number of valid strides seen so far, capped at 5
  uint8_t count = 0;

  // whether lastVal has been initialized by a real value yet
  bool hasLast = false;
};

/*
* Use this to find the median of 3 values.
* Kept exactly as requested.
*/
uint64_t median_of_three(uint64_t a, uint64_t b, uint64_t c) {
    if ((b > a) == (a > c)) return a;
    if ((a > b) == (b > c)) return b;
    return c;
}

/*
* Internal helper: median of 5 signed strides.
* This is not part of your external interface, so it is safe to add.
*/
static int64_t median_of_five(int64_t a, int64_t b, int64_t c, int64_t d, int64_t e) {
    int64_t arr[5] = {a, b, c, d, e};

    // simple insertion sort for 5 elements
    for (int i = 1; i < 5; i++) {
        int64_t key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }

    return arr[2]; // middle element
}

void strideUpdate(stride* s, uint64_t value){
  if (!s->hasLast) {
    s->lastVal = value;
    s->hasLast = true;
    return;
  }

  int64_t newStride = (int64_t)value - (int64_t)s->lastVal;

  s->s5 = s->s4;
  s->s4 = s->s3;
  s->s3 = s->s2;
  s->s2 = s->s1;
  s->s1 = newStride;

  if (s->count < 5) {
    s->count++;
  }

  s->lastVal = value;
}

uint64_t getStride(stride* s){
  if (s->count == 0) {
    return 0;
  }
  if (s->count == 1) {
    return (uint64_t)s->s1;
  }

  // Fast adaptation: if the two newest strides agree, trust the new phase.
  if (s->s1 == s->s2) {
    return (uint64_t)s->s1;
  }

  if (s->count == 2) {
    return (uint64_t)((s->s1 + s->s2) / 2);
  }
  if (s->count == 3) {
    return median_of_three((uint64_t)s->s1, (uint64_t)s->s2, (uint64_t)s->s3);
  }
  if (s->count == 4) {
    int64_t arr[4] = {s->s1, s->s2, s->s3, s->s4};

    for (int i = 1; i < 4; i++) {
      int64_t key = arr[i];
      int j = i - 1;
      while (j >= 0 && arr[j] > key) {
        arr[j + 1] = arr[j];
        j--;
      }
      arr[j + 1] = key;
    }

    return (uint64_t)((arr[1] + arr[2]) / 2);
  }

  return (uint64_t)median_of_five(s->s1, s->s2, s->s3, s->s4, s->s5);
}