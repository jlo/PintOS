	
cd src/userprog/build/;
pintos -v -k --fs-disk=2 -p ../../examples/file_syscall_tests -a fst -- -f -q run 'fst testing one two three'

