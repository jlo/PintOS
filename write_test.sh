
cd src/userprog/build;
pintos -v -k -T 120 --fs-disk=2 --qemu \
-p ../../examples/pfs -a pfs \
-p ../../examples/pfs_writer -a pfs_writer \
-p ../../examples/pfs_reader -a pfs_reader \
-g messages -- -f -q run pfs;
