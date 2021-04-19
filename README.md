## Usage
* Download and install ns3 in Ubuntu
* For **CSMA**
    * `cd ns3.29`
    * move file csma.cc to dir /scratch
    * build: `./waf`
    * run: `./waf --run ./scratch/csma --vis` (vis is used for visualization)
    * run throughout.awk to analyze the throughout of a node(or a link) 
* For **RIP**
    * build and run: Similar to the above
    * analyze the router table printed in terminal or the visualization results


## Reference
[CSMA in nsnam.org](https://www.nsnam.org/doxygen/csma-one-subnet_8cc_source.html)  
[RIP in nsnam.org](https://www.nsnam.org/doxygen/rip-simple-network_8cc_source.html)