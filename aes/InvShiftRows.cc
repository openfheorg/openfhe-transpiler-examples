#include "InvShiftRows.h"


// The Inverse of the ShiftRows() function shifts the rows in the state to the right.

#pragma hls_top
void InvShiftRows(state_t &state)
{
  uint8_t temp;

  // Rotate first row 1 columns to right  
  temp = state[3][1];
  state[3][1] = state[2][1];
  state[2][1] = state[1][1];
  state[1][1] = state[0][1];
  state[0][1] = temp;

  // Rotate second row 2 columns to right 
  temp = state[0][2];
  state[0][2] = state[2][2];
  state[2][2] = temp;

  temp = state[1][2];
  state[1][2] = state[3][2];
  state[3][2] = temp;

  // Rotate third row 3 columns to right
  temp = state[0][3];
  state[0][3] = state[1][3];
  state[1][3] = state[2][3];
  state[2][3] = state[3][3];
  state[3][3] = temp;
}
