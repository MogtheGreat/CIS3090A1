# CIS3090A1
<h1>CIS 3090 Parallel Programming Assignment One</h1>
<h2>Intro</h2>
<div>
	<p>This assignment is broken into two separate programs:
	<ul>
		<li>data: data.c, dataFunc.c, dataFunc.h</li>
		<li>task: task.c, taskFunc.c, taskFunc.h, queue.c, queue.h</li>
	</ul>
	Both programs plays Conway's Game of Life (https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) and uses parallelism for optimization. Uses pthread library. Difference between the two programs is how they use the pthreads.</p>
	<h3>data</h3>
	<div>
		<p>Use data parallelism to split the work of running game of life. All pthreads perform the same operation but divides the work.</p>
	</div>
	<h3>task</h3>
	<div>
		<p>Uses task parallism to run the game of life using threads. All pthreads perform different operations, dividing the work between them.</p>
	</div>
</div>
<h2>Requirements</h2>
<div>
	<ul>
		<li>OS: Unix (Ubuntu prefered)</li>
		<li>c compiler (gcc preferred)</li>
		<li>pthread library</li>
	</ul>
	<p>Note: pthread library is already installed on Ubuntu 15.10 or newer. For older version use the following commands:</p>
	<code>sudo apt-get install libpthread-stubs0-dev</code>
</div>
<h2>To Run</h2>
<div>
	<p>To compile both program, move into the main directory where the source code is located and type make. The makefile should compile both programs with no warnings. </p>
	<p>To run the data parallel program, type in: <code>./data Numthreads(int) SizeGrid(int) NumbIterate(int)</code> where:</p>
	<ul>
		<li>Numthreads is the number of threads the program will generate. Must be greater than 0.</li>
		<li>SizeGrid is the size of the grid. Must be greater than 0.</li>
		<li>NumIteration is the number of steps the game of life goes through. Must be greater than 0.</li>
	</ul>
	<p>To run the task parallel program, type in: <code>./task SizeGrid(int) NumbIterate(int)</code> where:</p>
	<ul>
		<li>SizeGrid is the size of the grid. Must be greater than 0.</li>
		<li>NumIteration is the number of steps the game of life goes through. Must be greater than 0.</li>
	</ul>
	<p>To run either programs with preset command line arguments type in</p>
	<ul>
		<li><code>make test</code> to run the data parallel program</li>
		<li><code>make test3</code> to run the task parallel program</li>
	</ul>
</div>
<h2>To Clean</h2>
<div>
	<p>To clean up the project, move into the main directory and type make clean to clean up the entire project.</p>
</div>
