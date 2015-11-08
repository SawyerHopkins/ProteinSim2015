#----------

echo
echo Making core library
echo

cd ClusteredCore/$1
make
cd ../../

#----------

echo
echo Making AOPotential
echo

cd AOPotential/$1
make
cd ../../

#----------

echo
echo Making Lennard Jones Potential
echo

cd LennardJones/$1
make
cd ../../

#----------

echo
echo Making Calibration Test
echo

cd Calibration/$1
make
cd ../../

#----------

echo
echo Making user interface
echo 

cd ClusteredUI/$1
make
make post-build
cd ../../

#----------
