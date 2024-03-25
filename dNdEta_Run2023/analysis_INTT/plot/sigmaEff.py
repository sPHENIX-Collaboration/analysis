import numpy as np
import math

def sigmaEff(v, threshold, xmin, xmax):
	v = np.sort(v)

	total = np.size(v)
	max = threshold * total

	start = []
	stop = []
	width = []

	i = 0 
	while i != np.size(v)-1:
		count = 0
		j = i
		while j != np.size(v)-1 and count < max:
			count +=1
			j += 1

		if j != np.size(v)-1:
			start.append(v[i])
			stop.append(v[j])
			width.append(v[j] - v[i])

		i += 1

	npminwidth = np.array(width)

	minwidth = np.amin(npminwidth)
	pos = np.argmin(npminwidth)

	xmin += [start[pos]]
	xmax += [stop[pos]]

	return minwidth


def minimum_size_range(file_sizes, percentage):
    # calculate how many files need to be in the range
    window_size = math.ceil(len(file_sizes) * percentage / 100)

    sorted_sizes = sorted(file_sizes)

    # initialize variables with worst-case values
    min_size, max_size = sorted_sizes[0], sorted_sizes[-1]
    min_interval = max_size - min_size

    # calculate interval for every window
    for i in range(len(sorted_sizes) - (window_size - 1)):
        lower, upper = sorted_sizes[i], sorted_sizes[i + (window_size - 1)]
        interval = upper - lower

        # if we found a new minimum interval, replace values
        if interval < min_interval:
            min_interval = interval
            min_size, max_size = lower, upper

    return min_size, max_size