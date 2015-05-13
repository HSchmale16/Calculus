---
title   : Applying Amdahl's Law to the Mandelbrot Fractal
author  : Henry J Schmale
---

<img src="http://upload.wikimedia.org/wikipedia/commons/2/21/Mandel_zoom_00_mandelbrot_set.jpg" width="45%" style="margin:auto"/>

# What is Amdahl's Law
$$T(n) = T(1)(B +\frac{1}{n}(1-B))$$

* Maximum expected improvement to system by parallelization
* Named after Gene Amdahl
* $T(n)$ is the time an algorithim takes to complete
* $n \epsilon N$ number of threads of execution
* $B \epsilon [0,1]$ Percent of algorithim that is serial

# What is the Mandelbrot set?
* A set of complex numbers for which a repeating pattern does **NOT**
approach infinity.
* Described by where $z_{n+1} = z_{n}^2+c$ remains bounded
* That means that $c$ is a member of the mandelbrot set

# Rules for drawing the Mandelbrot Fractal
* Number of Iterations for the series to converge determines color
* Some cases may require millions of iterations, so a cut-off point is
  determined.

# Applications of the Mandelbrot Fractal
* Demo of simple rules creating complex thing
* Computer Benchmark
* A test of parallelization

# Applying Amdahl's Law
* The drawing of the mandelbrot fractal can easily be parallelized
* As such amdahl's law applies to it
1* Writing to disk and displaying to screen is strictly serial.

