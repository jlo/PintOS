#!/usr/bin/bash

cd src/userprog/build/;

<<COMMENT1
Starta ett användarprogram enligt föregående uppgifter, t.ex. sumargv. I debug-utskriften för
returvärdet från process_execute skall du se ett giltigt id, t.ex. 3.
COMMENT1

str="pintos -p ../../examples/sumargv -a sumargv -v -k --fs-disk=2 -- -f -q run 'sumargv 1 2 3'"
eval $str


<<COMMENT2
Gör som ovan, men skriv fel programnamn, skriv t.ex. sumsum istället för sumargv sist på
raden. Detta gör att load i start_process kommer misslyckas. I debug-utskriften för
returvärdet från process_execute skall du se -1.

COMMENT2

echo
echo "Press ENTER to continue..."
echo
read
str="pintos -p ../../examples/sumargv -a sumargv -v -k --fs-disk=2 -- -f -q run 'sumsum 1 2 3'"
eval $str



<<COMMENT3
Kör ett program med argument -tcl=2. Detta simulerar att thread_create misslyckas
skapa tråden vid andra anropet. I debug-utskriften för returvärdet från process_execute
skall du se -1, eftersom det inte blev någon process (inte ens en tråd!).
pintos -p ../../examples/sumargv -a sumargv -v -k --fs-disk=2 \
-- -f -q -tcl=2 run ’sumargv 1 2 3’

COMMENT3

echo
echo "Press ENTER to continue..."
echo
read


str="pintos -p ../../examples/sumargv -a sumargv -v -k --fs-disk=2 -- -f -q -tcl=2 run 'sumargv 1 2 3'"
eval $str

