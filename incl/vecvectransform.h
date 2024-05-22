#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

// Template function to transform a vector of Something to a vector of OtherThing
template <typename InputType, typename OutputType, typename UnaryOperation>
std::vector<OutputType> transformVector(std::vector<InputType> const& inputVec, UnaryOperation op) {
    std::vector<OutputType> outputVec(inputVec.size());
    std::transform(inputVec.begin(), inputVec.end(), outputVec.begin(), op);
    return outputVec;
}
