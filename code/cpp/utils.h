#pragma once

#pragma region // --------- INCLUDES

#include <fstream>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <windows.h>

#pragma endregion

#pragma region // --------- DEFINES

// Add definition for NULL
#define nullptr NULL

// Used to contextualize the "static" keyword based on usage
#define global static  // Used for global variables
#define local static   // Used for functions reserved to a single translation unit
#define persist static // Used for persistent memory variables

// Redefine shitty Windows naming convention
#define sleep Sleep

#pragma endregion

#pragma region // --------- MACROS

// Logging system
#define LOG(str, ...) printf(str, ##__VA_ARGS__)
#define LOG_MSG(msg, ...) printf("\033[0;46mMSG[%15s][%15s:%04u] : \033[0m " msg, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG_ERR(err, ...)                                                                                   \
  printf("\033[1;41mERR[%15s][%15s:%04u] : \033[0m " err, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__); \
  exit(-1)

#pragma endregion

#pragma region // --------- TYPES

// Types clearly stating memory dimensions in bits
typedef bool b8;      // Bool (size bit = 8)
typedef char c8;      // Char (size = 8 bit)
typedef float f32;    // Float (size = 32 bit)
typedef double f64;   // Float (size = 64 bit)
typedef int8_t i8;    // Integer (size = 8 bit)
typedef int16_t i16;  // Integer (size = 16 bit)
typedef int32_t i32;  // Integer (size = 32 bit)
typedef int64_t i64;  // Integer (size = 64 bit)
typedef uint8_t u8;   // Unsigned integer (size = 8 bit)
typedef uint16_t u16; // Unsigned integer (size = 16 bit)
typedef uint32_t u32; // Unsigned integer (size = 32 bit)
typedef uint64_t u64; // Unsigned integer (size = 64 bit)
typedef void v0;      // Void (0 is a placeholder for this special type)

#pragma endregion

#pragma region // --------- LOCAL FUNCTIONS

#pragma region // --- Declarations

static std::vector<c8> ut_read_bin(const std::string &filename);

#pragma endregion

#pragma region // --- Implementations

static std::vector<c8> ut_read_bin(const std::string &filename)
{
  std::ifstream file(filename, std::ios::ate | std::ios::binary);
  std::vector<c8> buffer{};
  u64 filesize = 0u;

  if (!file.is_open())
  {
    LOG_ERR("cannot open file '%s'.\n", filename.c_str());
  }

  filesize = (u64)file.tellg();
  buffer.resize(filesize);
  file.seekg(0);
  file.read(buffer.data(), filesize);
  file.close();

  return buffer;
}

#pragma endregion

#pragma endregion