# ------------------------------------- IMPORTS

import re

# ------------------------------------- FUNCTIONS

def contains(str, keys):
  for k in keys.split(','):
    if k in str:
      return True
  return False

def camel2snake(str):
  # Insert underscores before capital letters (except the first one)
  s1 = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', str)
  # Handle cases like 'HTMLParser' → 'html_parser'
  snake = re.sub('([a-z0-9])([A-Z])', r'\1_\2', s1)
  return snake.lower()

def norm_space(str):
  return re.sub(r'\s+', ' ', str).strip()

# # ------------------------------------- MAIN

# Open file
f = open('input/input.h', 'r')
lines = [x.rstrip() for x in f.readlines()]
f.close()

# Accumulate info in a dictionary
key = ''
enums = {}
defines = []
for line in lines:
  line = norm_space(line)
  if '#define' in line and not contains(line, '('):
    defines.append(line[8:].upper())
  if 'typedef enum ' in line:
    key = line[13:-2]
    enums[key] = []
  elif key in enums:
    if key in line:
      key = ''
    elif not contains(line, '//,#'):
      info = line.replace(' ','').split('=')
      enums[key].append(info)

# Print results as needed
print('#pragma once\n')
print('#pragma region // --------- DEFINES\n')
for d in defines:
  d = d.replace('VK_', 'SDW_D_')
  print('#define ' + d)
print('#pragma endregion\n')
print('#pragma region // --------- TYPES\n')
for key in enums:
  t = 'sdw' + camel2snake(key)[2:]
  print(f'#pragma region // Defines for "{key}"')
  for item in enums[key]:
    f = item[0].replace('VK_', 'SDW_D_')
    v = f'(({key}) ' + item[1].replace('VK_', 'SDW_D_').replace(',','') + ')'
    print(f'#define {f} {v}')
  print('#pragma endregion\n')
print('#pragma endregion\n')
