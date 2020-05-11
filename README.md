# multithreaded-mergesort

this was built in Ubuntu 20.04, I cannot guarantee this will work for any other operating system

to build type "make" in the directory

for instructions on how to run type "./sort -h"

I am mostly satisfied with this project, it can currently sort up to 10,000,000 numbers without seg faulting, perhaps a lot more

reading from a file is very slow, and the actual sort is a very small fraction of time

There are definitely some more optimizations that can be done for the multithreaded mergesort option, but, as for now, adding more threads increases the time to sort