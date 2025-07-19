#pragma once

// --------- INCLUDES

#include <windows.h>
#include "utils.h"

// --------- DEFINES

#define INP_NUM 256
#define INP_U VK_UP
#define INP_L VK_LEFT
#define INP_D VK_DOWN
#define INP_R VK_RIGHT

// --------- LOCAL FUNCTIONS

local b8 input_press(c8 key)
{
  return (GetAsyncKeyState(key) & 0x8000);
}

// local b8 input_tap(c8 key)
// {
//   persist b8 was_down[INP_NUM]; // Tracks previous key states

//   b8 is_down_now = (GetAsyncKeyState(key) & 0x8000) != 0;
//   b8 just_pressed = is_down_now && !was_down[(u8)key];

//   was_down[(u8)key] = is_down_now;
//   return just_pressed;
// }
