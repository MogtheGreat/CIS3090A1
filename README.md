# CIS3090A1
Guelph CIS 3090 Parallel Programming Assignment One

<p>This assignment is broken into two separate programs:
<ul>
	<li>data: data.c, dataFunc.c, dataFunc.h</li>
	<li>task: task.c, taskFunc.c, taskFunc.h, queue.c, queue.h</li>
</ul>
Both programs plays Conway's Game of Life (https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) and uses parallelism for optimization. Uses pthread library. Difference between the two programs is how they use the pthreads.</p>

<h3>data</h3>
<p>Use data parallelism to split the work of running game of life. All pthreads perform the same operation but divides the work.</p>

<h3>task</h3>
<p>Uses task parallism to run the game of life using threads. All pthreads perform different operations, dividing the work between them.</p>