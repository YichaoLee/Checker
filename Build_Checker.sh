#!/bin/bash
myPath="Bin"
if [ -d "$myPath" ]; then
	rm -rf "$myPath"
fi 
if [ ! -f "/usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.20" ]; then
	sudo rm /usr/lib/x86_64-linux-gnu/libstdc++.so*
	sudo cp mylib/libstdc++* /usr/lib/x86_64-linux-gnu/
	sudo ln -s /usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.20 /usr/lib/x86_64-linux-gnu/libstdc++.so.6
fi
echo "Building Checker~~~~~~~~~~~~~~~~~~~~~~~~"
cp mylib/lib* ../../../Release+Asserts/lib/
make
mkdir "$myPath"
sudo rm /usr/lib/buildCFG.so
sudo ln -s ../../../Release+Asserts/lib/buildCFG.so /usr/lib
g++ verify.cpp -o Bin/Checker
sudo chmod -R 777 Bin
echo "Building finished!-----------------------Start run program in Directory Bin"

