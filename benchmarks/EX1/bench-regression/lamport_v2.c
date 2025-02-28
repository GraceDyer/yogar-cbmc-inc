/* Copyright (C) 2018
 * This benchmark is part of SWSC
 */

/* Adapted from: https://svn.sosy-lab.org/software/sv-benchmarks/trunk/c/pthread-atomic/lamport_true-unreach-call.c */

#include <assert.h>
#include <stdint.h>
#include <pthread.h>

#ifndef N
#  warning "N was not defined"
#  define N 4
#endif

// shared variables
int x, y;
int b1, b2;
int var;

void *p1(void *arg)
{
  int ok;
  for (int jj=0; jj<2; jj++) {
    ok = 0;
    
    for (int i=0; i<N; i++) {
      b1 = 1;
      x = 1;
      
      if (y != 0) {
        b1 = 0;
        
        for (int i=0; i<N; i++) {
          if (y == 0)
            goto breaklbl0;
        }
        
        goto breaklbl;
      breaklbl0:;
        continue;
      }
      
      y = 1;
      
      if (x != 1) {
        b1 = 0;
        
        for (int i=0; i<N; i++) {
          if (b2 < 1)
            goto breaklbl1;
        }
        
        goto breaklbl;
      breaklbl1:;
        
        if (y != 1) {
          for (int i=0; i<N; i++) {
            if (y == 0)
              goto breaklbl2;
          }
          
          goto breaklbl;
        breaklbl2:;
          continue;
        }
      }
      
      ok = 1;
      goto breaklbl;
    }
    
  breaklbl:;
    if (ok==0) return NULL;
    
    // critical section
    // var = 1; sub
    assert(var == 1);
    
    y = 0;
    b1 = 0;
  }
  
  return NULL;
}

void *p2(void *arg)
{
  int ok;
  for (int jj=0; jj<2; jj++) {
    ok = 0;
    
    for (int i=0; i<N; i++) {
      b2 = 1;
      x = 2;
      
      if (y != 0) {
        b2 = 0;
        
        for (int i=0; i<N; i++) {
          if (y == 0)
            goto breaklbl0;
        }
        
        goto breaklbl;
      breaklbl0:;
        continue;
        
      }
      
      y = 2;
      x=2; //add
      if (x != 2) {
        b2 = 0;
        
        for (int i=0; i<N; i++) {
          if (b1 < 1)
            goto breaklbl1;
        }
        
        goto breaklbl;
      breaklbl1:;
        
        if (y != 2) {
          for (int i=0; i<N; i++) {
            if (y == 0)
              goto breaklbl2;
          }
          
          goto breaklbl;
        breaklbl2:;
          continue;
        }
      }
      
      ok = 1;
      goto breaklbl;
    }
    
  breaklbl:;
    if (ok==0) return NULL;
    
    // critical section
    var = 2;
    assert(var == 2);
    
    y = 0;
    b2 = 0;
  }
  
  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t a, b;
  
  x = 0;
  y = 0;
  b1 = 0;
  b2 = 0;
  var = 0;
  
  pthread_create(&a, NULL, p1, NULL);
  pthread_create(&b, NULL, p2, NULL);
  
  pthread_join(a, NULL);
  pthread_join(b, NULL);
  
  return 0;
}

