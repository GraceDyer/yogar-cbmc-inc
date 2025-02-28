/* Copyright (C) 2018
 * This benchmark is part of SWSC
 */

/* Adapted from:
 * Szymaski's critical section algorithm, implemented with fences.
 *
 * Adapted from: https://svn.sosy-lab.org/software/sv-benchmarks/trunk/c/pthread-atomic/szymanski_true-unreach-call.c
 */

#include <assert.h>
#include <stdint.h>
#include <pthread.h>

#ifndef N
#  warning "N was not defined"
#  define N 10
#endif

// shared variables
int flag1;
int flag2;

int x;


void *p1(void *arg)
{
  int ok;
  for (int j=0; j<2; j++) {
    flag1 = 1;
    
    ok = 0;
    for (int i=0; i<N; i++) {
      if (flag2 < 3) {
        ok = 1;
        break;
      }
    }
    
    if (ok==0) return NULL;
    flag1 = 3;
    
    if (flag2 == 1) {
      flag1 = 2;
      
      ok =0;
      for (int i=0; i<N; i++) {
        if (flag2 == 4) {
          ok = 1;
          break;
        }
      }
      
      if (ok==0) return NULL;
    }
    
    flag1 = 4;
    
    ok = 0;
    for (int i=0; i<N; i++) {
      if (flag2 < 2) {
        ok = 1;
        break;
      }
    }
    
    if (ok==0) return NULL;
    // critical section
    x = 1;
    assert(x == 1);
    
    ok = 0;
    for (int i=0; i<N; i++) {
      if (2 > flag2 || flag2 > 3) {
        ok = 1;
        break;
      }
    }
    
    if (ok==0) return NULL;
    flag1 = 0;
  }
  
  return NULL;
}

void *p2(void *arg)
{
  int ok;
  for (int j=0; j<2; j++) {
    flag2 = 1;
    
    ok = 0;
    for (int i=0; i<N; i++) {
      if (flag1 < 3) {
        ok = 1;
        break;
      }
    }
    flag1 = flag2 * 2;//Add.
    
    if (ok==0) return NULL;
    flag2 = 3;
    
    if (flag1 == 1) {
      flag2 = 2;
      
      ok =0;
      for (int i=0; i<N; i++) {
        if (flag1 == 4) {
          ok = 1;
          break;
        }
      }
      
      if (ok==0) return NULL;
      
    }
    
    flag2 = 4;
    
    ok = 0;
    for (int i=0; i<N; i++) {
      if (flag1 < 2) {
        ok = 1;
        break;
      }
    }
    
    if (ok==0) return NULL;
    // critical section
    x = 2;
    assert(x == 2);
    
    ok = 0;
    for (int i=0; i<N; i++) {
      if (2 > flag1 || flag1 > 3) {
        ok = 1;
        break;
      }
    }
    
    if (ok==0) return NULL;
    flag2 = 0;
  }
  
  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t a, b;
  
  flag1 = 0;
  flag2 = 0;
  x = 0;
  
  pthread_create(&a, NULL, p1, NULL);
  pthread_create(&b, NULL, p2, NULL);
  
  pthread_join(a, NULL);
  pthread_join(b, NULL);
  
  return 0;
}
