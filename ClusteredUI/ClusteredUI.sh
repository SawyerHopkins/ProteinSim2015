
echo Executing Pre Build commands ...
echo
tput setaf 6
mkdir linked
mkdir bin
mkdir build
echo " Getting updated headers"
cp ../ClusteredCore/include/*.h include/
echo " Getting updated libraries"
cp ../ClusteredCore/bin/*.a linked/
tput setaf 7
echo Done
make
echo Executing Post Build commands ...

echo Done
