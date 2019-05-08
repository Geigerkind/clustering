#include<iostream>
#include<vector>
#include<math.h>

#include<fstream>
#include<chrono>

#include "clustering.h"

/**
* Functions to generate a graph output using python, just utility
**/
std::ofstream python_init_graph(std::size_t grid_size, std::size_t target_size)
{
    std::ofstream stream(std::to_string(grid_size) + std::string("_") + std::to_string(target_size) + std::string("_plot.py"));
    stream << "import matplotlib.pyplot as plt" << std::endl;
    stream << "fig = plt.figure()" << std::endl;
    return stream;
}
void python_plot(std::ofstream &stream, std::vector<std::pair<int,int>> &points, bool point = false)
{
    std::vector<int> xAxis;
    std::vector<int> yAxis;
    for (auto entry : points)
    {
        xAxis.push_back(entry.first);
        yAxis.push_back(entry.second);
    }

    stream << "plt.plot([";
    for (std::size_t q = 0; q< xAxis.size()-1; ++q)
        stream << xAxis[q] << ",";
    stream << xAxis[xAxis.size()-1] << "]";

    stream << ",[";
    for (std::size_t q = 0; q< yAxis.size()-1; ++q)
        stream << yAxis[q] << ",";
    stream << yAxis[yAxis.size()-1] << "]" << (point ? ",'o'" : "") << ")" << std::endl;
}
void python_generate(std::ofstream &stream, std::size_t grid_size, std::size_t target_size)
{
    stream << "fig.savefig('" << grid_size << "_" << target_size << ".png')" << std::endl;
    stream.close();
}

/**
* Euclidian distance measure used fore our tests
**/
double euclidian_distance(std::pair<int,int> &p1, std::pair<int,int> &p2)
{
    int first_param = p1.first-p2.first;
    int second_param = p1.second-p2.second;
    return sqrt(first_param*first_param + second_param*second_param);
}

int main(int argc, char** argv)
{
    std::size_t grid_size = std::stoi(argv[1]);
    std::size_t target_size = std::stoi(argv[2]);

    std::vector<std::pair<int,int>> testData;
    auto stream = python_init_graph(grid_size, target_size);

    for (std::size_t i=0; i<grid_size; ++i)
    for (std::size_t j=0; j<grid_size; ++j)
        testData.push_back(std::make_pair(i,j));

    auto start = std::chrono::steady_clock::now();

    clustering_dac(testData, testData.size(), target_size, euclidian_distance);

    auto end = std::chrono::steady_clock::now();

    python_plot(stream, testData, true);
    python_generate(stream, grid_size, target_size);

    auto diff = end - start;
    std::cout << "Algorithm runtime: " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

    return 0;
}