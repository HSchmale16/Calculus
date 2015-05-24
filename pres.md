---
title   : Applying Amdahl's Law to the Mandelbrot Fractal
author  : Henry J Schmale
---

<img src="http://upload.wikimedia.org/wikipedia/commons/2/21/Mandel_zoom_00_mandelbrot_set.jpg" width="45%" style="margin:auto"/>

# What is Amdahl's Law
$$T(n) = T(1)(B +\frac{1}{n}(1-B))$$

* Maximum expected improvement to system by parallelization
* Named after Gene Amdahl
* $T(n)$ is the time an algorithm takes to complete
* $n \epsilon N$ number of threads of execution
* $B \epsilon [0,1]$ Percent of algorithm that is serial

# What is the Mandelbrot set?
* A set of complex numbers for which a repeating pattern does **NOT**
approach infinity.
* Described by where $z_{n+1} = z_{n}^2+c$ remains bounded
* That means that $c$ is a member of the Mandelbrot set

# Rules for drawing the Mandelbrot Fractal
* Number of Iterations for the series to converge determines color
* Some cases may require millions of iterations, so a cut-off point is
  determined.

# Applications of the Mandelbrot Fractal
* Demo of simple rules creating complex thing
* Computer Benchmark
* A test of parallelization

# Applying Amdahl's Law
* The drawing of the Mandelbrot fractal can easily be parallelized
* As such Amdahl's law applies to it
* Certain parts are strictly serial operations

# Strictly Serial Operations in drawing the Mandelbrot fractal
* Drawing to the screen
* Managing Threads
* Writing to disk

# Operations that can be parallelized
* Drawing the image
    * Each frame can be drawn in separate threads
    * Each row/col could also been drawn in own thread
* Calculating the Mandelbrot value for each pixel

