#!/bin/bash
#------------------------ test qucsator ---------------------
# ~/local/qucs/bin/qucsator --help
# Usage: /home/gAz_/local/qucs/bin/qucsator [OPTION]...
# 
#   -h, --help     display this help and exit
#   -v, --version  display version information and exit
#   -i FILENAME    use file as input netlist (default stdin)
#   -o FILENAME    use file as output dataset (default stdout)
#   -b, --bar      enable textual progress bar
#   -g, --gui      special progress bar used by gui
#   -c, --check    check the input netlist and exit
#   -p, --path     project path (or location of dynamic modules)
#   -m, --module   list of dynamic loaded modules (base names separated by space)
# 
# Report bugs to <qucs-bugs@lists.sourceforge.net>.
CALL_DIR=$(dirname $(readlink -f $0))

Q_BIN=~/local/qucs/bin/qucsator; #binary from build
#~/local/qucs/bin/qucsator -i ~/raid1/bl/projects/qucs/qucs_tools/WeiGong.net -o out.txt
${Q_BIN} -i ${CALL_DIR}/WeiGong.net -o ${CALL_DIR}/WeiGong.out
