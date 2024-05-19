#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

using namespace std;

typedef vector<vector<double>> Matrix;
typedef vector<double>         Vector;

// Function to compute the Euclidean distance between two points
double euclidean_distance(const Vector& a, const Vector& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sqrt(sum);
}

// Function to generate random positions
Matrix generate_random_positions(size_t nnodes, size_t dim, unsigned int seed) {
    Matrix                            pos(nnodes, Vector(dim));
    default_random_engine             generator(seed);
    uniform_real_distribution<double> distribution(0.0, 1.0);

    for (size_t i = 0; i < nnodes; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            pos[i][j] = distribution(generator);
        }
    }
    return pos;
}

Matrix fruchterman_reingold(const Matrix&       A,
                            double              k          = -1,
                            Matrix              pos        = Matrix(),
                            const vector<bool>& fixed      = vector<bool>(),
                            size_t              iterations = 50,
                            double              threshold  = 1e-4,
                            size_t              dim        = 2,
                            unsigned int        seed       = 7) {

    size_t nnodes = A.size();
    if (pos.empty()) {
        pos = generate_random_positions(nnodes, dim, seed);
    }

    if (k == -1) {
        k = sqrt(1.0 / nnodes);
    }

    double t =
        0.1 *
        max((*max_element(pos.begin(),
                          pos.end(),
                          [](const Vector& a, const Vector& b) { return a[0] < b[0]; }))[0] -
                (*min_element(pos.begin(),
                              pos.end(),
                              [](const Vector& a, const Vector& b) { return a[0] < b[0]; }))[0],
            (*max_element(pos.begin(),
                          pos.end(),
                          [](const Vector& a, const Vector& b) { return a[1] < b[1]; }))[1] -
                (*min_element(pos.begin(), pos.end(), [](const Vector& a, const Vector& b) {
                    return a[1] < b[1];
                }))[1]);

    double dt = t / (iterations + 1);
    Matrix delta(nnodes, Vector(nnodes, 0.0));
    Matrix displacement(nnodes, Vector(dim, 0.0));

    for (size_t iteration = 0; iteration < iterations; ++iteration) {
        for (size_t i = 0; i < nnodes; ++i) {
            for (size_t j = 0; j < nnodes; ++j) {
                if (i != j) {
                    delta[i][j] = euclidean_distance(pos[i], pos[j]);
                    delta[i][j] = max(delta[i][j], 0.01);
                }
            }
        }

        for (size_t i = 0; i < nnodes; ++i) {
            fill(displacement[i].begin(), displacement[i].end(), 0.0);
            for (size_t j = 0; j < nnodes; ++j) {
                if (i != j) {
                    Vector diff(dim);
                    for (size_t d = 0; d < dim; ++d) {
                        diff[d] = pos[i][d] - pos[j][d];
                    }

                    double factor =
                        (k * k / (delta[i][j] * delta[i][j]) - A[i][j] * delta[i][j] / k);
                    for (size_t d = 0; d < dim; ++d) {
                        displacement[i][d] += diff[d] * factor;
                    }
                }
            }
        }

        for (size_t i = 0; i < nnodes; ++i) {
            double length = euclidean_distance(displacement[i], Vector(dim, 0.0));
            length        = max(length, 0.01);
            for (size_t d = 0; d < dim; ++d) {
                displacement[i][d] *= (t / length);
            }
            if (fixed.empty() || !fixed[i]) {
                for (size_t d = 0; d < dim; ++d) {
                    pos[i][d] += displacement[i][d];
                }
            }
        }

        t -= dt;
        if (euclidean_distance(displacement[0], Vector(dim, 0.0)) / nnodes < threshold) {
            break;
        }
    }

    // rescale from {-1, 1} to {0, 1}
    for (size_t i = 0; i < nnodes; ++i) {
        if (fixed.empty() || !fixed[i]) {
            for (size_t d = 0; d < dim; ++d) {
                pos[i][d] += 1.0;
                pos[i][d] /= 2.0;
            }
        }
    }

    return pos;
}
