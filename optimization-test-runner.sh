#!/bin/sh

HCOLOR='\033[0;36m'
NC='\033[0m' # No Color

LEVELS="O0"
LEVELS="${LEVELS} O1"
LEVELS="${LEVELS} O2"
LEVELS="${LEVELS} O3"
LEVELS="${LEVELS} Os"

cd ./bin

for LEVEL in $LEVELS; do
echo -e "${HCOLOR}====[Testing program with optimization level ${LEVEL}]====${NC}"
if [ -f "./contactbook-${LEVEL}" ];
then
    eval "./contactbook-${LEVEL}"
else
    echo "Fatal error: executable not found"
fi
echo -e "${HCOLOR}====[END]====${NC}\n\n"
done