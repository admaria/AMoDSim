# AMoDSim

An Efficient and Modular Simulation Framework for Autonomous Mobility on Demand.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and simulation purposes. 

### Prerequisites

AMoDSim requires the simulation framework OMNeT++ v4.6.

### Installing

(1) Download OMNeT++ 4.6 using the link below
    <https://omnetpp.org/component/jdownloads/summary/32-release-older-versions/2290-omnet-4-6-source-ide-tgz>

(2) Install OMNeT++ 4.6: follow the step by step installation guide under omnetpp-4.6/doc/InstallGuide.pdf.

(3) Clone the AMoDSim simulator into the Omnet++ workspace

*  Open a shell and go to the Omnet++ workspace (if you do not know what your workspace is, check Window/Preferences and type "Workspaces" into the search box):

    `cd /path/to/workplace/folder`

*  Clone the project to your Omnet++ workplace directory

    `git clone -b <branch> https://gitlab.com/andreadimaria/amodsim.git ./AMoD_Simulator`

    Branch is the name of the branch you want to work with. It can be `master` or `develop`
 
(4) Create an empty project into the Omnet++ workspace

   ```
    File → new → Omnet++ Project 
            - ProjectName: AMoD_Simulator
            - use default location
            - Support C++ Development
   ```
Then, "Create an empty project".
At this point, your project should already contain all the files needed.

**IMPORTANT:**
From within Omnet, delete the package.ned automatically created by Omnet into AMoD_Simulator/. If you don't find this file from within Omnet, delete it from outside, e.g., from command line.
 
 
(5) Build the project

    `Right click on project → Build Project`
    
(6) Now you can run the simulation example

    `Right click on omnetpp.ini located in simulations folder, then click on run as → Omnet++ Simulation`

## Authors

* **Andrea Di Maria** 

    Aucta Cognitio R&amp;D Labs, Catania 95123, Italy  
    Università di Catania, Catania 95125, Italy  
    [andrea.dimaria90@gmail.com](andrea.dimaria90@gmail.com), [adimaria@auctacognitio.net](adimaria@auctacognitio.net)

## Contributors

* Assoc. Prof. **Andrea Araldo**

    Télécom SudParis - Institut Polytechnique de Paris  
    [andrea.araldo@telecom-sudparis.eu](andrea.araldo@telecom-sudparis.eu)

* **Giovanni Morana** 

    Aucta Cognitio R&amp;D Labs, Catania 95123, Italy  
    [gmorana@auctacognitio.net](gmorana@auctacognitio.net)
    
* **Antonella Di Stefano**

    Università di Catania, Catania 95125, Italy  
    [ad@dieei.unict.it](ad@dieei.unict.it)

## Reference
The simulator is presented in the following article:

Di Maria, A., Araldo, A., Morana, G., Di Stefano, A.,  
**AMoDSim: An Efficient and Modular Simulation Framework for Autonomous Mobility on Demand**, Internet of Vehicles Conference, 2018 [paper-draft](https://arxiv.org/pdf/1808.04813.pdf) [slides](https://github.com/admaria/AMoDSim/blob/master/scientific-documents/slides/01.presentation-of-amodsim.iov.pdf)


It has been used in this other article:
A. Araldo, A. Di Maria, A. Di Stefano, G. Morana, “On the Importance of demand Consolidation inMobility on Demand,”IEEE/ACM International Symposium on Distributed Simulation and Real Time Applications (DS-RT), 2019 [paper-draft](https://arxiv.org/pdf/1907.02933) [slides](https://github.com/admaria/AMoDSim/blob/master/scientific-documents/slides/02.demand-consolidation.ds-rt.pdf)


## License

This project is licensed under the Attribution-NonCommercial-ShareAlike 4.0 International (CC BY-NC-SA 4.0) License - see the [LICENSE.md](LICENSE.md) file for details
