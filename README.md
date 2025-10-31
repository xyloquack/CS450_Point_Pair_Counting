## Point Pair Counting ##
This project was my entry for a class competition in CS450 Parallel Programming to create the most performant algorithm for a point-pair counting problem, and this was the winning solution within the class.

This is an implementation for utilizing a spatial grid to efficiently reduce the searching space for a point pair counting problem.

### Why does this work? ###
The reason that this works is because it reduces the search space of each point's comparisons from being against every other point, which would be an O(N^2) best case problem.
Changing to a spatial grid changes things into only needing to check against points that are within its own and the surrounding eight grid squares that are partitioning the space, bringing the problem to a worst case O(N^2) problem, which would be where the search distance encompasses the every point within a single grid square.

### What was the performance difference over a brute force solution? ###
A measured brute force solution on the same system ranged from being around 500x slower, to being almost 2000x slower depending on the system and its underlying architecture.

### Were there other considerations made? ###
- A lot of approaches were considered, starting off with a check that ruled out points that were greater than the epsilon search distance in just the x or y position, which did reduce computation time initially, but compromises like that had to be changed later because eventually in became a bottleneck.
- Another large consideration was the memory allocation of the application. At first, the program was designed to simply allocate memory that could fit every point within every grid square, but this proved to be slower than counting the amount of points that would end up in each grid square and then allocating the data by a significant margin.
- Then there were small changes, like changing the way that counting happened by passing the pointer to the count variable instead of individually counting the grid squares within the functions and summing to the count outside the function because the performance cost of setting up those temporary variables was too costly with how much it was happening.
  - This optimization extended to a few other data points, where passing the pointer to the struct for the points was more efficient than passing the data into a temporary function-scope variable
- Changing from iterating through the original data array was found to be significantly slower in testing than iterating through the allocated grid because of the cache hits that occured in the more ordered grid array compared to the randomly arranged data array.
