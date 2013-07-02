/* morse_plan.cpp */

// Tests the OMPL MORSE extension without invoking MORSE

#include "ompl/extensions/morse/MorseEnvironment.h"
#include "ompl/extensions/morse/MorseStateSpace.h"
#include "ompl/extensions/morse/MorseControlSpace.h"
#include "ompl/extensions/morse/MorseSimpleSetup.h"
#include "ompl/extensions/morse/MorseGoal.h"
#include "ompl/util/Console.h"
#include "ompl/util/ClassForward.h"

#include <vector>

using namespace ompl;

class MyEnvironment : public base::MorseEnvironment
{
public:
    MyEnvironment(const unsigned int rigidBodies, const unsigned int controlDim,
        const std::vector<double> &controlBounds, const std::vector<double> &positionBounds,
        const std::vector<double> &linvelBounds, const std::vector<double> &angvelBounds)
        : base::MorseEnvironment(rigidBodies, controlDim, controlBounds, positionBounds, linvelBounds, angvelBounds,
            30, 90)
    {
    }
    void readState(base::State *state)
    {
        // load fake state data into state
        base::MorseStateSpace::StateType *mstate = state->as<base::MorseStateSpace::StateType>();
        for (unsigned int i = 0; i < 4*rigidBodies_; i+=4)
        {
            double *pos = mstate->as<base::RealVectorStateSpace::StateType>(i)->values;
            double *lin = mstate->as<base::RealVectorStateSpace::StateType>(i+1)->values;
            double *ang = mstate->as<base::RealVectorStateSpace::StateType>(i+2)->values;
            for (unsigned int j = 0; j < 3; j++)
            {
                pos[j] = 1.0;
                lin[j] = 1.0;
                ang[j] = 1.0;
            }
            base::SO3StateSpace::StateType *quat = mstate->as<base::SO3StateSpace::StateType>(i+3);
            quat->w = 1.0;
            quat->x = 0.0;
            quat->y = 0.0;
            quat->z = 0.0;
        }
                
    }
    void writeState(const base::State *state)
    {
        // nothing to do
    }
    void applyControl(const std::vector<double> &control)
    {
        // nothing to do
    }
    void worldStep(const double dur)
    {
        // nothing to do
    }
};

class MyGoal : public base::MorseGoal
{
public:
    MyGoal(base::SpaceInformationPtr si)
        : base::MorseGoal(si), c(0)
    {
    }
    bool isSatisfied_Py(const base::State *state) const
    {
        // goal is "reached" the 10th time this is called
        distance_ = 10-(c++);
        if (distance_ == 0)
            return true;
        return false;
    }
private:
    mutable unsigned int c;
};

int main()
{
    // Control Bounds: velocity <= 10 m/s, turning angle <= ~pi/3
    std::vector<double> cbounds(4);
    cbounds[0] = -10;
    cbounds[1] = 10;
    cbounds[2] = -1;
    cbounds[3] = 1;
    // Position Bounds: stay inside 200x200x200 m cube at origin
    std::vector<double> pbounds(6);
    pbounds[0] = -100;
    pbounds[1] = 100;
    pbounds[2] = -100;
    pbounds[3] = 100;
    pbounds[4] = -100;
    pbounds[5] = 100;
    // Linear Velocity Bounds: velocity in any axis <= 10 m/s
    std::vector<double> lbounds(6);
    lbounds[0] = -10;
    lbounds[1] = 10;
    lbounds[2] = -10;
    lbounds[3] = 10;
    lbounds[4] = -10;
    lbounds[5] = 10;
    // Angular Velocity Bounds: rotation <= ~1 rps on every axis
    std::vector<double> abounds(6);
    abounds[0] = -6;
    abounds[1] = 6;
    abounds[2] = -6;
    abounds[3] = 6;
    abounds[4] = -6;
    abounds[5] = 6;
    base::MorseEnvironmentPtr env(new MyEnvironment(2, 2, cbounds, pbounds, lbounds, abounds));
    
    control::SimpleSetupPtr ss(new control::MorseSimpleSetup(env));
    
    base::GoalPtr g(new MyGoal(ss->getSpaceInformation()));
    
    ss->setGoal(g);
    
    ss->solve(1.0);

    return 0;
}

