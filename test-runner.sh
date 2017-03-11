#!/bin/sh

cd ./bin

HCOLOR='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${HCOLOR}====[Testing program using static library]====${NC}"
if [ -f ./contactbook-static ];
then
    ./contactbook-static
elif [ -f ./contactbook ];
then
    time ./contactbook
else
    echo "Fatal error: executable not found"
fi
echo -e "${HCOLOR}====[END]====${NC}\n\n"

echo -e "${HCOLOR}====[Testing program using shared library]====${NC}"
if [ -f ./contactbook-shared ];
then
    time ./contactbook-shared
else
    echo "Fatal error: executable not found"
fi
echo -e "${HCOLOR}====[END]====${NC}\n\n"

echo -e "${HCOLOR}====[Testing program using dynamic library]====${NC}"
if [ -f ./contactbook-dynamic ];
then
    time ./contactbook-dynamic
else
    echo "Fatal error: executable not found"
fi
echo -e "${HCOLOR}====[END]====${NC}\n\n"