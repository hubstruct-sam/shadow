def contains_one(item, keys):
  for key in keys.split(','):
    if key in item:
      return True
  return False


f = open('input', 'r')
lines = f.readlines()
f.close()

fcont = {}
fname = False

for i in range(len(lines)):
  line = lines[i]
  if not fname and line[0] == '{':
    fname = lines[i-1].split('(')[0].split(' ')[-1]
    # fname = lines[i-1].split('(')[0]
    fcont[fname] = []
    fcont[fname].append(lines[i-1])
  if fname and line[0] == '}':
    fcont[fname].append(line)
    fname = False
  if fname:
    fcont[fname].append(line)

ordered = fcont.keys()
ordered = sorted(ordered)

# print(ordered)

for fname in ordered:
  for line in fcont[fname]:
    print(line, end='')
  print()