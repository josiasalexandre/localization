#include "ParticleFilter.hpp"

// Constructor
ParticleFilter::ParticleFilter() : 
                                    nh(), 
                                    private_nh("~"), 
                                    cmd_vel(),
                                    cmd_odom(),
                                    laser(),
                                    map(), loop_rate(10) {

    // Motion model
    std::string motionModel;
    private_nh.param<std::string>("sample_motion_model", motionModel, "vel");
    if (0 == motionModel.compare("odom")) {
        // the default constructor
        motion = new SampleOdometryModel(private_nh, &cmd_odom);
    } else {
        // the default constructor
        motion = new SampleVelocityModel(private_nh, &cmd_vel);
    }

    // badd allocation?
    if (nullptr == motion) {
        throw std::bad_alloc();
    }


    // Measurement Model
    std::string measurementModel;
    private_nh.param<std::string>("measurement_model", measurementModel, "likelihood");
    if (0 == measurementModel.compare("beam")) {

        // default constructor
        measurement = new BeamRangeFinderModel(private_nh, &laser, &map);

    } else {

        // get the max_occ_dist
        double max_occ_dist;
        private_nh.param("map_max_occ_distance", max_occ_dist, 3.0);
        // update map max_occ_dist
        map.updateMaxOccDist(max_occ_dist);

        // default constructor
        measurement = new LikelihoodFieldModel(private_nh, &laser, &map);
    }

    // badd allocation?
    if (nullptr == measurement) {
        throw std::bad_alloc();
    }

    // The MCL object
    std::string mcl_version;
    private_nh.param<std::string>("monte_carlo_version", mcl_version, "augmented");

    if (0 == mcl_version.compare("normal")) {
        mcl = new MonteCarloLocalization(private_nh, motion, measurement);
    } else {
        mcl = new AugmentedMonteCarloLocalization(private_nh, motion, measurement);
    }

    if (nullptr == mcl) {
        throw std::bad_alloc();
    }

    // now, with all objects initialized we can start to the subscribe the topics
    // get the laser topic name
    std::string laser_topic;
    private_nh.param<std::string>("laser_scan_topic", laser_topic, "scan");
    // subscribe to the laser scan topic
    laser_sub = nh.subscribe(laser_topic, 1, &ParticleFilter::laserReceived, this);

    // get the command topic name
    std::string cmd_topic;
    private_nh.param<std::string>("motion_model_command_topic", cmd_topic, "cmd_vel");

    // subscribe to the command topic
    if (0 == cmd_topic.compare("odom")) {
        cmd_sub = nh.subscribe(cmd_topic, 10, &ParticleFilter::commandOdomReceived, this);
    } else {
        cmd_sub = nh.subscribe(cmd_topic, 10, &ParticleFilter::commandVelReceived, this);
    }

    // get the map topic name
    std::string map_topic;
    private_nh.param<std::string>("map_server_topic", map_topic, "map");
    // subscribe to the map topic
    map_sub = nh.subscribe(map_topic, 1, &ParticleFilter::readMap, this);

    // get the PoseArray topic name
    std::string pose_array_topic;
    private_nh.param<std::string>("pose_array_topic", pose_array_topic, "pose_array");

    // set the pose_array_pub, the correct topic and messages
    pose_array_pub = nh.advertise<geometry_msgs::PoseArray>(pose_array_topic, 10);

}

// Destructor
ParticleFilter::~ParticleFilter() {

    // delete the motion model
    if (nullptr != motion) {
        delete motion;
    }

    // delete the measurement model
    if (nullptr != measurement) {
        delete measurement;
    }

    //delete the MCL
    if (nullptr != mcl) {
        delete mcl;
    }

}

// base constructor

// callbacks
// laser received callback
void ParticleFilter::laserReceived(const sensor_msgs::LaserScan &msg) {

    // the laser object manages the apropriate mutex
    laser.setLaserScan(msg);

    if(map.mapReceived()) {
        // starts the MCL
        mcl->start();
    }
}

// the velocity motion command 
void ParticleFilter::commandVelReceived(const geometry_msgs::Twist &msg) {

    // push the new command to the command vector
    cmd_vel.push_back(msg);
}

// the odometry motion command
void ParticleFilter::commandOdomReceived(const nav_msgs::Odometry &msg) {

    /* TODO */
    //edit the new_pose received
    cmd_odom.setNew_pose(msg);


}

// the occupancy grid
void ParticleFilter::likelihoodMapReader(const nav_msgs::OccupancyGrid &msg) {

    // copy the OccupancyGrid to the Map
    if(map.updateMap(msg)) {
        // spread the particles
        // passing by the MCL and we get a free lock =)
        map.nearestNeighbor()
        mcl->spreadSamples(map);
    }
}

void ParticleFilter::beamMapReader(const nav_msgs::OccupancyGrid &msg) {

    // copy the OccupancyGrid to the Map
    if(map.updateMap(msg)) {
        // spread the particles
        // passing by the MCL and we get a free lock =)
        mcl->spreadSamples(map);
    }
}



// publish the poses - the PoseArray Publisher
void ParticleFilter::publishPoseArray() {

    // get the Poses
    geometry_msgs::PoseArray msg = mcl->getPoseArray();

    // publish
    pose_array_pub.publish(msg);
}

// run the particle
void ParticleFilter::start() {

    // the asynchronous spinner
    ros::AsyncSpinner spinner(1);

    // start spinning
    spinner.start();

    while(ros::ok()) {

        // publish the pose array
        publishPoseArray();

        // sleep
        loop_rate.sleep();

    }
//     // wait for Control + C
//     ros::waitForShutdown();

}
