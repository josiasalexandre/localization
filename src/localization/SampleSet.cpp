#include "SampleSet.hpp"
#include <cmath>

// basic constructor
SampleSet::SampleSet(const ros::NodeHandle &private_nh) : spreaded(false), samples(nullptr) {

    // get the size parameter
    // if not found, it'll be 800'
    private_nh.param( (std::string) "sample_set_size", size, 800);

    // get the min and max samples
    private_nh.param("min_sample_set_size", min, 100);
    private_nh.param( (std::string) "max_sample_set_size", max, 10000);

    // allocate the array of 2D samples
    newSamples();

}

// destructor
SampleSet::~SampleSet() {
    delete samples;
}

// normalize the weights
void SampleSet::normalizeWeights() {

    // normalize the weights
    double normalize = 1.0/( (double) total_weight);

    for (int i = 0; i < size; i++) {

        // normalize
        samples[i].weight *= normalize;

    }

}

// allocate the samples in memmory
void SampleSet::newSamples() {

    // the sample set size limits
    if (min <= size && size <= max && nullptr == samples) {

        // allocate the memmory
        samples = new Sample2D[size]();
        if (nullptr == samples) {
            throw std::bad_alloc();
        }

        // reset the samples
        resetSamples();

    }

}

// reset all poses to zero and the weigths to 1/size
void SampleSet::resetSamples() {

    if (nullptr != samples) {

        // size must be greater than zero
        double min_w = 1.0/((double)size);

        for (int i = 0; i < size; i++) {

            samples[i].pose = { 0.0, 0.0, 0.0 };
            samples[i].weight = min_w;

        }

    }

}