#ifndef Likelihood_FIELD_MODEL_H
#define Likelihood_FIELD_MODEL_H

#include <ros/ros.h>

#include "MeasurementModel.hpp"


class LikelihoodFieldModel : public MeasurementModel {

    private:
        // LikelihoodFieldModel parameters
        double z_hit, z_max, z_rand, sigma_hit;
        // helpers
        double z_rand_max, sigma_hit2, prob, z_hit_denon;

    public:
        // basic constructor
        LikelihoodFieldModel(ros::NodeHandle&, Laser*, Map*);

        // base class abstract method implementation
        virtual void getWeight(Sample2D *);

        // update the LaserScan
        virtual ros::Time update();

};

#endif