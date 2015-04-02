#! /bin/sh -ex
cd 'ren-cxx-filesystem'
g++ -std=c++1y -Wall -pedantic -Wno-unused-local-typedefs '-DRESOURCELOCATION="."' -O0 -ggdb  -c -o file.o file.cxx -fPIC
g++ -std=c++1y -Wall -pedantic -Wno-unused-local-typedefs '-DRESOURCELOCATION="."' -O0 -ggdb  -c -o path.o path.cxx -fPIC
cd 'test'
g++ -std=c++1y -Wall -pedantic -Wno-unused-local-typedefs '-DRESOURCELOCATION="."' -O0 -ggdb  -c -o testpath.o testpath.cxx 
g++ -Wall -Werror -pedantic -O0 -ggdb -o testpath testpath.o ../path.o ../file.o   
./testpath  2>&1 > testpath.results.txt
cd '../..'
g++ -std=c++1y -Wall -pedantic -Wno-unused-local-typedefs '-DRESOURCELOCATION="."' -O0 -ggdb  -c -o polytaxis-unwrap.o polytaxis-unwrap.cxx -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse
g++ -Wall -Werror -pedantic -O0 -ggdb -o polytaxis-unwrap polytaxis-unwrap.o ren-cxx-filesystem/path.o ren-cxx-filesystem/file.o -lfuse -pthread  
