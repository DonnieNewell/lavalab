file: ReadMe.txt
author: Donnie Newell
email: donaldnewell08@gmail.com
last revised: 03Aug2013 2:01p PST

The project is divided up into 3 parts, based on the authors:
Faust and Valente: the stencil compiler and some sample templates.
Meng: contains compiled examples 
	- Cell 3D 
	- HotSpot 2D
	- PathFinder ?D
	- Poisson ?D
Newell: 
	- Contains a copy of the compiler, but the main folder is "examples"
	- There are Netbeans project files included to make building easier.
	- The goal of this work was to develop a load-balancing template
		that would run on a small cluster using MPI, CUDA, and OpenMP.
	- Instead of working on the template from the start, all development was
		on Cell and HotSpot application source. The idea was that once it was
		working, we could work backwards and adapt it to a template.
	- In the examples folder, you will find the main classes for the
		load-balancing project. 
		-- Balancer : takes a Cluster, and a Decomposition and allocates
			the SubDomains to all of the devices on the nodes.
		-- Cluster : manages the nodes 
		-- comm : contains the mpi source for delivery of all of the
			blocks and ghost zones aka halos.
		-- Decomposition : handles how to break up the giant data matrix.
			* You can break it up into cubes, poles, or slabs.
			* How you decompose the data has implications for pyramid height,
				SubDomain allocation, etc.
			* Slab is the simplest because you only have to exchange 
				ghost zones with 2 neighbors for each SubDomain, where cubes
				have 22 neighbors for each SubDomain. 
			* The tradeoff is that as the number of neighbors goes down, the
				communication cost for each ghost zone goes up.
		-- Node : manages all of the SubDomains for itself, keeps track of
			it's neighbors, and who it's children are for the purposes of 
			the load balancing.
			* Each Node keeps track of it's weight (iterations per sec)
			* using it's weight, each Node can estimate how long it will
				take to process all of the SubDomains it currently has.
			* During the balance phase, the weight information is used
				to request/give blocks based on an estimated time of
				completion that is given to the Node.
				** e.g. if you are told that the goal is to be done in
					20 seconds, then if you can process all of your
					SubDomains in 10 seconds, then you will ask for
					more SubDomains from the parent Node.
		-- Profiler : (not implemented)
		-- Model : (not used)
		-- SubDomain : contains all of the information about a decomposed
			chunk of data.
			* Each SubDomain has a unique id, and it knows the id's of all
				of it's neighbors (this is used for ghost zone exchange).
			* These were designed so that each SubDomain would know where
				it should go in the final data matrix.
	- The cell and hotspot examples are extremely similar
		-- The main idea is that the root node will set up the MPI environment
			and then launch the distributed functions on all of the nodes.
		-- Each node then will profile itself (read CPU) and its children
			nodes (read GPUs).
		-- The load balancing works as follows:
			* The root node decomposes the matrix, sends a block to all nodes.
			* The nodes "benchmark" themselves using their test block and
				send their weights back to the root.
			* The root then simulates the load balancing on the Cluster data
				structure.
				** powerful nodes request work from their parents
				** weak nodes give nodes to their parents
				** parents always give available blocks to children who need
					them.
	- The main issues to consider are as follows:
		-- how you should decompose the data
			* currently slabs are combined into large single slabs for each 
				device to minimize communication.
		-- how to accurately/quickly estimate communication latency during
			load-balancing stage.
		-- What scheme to use for distributing the SubDomains to the Nodes
			that will minimize communication cost (e.g. place neighboring
			blocks on nodes close together).
		-- how to abstract these techniques out into a general compiler template.
		-- Determine how performance scales with data size
		