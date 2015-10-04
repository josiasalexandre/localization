#ifndef SAMPLE_MOTION_MODEL_VELOCITY_H
#define SAMPLE_MOTION_MODEL_VELOCITY_H

#include "pose.hpp"
#include "sampleMotionModel.hpp"
#include "commandVelocity.hpp"
#include "VelocityModelParameters.hpp"

class SampleVelocityModel : public SampleMotionModel {

    // the command reader
    CommandVel cmd;

    // parameters
    double a1, a2, a3, a4, a5, a6;
    double deltaT;


    public:
        // constructor
        SampleVelocityModel();

        // updates the pose to a new one based on the CommandVel
        virtual void samplePose2D(Pose2D *pose);

};

#endif