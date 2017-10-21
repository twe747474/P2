1 /bin/bash 

program=$1

echo "g++ -std=c++14 -Wall -Wextra -Wpedantic -Werror " $program

g++ -std=c++14 -Wall -Wextra -Wpedantic -Werror  $program 

