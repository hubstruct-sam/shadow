# ----------------- FUNCS

def lines(filename):
  f = open(filename, 'r')
  lines = [x.rstrip() for x in f.readlines()]
  f.close()
  return lines

def remove(str, stuff):
  stuff = stuff.replace(' ', '')
  for x in stuff.split(','):
    str = str.replace(x, '')
  return str

def filter(key, stuff):
  stuff = stuff.replace(' ', '')
  for x in stuff.split(','):
    if x in key:
      return True
  return False

def try_add(map, key, value):
  if filter(key, 'vkCreate,vkGet,vkEnumerate'):
  # if filter(key, 'vkCreate'):
    if key not in map or (len(map[key]) < len(value)):
      map[key] = value    

# ----------------- MAIN

def main():
  funcs = lines('input/funcs.txt')
  types = lines('input/types.txt')
  
  print('#pragma once\n')
  
  print('#define SDW_MAP(key) \\')
  print('  SdwMap_##key\n')
  
  map = {}
  for t in types:
    tmp_t = remove(t, 'Vk')
    for f in funcs:
      tmp_f = remove(f, 'Result,Results')
      if 'sKHR' == tmp_f[-4:]:
        tmp_f = tmp_f[:-4] + 'KHR'
      elif tmp_t[-1] != 's':
        tmp_f = tmp_f.rstrip('s')        
      if tmp_f[-len(tmp_t):] == tmp_t:
        try_add(map, f, t)
      elif 'KHR' not in tmp_t and tmp_t in tmp_f[-len(tmp_t) - 3:]:
        tmp_f = tmp_f.replace('KHR', '')
        try_add(map, f, t)
  
  keys = sorted(map.keys())
  [print(f'#define SdwMap_{key} {map[key]}') for key in keys]
        

# ----------------- EXEC

main()

# VkSurfaceFormatKHR -> remove(Vk, KHR), rstrip(s)
# vkGetPhysicalDeviceSurfaceFormatsKHR -> remove(KHR), rstrip(s)