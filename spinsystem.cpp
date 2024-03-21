#pragma once
#include "spinSystem.h"
#include <vector>
#include <cmath>
#include <bitset>
#include <algorithm> // std::sort
#include <numeric> // std::iota
using QuantumNumberFunction = std::function<int(unsigned int, int)>;

// General functions

std::vector<unsigned int> generateSimpleBasis(int N) {
    unsigned int totalStates = 1U << N; // 2^N states
    std::vector<unsigned int> basis(totalStates);
    for (unsigned int i = 0; i < totalStates; ++i) {
        basis[i] = i;
    }
    return basis;
}

int defaultQuantumNumberFunction(unsigned int state, int N) {
    int magnetization = 0;
    for (int i = 0; i < N; ++i) {
        magnetization += (state & (1U << i)) ? 1 : -1;
    }
    return magnetization;
}

std::vector<unsigned int> generateSortedSimpleBasis(int N, QuantumNumberFunction getQuantumNumber = defaultQuantumNumberFunction) {
    // Generate the simple basis
    std::vector<unsigned int> simpleBasis(1U << N);
    std::iota(simpleBasis.begin(), simpleBasis.end(), 0); // Fill with 0, 1, ..., 2^N - 1

    // Sort simpleBasis based on quantum numbers
    std::sort(simpleBasis.begin(), simpleBasis.end(),
              [N, &getQuantumNumber](unsigned int a, unsigned int b) {
                  return getQuantumNumber(a, N) < getQuantumNumber(b, N);
              });

    return simpleBasis;
}


// Binary search for simple states, sorted by some sorting function



// QuantumNumbers


// QuantumNumberCombinations


// Protected constructor
SpinSystem::SpinSystem(int N, QuantumNumberCombinations& QnumCombos)
    : N(N), QnumCombos(QnumCombos) {
    // Calculate the Hilbert space dimension for a spin-1/2 system
    int hilbertSpaceDim = std::pow(2, N);

    // Resize the outer vector to match the Hilbert space dimension
    linearCombinations.resize(hilbertSpaceDim);

    // Iterate through the sets of legal quantum numbers in QnumCombos
    for (size_t i = 0; i < QnumCombos.Legal_values.size(); ++i) {
        for (size_t j = 0; j < QnumCombos.Legal_values[i].size(); ++j) {
            // Construct a QuantumNumber instance for the current combination
            // Assuming each quantum number dimension is independent and fills one slot in the vector
            std::vector<float> quantumNums(QnumCombos.Legal_values.size(), 0);
            for (size_t k = 0; k < QnumCombos.Legal_values.size(); ++k) {
                // Populate with the current legal value for dimension i, others remain 0
                quantumNums[k] = (k == i) ? QnumCombos.Legal_values[i][j] : 0;
            }

            // Quickly determine degeneracy; skip if it's 0 to enforce selection rules
            QuantumNumber qNum(quantumNums, 0, QnumCombos); // Dummy degeneracy index
            int degeneracy = QnumCombos.get_degeneracy(qNum);
            if (degeneracy == 0) {
                continue; // Skip illegal states
            }

            // Calculate the coefficient array length for this quantum number
            int coeffArrayLength = QnumCombos.get_coefficient_array_length(qNum.QuantumNumbers.back());

            // Determine the index in the linearCombinations vector for this quantum number
            int index = QnumCombos.qnum_2_idx(qNum);
            linearCombinations[index].resize(coeffArrayLength);

            // Apply the same coefficient array length to all degenerate states
            for (int d = 1; d < degeneracy; ++d) {
                linearCombinations[index + d].resize(coeffArrayLength);
            }
            // Adjust the inner loop counter to account for degeneracy
            j += degeneracy - 1;
        }
    }
}


// SSzSpinsystem