#include "CommandVelocity.hpp"

// basic constructor
CommandVel::CommandVel() : cmds(), msg_seq(0), last_cmd({}), cmds_mutex() {
    // set the last current time
    last_cmd.stamp = ros::Time::now();
}
// push a new message
void CommandVel::push_back(const geometry_msgs::Twist &msg) {

    // lock the mutex
    cmds_mutex.lock();

    // creates a new Velocity
    Velocity v;

    // updates the TwistStamped
    v.linear = msg.linear.x;
    v.angular = msg.angular.z;

    // updates the header
    v.seq = msg_seq++;
    v.stamp = ros::Time::now(); // not so good... :-(

    // push to the qeue
    cmds.push(v);

    // unlock the mutex
    cmds_mutex.unlock();
}

// get the entire queue
// it stops at the last command before the current laser
std::vector<Velocity> CommandVel::getAll(const ros::Time &end) {

    // creates a new std::vector
    std::vector<Velocity> commands;

    // lock the mutex
    cmds_mutex.lock();

    // get the last command
    // let the last_cmd be the command that starts at the last LaserScan timestamp
    commands.push_back(last_cmd);

    // copy the entire vector and clear the cmds
    while(!cmds.empty()) {
        Velocity v = cmds.front();
        if (v.stamp > end) {
            break;
        } else {
            commands.push_back(v);
            cmds.pop();
        }
    }

    // set the last_cmd
    last_cmd = commands.back();

    // updates the timestamp to the new LaserScan timestamp
    last_cmd.stamp = end;

    // copy this last command to the vector with the updated timestamp
    // hack
    commands.push_back(last_cmd);

    // unlock the mutex
    cmds_mutex.unlock();

    return commands;

}