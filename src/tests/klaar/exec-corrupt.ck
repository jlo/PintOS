# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
(exec-corrupt) begin
(exec-corrupt) exec("currupt-elf"): -1
(exec-corrupt) end
exec-corrupt: exit(0)
EOF
pass;
