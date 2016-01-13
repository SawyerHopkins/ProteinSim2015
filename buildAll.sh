#CORE

echo
echo Making core library
echo

cd ClusteredCore/$1
make
cd ../../

#END
#FORCE

echo
echo Making LennardJones
echo

cd LennardJones/$1
make
cd ../../

#END
#FORCE

echo
echo Making AOPotential
echo

cd AOPotential/$1
make
cd ../../

#END
#FORCE

echo
echo Making Calibration
echo

cd Calibration/$1
make
cd ../../

#END
#UI

echo
echo Making user interface
echo 

cd ClusteredUI/$1
make
make post-build
cd ../../

#END
