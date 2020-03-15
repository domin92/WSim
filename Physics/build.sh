mkdir .build -p
cd .build
cmake .. -DCMAKE_BUILD_TYPE=Debug

if [ $? != 0 ]; then
    read _
fi
