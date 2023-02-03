# otsu-algorithm

Threshold filtering of an image by the Otsu method


Stdout+stderr (./omp4 0 in.pgm out0.pgm):
```
OK [program completed with code 0]
    [STDERR]:  
    [STDOUT]: Time (2 thread(s)): 18.6396 ms

```
     
Stdout+stderr (./omp4 -1 in.pgm out-1.pgm):
```
OK [program completed with code 0]
    [STDERR]:  
    [STDOUT]: Time (1 thread(s)): 37.0651 ms

```

Input image:

![Input image](test_data/in.png?sanitize=true&raw=true)

Output image 0:

![Output image 0](test_data/out0.pgm.png?sanitize=true&raw=true)

Output image -1:

![Output image -1](test_data/out-1.pgm.png?sanitize=true&raw=true)
