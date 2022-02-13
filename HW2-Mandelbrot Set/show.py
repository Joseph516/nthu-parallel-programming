import numpy as np 
import matplotlib.pyplot as plt

if __name__ == "__main__":
    filename = "output_file"
    f = open(filename, 'r')

    res = np.array([])
    line = f.readline().rstrip()
    n_row = len(line.split(" "))
    while line:
        values = line.split(" ")
        res = np.append(res, values)    
        line = f.readline().rstrip()
    f.close()

    res = res.reshape(-1, n_row).astype(np.int)
    plt.figure(1)
    plt.imshow(res.T)
    plt.savefig("set.png")
