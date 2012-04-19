#!/usr/bin/bash

cd src/userprog/build/;
pintos -v -k --fs-disk=2 --qemu -p ../../examples/longrun_nowait -a nowait -p ../../examples/generic_parent -a generic_parent -p ../../examples/busy -a dummy -- -f -q run 'nowait 2 3'
  
