#ifndef MAP_H
#define MAP_H

#include <mutex>

#include <nav_msgs/OccupancyGrid.h>
#include <ros/node_handle.h>

#include "SampleSet.hpp"
#include "GridMap.hpp"

class Map {

    private:

        // our internal GridMap/OccupancyGrid representation
        GridMap grid;

        // mutex to lock map
        std::mutex map_mutex;

        // get the free cells
        std::vector<int> availableCells;

        // the likelihood field flag
        bool likelihood_flag;

        // random
        // set another uniform distribution from -PI to PI
        std::uniform_real_distribution<double> angle_dist;

        // set a normal distribution
        std::normal_distribution<double> normal_dist;

        // set a generator engine
        std::default_random_engine generator;

        // build the likelihood
        void buildGridMap();

        // update the availableCells
        void updateAvailableCells();

    public:

        // basic constructor
        Map(const ros::NodeHandle&);

        // updates the map
        void updateMap(const nav_msgs::OccupancyGrid&);

        // update max_occ_dist
        void updateMaxOccDist(double);

        // returns the grid
        void getGridMap(GridMap *);

        // spreads the particles over the entire map, randomly
        void uniformSpread(SampleSet*);

        // returns an random Pose2D inside the available cells
        Pose2D randomPose2D();

        // export grid map to ros msg
        void export_grid_map(nav_msgs::OccupancyGrid&);

};

#endif
