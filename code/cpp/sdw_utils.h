#pragma once

#pragma region // --------- INCLUDES

#include <set>
#include <string>
#include <vector>
#include "sdw_map.h"

#pragma endregion

#pragma region // --------- MACROS

#define SDW_MAP(op, key) \
  SdwMap##op_##key

#define SDW_CREATE(type, var, fcall) \
  type var{};                        \
  SDW_TRY(fcall)

#define SDW_STRUCT(type, var) \
  type var{};                 \
  var.sType = (VkStructureType)SdwMapSE_##type

#define SDW_TRY(fcall)                                    \
  do                                                      \
  {                                                       \
    VkResult result = fcall;                              \
    if (result != VK_SUCCESS)                             \
    {                                                     \
      LOG_ERR("failed to create object (%d).\n", result); \
    }                                                     \
  } while (0)

#define SDW_OBJECTS(function, var, ...)              \
  std::vector<SdwMapFS_##function> var;              \
  uint32_t var##_count = 0u;                         \
  function(__VA_ARGS__, &var##_count, NULL);         \
  if (var##_count)                                   \
  {                                                  \
    var.resize(var##_count);                         \
    function(__VA_ARGS__, &var##_count, var.data()); \
  }

#define SDW_CAPSULE(type, ...) \
  ([&]() -> const type * {                      \
    static const type arr[] = { __VA_ARGS__ }; \
    return arr; }())

#pragma endregion
