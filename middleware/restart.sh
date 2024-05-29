#!/bin/sh
PROCESS=`ps -ef|grep Middleware|grep -v grep|awk '{print $2}'`
for i in $PROCESS
do 
  echo "KILL the Middleware"
  kill -9 $i
done
nohup /root/async-socket-md/middleware/Middleware &