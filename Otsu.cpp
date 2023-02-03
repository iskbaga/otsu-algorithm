#include <iostream>
#include <omp.h>
#include <vector>
#include <fstream>
#include <string>
using namespace std;
int main(int arg, char* argv[]) {
    vector<string> args(arg);
    vector<int> gist(256);
    vector<int> gistSum(257);
    vector<int> brightSum(257);
    string format;
    bool isOmp = true;
    int width = 0;
    int height = 0;
    int bright = 0;
    int minBright = 255;
    int maxBright = 0;
    for (int i = 0; i < arg; i++) {
        args[i] = argv[i];
    }
    ifstream input(args[2]);
    ofstream output(args[3]);
    input >> format >> width >> height >> bright;
    if (format != "P5") {
        cerr << "Incorrect file format\n";
        return 0;
    }
    int numThreads = stoi(args[1]);
    if (numThreads == -1) {
        numThreads = 1;
        isOmp = false;
    } else if (numThreads == 0) {
        numThreads = omp_get_max_threads();
    } else if (numThreads < -1) {
        cerr << "Incorrect number of threads\n";
        return 0;
    }
    if (bright != 255) {
        cerr << "Incorrect brightness\n";
        return 0;
    }
    vector<int> x(numThreads);
    vector<int> y(numThreads);
    vector<int> z(numThreads);
    vector<double> maxThreadDisp(numThreads);
    input.get();
    vector<int> data(width * height);
    for (int i = 0; i < width * height; i++) {
        data[i] = input.get();
    }
    double start = omp_get_wtime();
#pragma omp parallel if(isOmp) num_threads(numThreads)
    {
        vector<int> temp(256);
#pragma omp for schedule(static)
        for (int i = 0; i < width * height; i++) {
            temp[data[i]]++;
        }
#pragma omp critical
        {
            for (int i = 0; i < 256; i++) {
                gist[i] += temp[i];
            }
        }
    }
    for (int i = 0; i < 256; i++) {
        if (gist[i] > 0) {
            if (minBright > i) {
                minBright = i;
            } else if (maxBright < i) {
                maxBright = i;
            }
        }
    }
    for (int i = 0; i < 256; i++) {
        gistSum[i + 1] = gistSum[i] + gist[i];
        brightSum[i + 1] = brightSum[i] + gist[i] * i;
    }
#pragma omp parallel for schedule(dynamic) num_threads(numThreads) if(isOmp)
    for (int i = 0; i < 256; i++) {
        for (int j = i + 1; j < 256; j++) {
            for (int k = j + 1; k < 256; k++) {
                int id = omp_get_thread_num();
                double dispertion = 0;
                double probability, average;
                probability = (double) (gistSum[i + 1] - gistSum[minBright]) / (width * height);
                average = (double) (brightSum[i + 1] - brightSum[minBright]) /
                          (gistSum[i + 1] - gistSum[minBright]);
                dispertion += probability * average * average;
                probability = (double) (gistSum[j + 1] - gistSum[i + 1]) / (width * height);
                average = (double) (brightSum[j + 1] - brightSum[i + 1]) /
                          (gistSum[j + 1] - gistSum[i + 1]);
                dispertion += probability * average * average;
                probability = (double) (gistSum[k + 1] - gistSum[j + 1]) / (width * height);
                average = (double) (brightSum[k + 1] - brightSum[j + 1]) /
                          (gistSum[k + 1] - gistSum[j + 1]);
                dispertion += probability * average * average;
                probability = (double) (gistSum[maxBright + 1] - gistSum[k + 1]) / (width * height);
                average = (double) (brightSum[maxBright + 1] - brightSum[k + 1]) /
                          (gistSum[maxBright + 1] - gistSum[k + 1]);
                dispertion += probability * average * average;
                if (dispertion > maxThreadDisp[id]) {
                    maxThreadDisp[id] = dispertion;
                    x[id] = i;
                    y[id] = j;
                    z[id] = k;
                }
            }
        }
    }
    int maxDisp = 0;
    int X, Y, Z;
    for (int i = 0; i < numThreads; i++) {
        if (maxThreadDisp[i] > maxDisp) {
            maxDisp = maxThreadDisp[i];
            X = x[i], Y = y[i], Z = z[i];
        }
    }
#pragma omp parallel for schedule(static) num_threads(numThreads) if(isOmp)
    for (int i = 0; i < width * height; i++) {
        if (data[i] <= X) {
            data[i] = 0;
        } else if (data[i] <= Y) {
            data[i] = 84;
        } else if (data[i] <= Z) {
            data[i] = 170;
        } else {
            data[i] = 255;
        }
    }
    double end = omp_get_wtime();
    printf("Time (%i thread(s)): %g ms\n", numThreads, 1000 * (end - start));
    output << format << "\n" << width << " " << height << "\n" << bright << "\n";
    for (int i = 0; i < width * height; i++) {
        output.put(data[i]);
    }
}

