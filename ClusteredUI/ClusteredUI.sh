
echo Executing Pre Build commands ...
mkdir -p bin
mkdir -p build
echo
tput setaf 6
echo " Getting updated headers"
cp ../ClusteredCore/include/*.h include/
cp ../ClusteredCore/include/*.cuh include/
echo " Getting updated libraries"
cp ../ClusteredCore/bin/*.a linked/
tput setaf 7
echo Done
make
