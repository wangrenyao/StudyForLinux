cmd_/home/yao/Documents/project/mod/test_mod.mod := printf '%s\n'   test_mod.o | awk '!x[$$0]++ { print("/home/yao/Documents/project/mod/"$$0) }' > /home/yao/Documents/project/mod/test_mod.mod
