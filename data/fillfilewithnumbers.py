import random as r
import sys

filename = sys.argv[1]
times = int(sys.argv[2])

if(len(sys.argv) > 3):
    r.seed(int(sys.argv[3]))

else:
    r.seed(None)

file = open(filename, 'w')

file.write(str(r.randint(-1000000,1000000)))

for i in range(1, times):
    file.write(',')
    file.write(str(r.randint(-1000000,1000000)))

file.close()