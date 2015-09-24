#----------

echo
tput setaf 2
echo Making core library
tput setaf 7
echo

cd ClusteredCore
make clean
echo
./ClusteredCore.sh
cd ../

#----------

echo
tput setaf 2
echo Making Lennard Jones Potential
tput setaf 7
echo

cd LennardJones
make clean
echo
./LennardJones.sh
cd ../

#----------

echo
tput setaf 2
echo Making user interface
tput setaf 7
echo 

cd ClusteredUI
make clean
echo
./ClusteredUI.sh
cd ../

#----------

echo
tput setaf 2
echo Copying Forces
tput setaf 7
echo

cp LennardJones/bin/LJ.so ClusteredUI/bin/LJ.so
cp settings.cfg ClusteredUI/bin/settings.cfg
