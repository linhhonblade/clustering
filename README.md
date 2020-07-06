clustering: a ns3-clustering algorithm implementation
========================================================


### An overview
clustering implements a clustering algorithm in our internship

*The code is tested in 3.30 version of ns-3 simulator*


### Build clustering
Copy and paste the clustering package under src/ folder of the ns-3 simulator.

type the command

`./waf configure --enable-examples --enable-modules=clustering`

followed by

`./waf`

in the the root directory of the ns-3 simulator. The files built will be copied in the build/ directory.


### Run the clustering example
type the command

`./waf --run clustering-example`

*The source code of the example can be found in the examples/ directory*
