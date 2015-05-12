<!--
  The Presentation File
  This file contains my presentation for calculus project
  Henry J Schmale
-->

# Applying Amdahl's law to Drawing the Mandelbrot Fractal
<img src="http://upload.wikimedia.org/wikipedia/commons/2/21/Mandel_zoom_00_mandelbrot_set.jpg" width="45%" style="margin:auto"/>

# What is Amdahl's Law
<img src="http://latex.codecogs.com/gif.latex?$$T(n)&space;=&space;T(1)(B&space;&plus;\frac{1}{n}(1-B))$$" title="$$T(n) = T(1)(B +\frac{1}{n}(1-B))$$" />
* Maximum expected improvement to system by parallelization


# What is the Mandelbrot set?
* A set of complex numbers for which a repeating pattern does **NOT**
approach infinity.
* It is described by $z_{n+1} = z_{n}^2+c$ remains bounded.
* That means that $c$ is a member of the mandelbrot set

# Rules for drawing the Mandelbrot Fractal
* Number of Iterations for the series to converge determines color
* Some cases may require millions of iterations, so a cut-off point is
  determined.

# Applications of the Mandelbrot Fractal
* Demo of simple rules creating complex thing
* Computer Benchmark
* A test of parallelization
