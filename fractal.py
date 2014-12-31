#!/usr/bin/env python3

import math
import numpy
import pylab

import cfractal

def numpy_nan(shape):
    out = numpy.zeros(shape=shape)
    out.fill(numpy.NAN)
    return out

class NewtonFractal:
    def __init__(self, zeros):
        self.zeros = zeros
        self.cfractal = cfractal.NewtonFractal(self.zeros)

        self.colors = ['Blues_r', 'Oranges_r', 'Greens_r', 'Purples_r', 'Greys_r']

    def plot(self, xmin, xmax, xres, ymin, ymax, yres):
        x,y = numpy.mgrid[xmin:xmax:xres,
                          ymin:ymax:yres]
        z = x + (1j)*y
        iter_counts = [numpy_nan(z.shape) for _ in self.zeros]

        for xi in range(z.shape[0]):
            for yi in range(z.shape[1]):
                zero_index, iterations = self.cfractal.eval(z[xi,yi])
                if zero_index >= 0:
                    iter_counts[zero_index][xi,yi] = math.log(iterations+1)

        for iters,cmap in zip(iter_counts, self.colors):
            iters = numpy.ma.array(iters, mask=numpy.isnan(iters))
            pylab.pcolor(iters, cmap=cmap)


if __name__=='__main__':
    fractal = NewtonFractal([1,-1,0+1j,0-1j])
    fractal.plot(-2,2,0.01, -2,2,0.01)
    pylab.show()
