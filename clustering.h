#include<iostream>
#include<vector>
#include<math.h>
#include<iterator>
#include <thread>

/**
* Implementation of the clustering algorithm
**/
template <typename T>
void clustering(std::vector<T> &data, int N_max_size_external_set, double (*distance_measure)(T&,T&))
{
    // Step 1: 
    // Create a set of sets of clusters
    std::vector<std::vector<T>> c_cluster_set;
    int cluster_size = data.size();
    c_cluster_set.reserve(cluster_size);
    
    for (auto &entry : data)
    {
        std::vector<T> cluster;
        cluster.reserve(1);
        cluster.push_back(entry);
        c_cluster_set.push_back(std::move(cluster));
    }

    // Step 2: Goto step 5 if the set contains lesser or equal to max specified number
    std::vector<std::vector<double>> solution_set;
    solution_set.reserve(cluster_size);
    for (int i=0; i<cluster_size; ++i)
    {
        std::vector<double> inner_solution_set;
        inner_solution_set.reserve(cluster_size-i);
        for (int j=0; j<cluster_size-i-1; ++j)
            inner_solution_set.push_back(0.0);
        solution_set.push_back(std::move(inner_solution_set));
    }

    // Step 3: Initialization
    int cluster_solution_index;
    // Using symmetry here, still O(N/2 * N/2)
    for (int i=0; i<cluster_size; ++i)
    {
        auto &cluster_1 = c_cluster_set[i];
        int cluster_1_size = cluster_1.size();

        for (int j=0; j<cluster_size-(i+1); ++j)
        {
            auto &cluster_2 = c_cluster_set[cluster_size-1-j];

            // This is supposed to be this d function
            double distance = 0.0;
            for (auto &cluster_1_entry : cluster_1)
            for (auto &cluster_2_entry : cluster_2)
                distance += distance_measure(cluster_1_entry, cluster_2_entry);
            distance = (1.0/(cluster_1_size*cluster_2.size())) * distance;
            solution_set[i][j] = std::move(distance);
        }
    }
    goto cluster_initial_skip;
    for (; cluster_size > N_max_size_external_set; --cluster_size)
    {
        {
            // Step 3: Find the two sets with the minimum distance to each other
            auto &cluster_1 = c_cluster_set[cluster_solution_index];
            int cluster_1_size = cluster_1.size();
            // Here we need to reevaluate in two directions
            // First to the right
            for (int j=cluster_size-1; j>cluster_solution_index; --j)
            {
                auto &cluster_2 = c_cluster_set[j];

                // This is supposed to be this d function
                double distance = 0.0;
                for (auto &cluster_1_entry : cluster_1)
                for (auto &cluster_2_entry : cluster_2)
                    distance += distance_measure(cluster_1_entry, cluster_2_entry);
                distance *= (1.0/(cluster_1_size*cluster_2.size()));
                solution_set[cluster_solution_index][(cluster_size-1)-j] = std::move(distance);
            }

            // Now down the graph
            for (int j=cluster_solution_index - 1; j>=0; --j)
            {
                auto &cluster_2 = c_cluster_set[j];

                // This is supposed to be this d function
                double distance = 0.0;
                for (auto &cluster_1_entry : cluster_1)
                for (auto &cluster_2_entry : cluster_2)
                    distance += distance_measure(cluster_1_entry, cluster_2_entry);
                distance *= (1.0/(cluster_1_size*cluster_2.size()));
                solution_set[j][cluster_size-1-cluster_solution_index] = std::move(distance);
            }
        }
        
        cluster_initial_skip:
        
        // Step 4: Find pair with minimal distance
        int min_distance_x = 0;
        int min_distance_y = 0;
        double min_distance = solution_set[0][0];
        int solution_set_real_size_x = solution_set.size();
        for (int x=0; x<solution_set_real_size_x; ++x)
        {
            int solution_set_real_size_y = solution_set[x].size();
            for (int y=0; y<solution_set_real_size_y; ++y)
            {
                auto& distance = solution_set[x][y];
                if (distance >= min_distance)
                    continue;

                min_distance_x = x;
                min_distance_y = y;
                min_distance = distance;
            }
        }

        // Now merge the sets and create a new clusterset
        int min_distance_y_real = cluster_size-1-min_distance_y;
        if (min_distance_x < min_distance_y_real)
        {
            c_cluster_set[min_distance_x].insert(
                c_cluster_set[min_distance_x].end(),
                std::make_move_iterator(c_cluster_set[min_distance_y_real].begin()),
                std::make_move_iterator(c_cluster_set[min_distance_y_real].end())
            );
            cluster_solution_index = min_distance_x;
            c_cluster_set.erase(c_cluster_set.begin()+min_distance_y_real); 

            for (int q=0; q<cluster_size && min_distance_y < cluster_size-1-q; ++q)
                solution_set[q].erase(solution_set[q].begin()+min_distance_y);
            solution_set.erase(solution_set.begin()+min_distance_y_real);
        }
        else
        {
            c_cluster_set[min_distance_y_real].insert(
                c_cluster_set[min_distance_y_real].end(),
                std::make_move_iterator(c_cluster_set[min_distance_x].begin()),
                std::make_move_iterator(c_cluster_set[min_distance_x].end())
            );
            cluster_solution_index = min_distance_y_real;
            c_cluster_set.erase(c_cluster_set.begin()+min_distance_x); 

            for (int q=0; q<cluster_size && min_distance_x <= cluster_size-1-q; ++q)
                solution_set[q].erase(solution_set[q].begin()+min_distance_x);
            solution_set.erase(solution_set.begin()+min_distance_x);
        }
    }

    // Step 5: Return set of center points of each cluster
    // Symmetry can also be exploited here, but worth the effort?
    data.clear();
    data.reserve(N_max_size_external_set);
    for(auto &cluster : c_cluster_set)
    {
        // This is supposed to be this d function
        double min_distance = 0.0;
        auto &min_distance_pair = cluster[0];

        int inner_cluster_size = cluster.size();
        for (int i=1; i<inner_cluster_size; ++i)
            min_distance += distance_measure(min_distance_pair, cluster[i]);
        

        for (int i=0; i<inner_cluster_size; ++i)
        {
            auto &cluster_entry_1 = cluster[i];
            double distance = 0.0;
            for (int j=0; j<i; ++j)
                distance += distance_measure(cluster_entry_1, cluster[j]);

            for (int j=i+1; j<inner_cluster_size; ++j)
                distance += distance_measure(cluster_entry_1, cluster[j]);

            if (min_distance <= distance)
                continue;

            min_distance = std::move(distance);
            min_distance_pair = cluster_entry_1;
        }
        data.push_back(std::move(min_distance_pair));
    }
}

/**
* Singlethreaded divide and conquer
**/
template <typename T>
void clustering_dac(std::vector<T> &data, int data_length, int N_max_size_external_set, double (*distance_measure)(T&,T&))
{
    int half_length = static_cast<int>(data_length / 2);
    if (half_length > N_max_size_external_set)
    {
        std::vector<T> left(data.begin(), data.begin()+half_length);
        std::vector<T> right(data.begin()+half_length, data.end());
        clustering_dac(left, half_length, N_max_size_external_set, distance_measure);
        clustering_dac(right, data_length-half_length, N_max_size_external_set, distance_measure);
        left.insert(
            left.end(),
            std::make_move_iterator(right.begin()),
            std::make_move_iterator(right.end())
        );
        data = left;
    }
    clustering(data, N_max_size_external_set, distance_measure);
}

/**
* Multithreaded divide and conquer
**/
template <typename T>
void clustering_dac_multithread(std::vector<T> *data, int data_length, int N_max_size_external_set, double (*distance_measure)(T&,T&), int max_depth, int depth = 0)
{
    int half_length = static_cast<int>(data_length / 2);
    if (half_length > N_max_size_external_set)
    {
        std::vector<T> left(data->begin(), data->begin()+half_length);
        std::vector<T> right(data->begin()+half_length, data->end());

        if (depth < max_depth)
        {
            // One thread for each division
            std::thread thread_left(&clustering_dac_multithread<T>, &left, half_length, N_max_size_external_set, distance_measure, max_depth, depth+1);
            std::thread thread_right(&clustering_dac_multithread<T>, &right, data_length-half_length, N_max_size_external_set, distance_measure, max_depth, depth+1);

            // Joining threads
            thread_left.join();
            thread_right.join();
        }else{
            clustering_dac_multithread(&left, half_length, N_max_size_external_set, distance_measure, max_depth, depth);
            clustering_dac_multithread(&right, data_length-half_length, N_max_size_external_set, distance_measure, max_depth, depth);
        }

        left.insert(
            left.end(),
            std::make_move_iterator(right.begin()),
            std::make_move_iterator(right.end())
        );
        *data = left;
    }
    clustering(*data, N_max_size_external_set, distance_measure);
}