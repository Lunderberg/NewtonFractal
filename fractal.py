#!/usr/bin/env python3

import math
import numpy
import pylab

def numpy_nan(shape):
    out = numpy.zeros(shape=shape)
    out.fill(numpy.NAN)
    return out

class NewtonFractal:
    def __init__(self, zeros):
        self.zeros = zeros
        self.poly = numpy.poly1d(numpy.poly(self.zeros))
        self.polyder = numpy.polyder(self.poly)

        self.colors = ['Blues_r', 'Oranges_r', 'Greens_r', 'Purples_r', 'Greys_r']

        self.threshold = 1e-5

    def eval(self, guess):
        iterations = 0
        max_iter = 250
        for _ in range(max_iter):
            iterations += 1
            polyval = self.poly(guess)
            if abs(polyval) < self.threshold:
                break
            guess -= polyval/self.polyder(guess)
        else:
            return -1, max_iter

        zero_index = min(enumerate(self.zeros), key = lambda z:abs(guess-z[1]))[0]
        return zero_index, iterations

    def plot(self, xmin, xmax, xres, ymin, ymax, yres):
        x,y = numpy.mgrid[xmin:xmax:xres,
                          ymin:ymax:yres]
        z = x + (1j)*y
        iter_counts = [numpy_nan(z.shape) for _ in self.zeros]

        for xi in range(z.shape[0]):
            for yi in range(z.shape[1]):
                zero_index, iterations = self.eval(z[xi,yi])
                if zero_index >= 0:
                    iter_counts[zero_index][xi,yi] = math.log(iterations)

        for iters,cmap in zip(iter_counts, self.colors):
            iters = numpy.ma.array(iters, mask=numpy.isnan(iters))
            pylab.pcolor(iters, cmap=cmap)


if __name__=='__main__':
    fractal = NewtonFractal([1,-1,0+1j,0-1j])
    fractal.plot(-2,2,0.025, -2,2,0.025)
    #pylab.show()
