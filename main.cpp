#include<iostream>
#include<vector>
#include<list>
#include<math.h>

#include<fstream>
#include<chrono>
#include <algorithm>
#include <random>

#include "clustering.h"


/**
* Functions to generate a graph output using python, just utility
**/
std::ofstream python_init_graph(int grid_size, int target_size)
{
    std::ofstream stream(std::to_string(grid_size) + std::string("_") + std::to_string(target_size) + std::string("_plot.py"));
    stream << "import matplotlib.pyplot as plt" << std::endl;
    stream << "fig = plt.figure()" << std::endl;
    return stream;
}

template<typename T>
void python_plot(std::ofstream &stream, std::vector<std::pair<T,T>> &points, bool point = false)
{
    std::vector<T> xAxis;
    std::vector<T> yAxis;
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

void python_generate(std::ofstream &stream, int grid_size, int target_size)
{
    stream << "fig.savefig('" << grid_size << "_" << target_size << ".png')" << std::endl;
    stream.close();
}

/**
* Euclidian distance measure used fore our tests
**/
template<typename T>
double euclidian_distance(std::pair<T,T> &p1, std::pair<T,T> &p2)
{
    T first_param = p1.first-p2.first;
    T second_param = p1.second-p2.second;
    return sqrt(first_param*first_param + second_param*second_param);
}

/**
* A collection of test functions
**/
template<typename T>
void test_data_grid(std::vector<std::pair<T,T>> &container, int grid_size)
{
    for (int i=0; i<grid_size; ++i)
    for (int j=0; j<grid_size; ++j)
        container.push_back(std::make_pair(i,j));
}

void test_data_schaffner_n1(std::vector<std::pair<long long,long long>> &container, int grid_size)
{
    for (int i=0; i<grid_size; ++i)
        container.push_back(std::make_pair(i*i, (i-2)*(i-2)));
}

int main(int argc, char** argv)
{
    int grid_size = std::stoi(argv[1]);
    int target_size = std::stoi(argv[2]);

    std::vector<std::pair<int,int>> test_data;
    auto stream = python_init_graph(grid_size, target_size);

    test_data_grid(test_data, grid_size);
    //test_data_schaffner_n1(test_data, grid_size);
    int test_data_length = test_data.size();

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle (test_data.begin(), test_data.end(), std::default_random_engine(seed));

    auto start = std::chrono::steady_clock::now();

    //clustering(test_data, target_size, test_data_length, euclidian_distance);
    //clustering_dac(&test_data, test_data_length, target_size, euclidian_distance, 4);
    clustering_batch(&test_data, test_data_length, target_size, euclidian_distance, 360, 14);

    auto end = std::chrono::steady_clock::now();

    python_plot(stream, test_data, true);
    python_generate(stream, grid_size, target_size);

    auto diff = end - start;
    std::cout << "Algorithm runtime: " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;

    return 0;
}