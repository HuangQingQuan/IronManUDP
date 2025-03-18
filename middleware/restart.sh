#!/bin/sh
PROCESS=`ps -ef|grep Middleware|grep -v grep|awk '{print $2}'`
for i in $PROCESS
do 
  echo "KILL the Middleware"
  kill -9 $i
done
cd /root/async-sockets-md/middleware
nohup ./Middleware > /root/async-sockets-md/middleware/daily.log 2>&1 &
