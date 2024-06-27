#!/bin/bash




remote=root@192.168.58.128

rsync -azcvPh --delete-before  ./ $remote:/home/yao/Documents/project_remote/

time ssh -t $remote "cd /home/yao/Documents/project_remote; cd char_dev; make" 
