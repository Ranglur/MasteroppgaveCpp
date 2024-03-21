#pragma once
#include <vector>
#include <functional> // For std::function
using QuantumNumberFunction = std::function<int(unsigned int, int)>;

// Generates the simple basis
std::vector<unsigned int> generateSimpleBasis(int N);
// Sorts the simple basis acording to some quantum number
std::vector<unsigned int> generateSortedSimpleBasis(int N, QuantumNumberFunction getQuantumNumber = defaultQuantumNumberFunction);

int defaultQuantumNumberFunction(unsigned int state, int N);

// Converts a bool-array to int
int boolArray2int(std::vector<bool> boolArray);
// Converts the simple basis to an int-array
std::vector<int> simpleBasis2intArray (std::vector<std::vector<bool>> spinSystem);
// Binary searches integer representation of the basis for target state in the appropriate Sz block
int binarySearchInBlock(std::vector<int> sortedBasisInt, float Sz, std::vector<bool> targetState);

// Returns an array of ints of length 2S_max + 1, which gives the index of the first element with a certain Sz (zeroth element correspnds to smallest Sz, 
// whilst last element corresponds to largest Sz)
std::vector<int> getStartingIndicesForSz(int N);

// Returns array of length 2S_max + 1 where the i-th element corresponds to the number of states in the simple basis with the i-th Sz in the in the simple basis
std::vector<int> getCoefficientArrayDimention(int N);  

// Returns an array of lenth equal to number of legal S-values. Gives the degeneracy of S,Sz
std::vector<int> getDegeneracy(int N);

// This class is a generic class used for enumerating QM states. It is assumed that the last index
// Is a numbering index (and is therefore not a quantum number of any known operator).
// The second to last quantum number can controll how many of the simple states are used for each block. 
// For this to give any speed-up the last quantum number must commute with the individual S^z_i operators


class QuantumNumber {
    private:

        QuantumNumber(std::vector<float> QuantumNumbers, int degeneracy_index)
        : degeneracy_index(degeneracy_index), QuantumNumbers(QuantumNumbers){}
    

    public:
        std::vector<float> QuantumNumbers;
        int degeneracy_index;
        QuantumNumber();
        QuantumNumber(std::vector<float> QuantumNumbers, int degeneracy_index, QuantumNumberCombinations& Q_combos )
        : degeneracy_index(degeneracy_index), QuantumNumbers(QuantumNumbers){}
    
    friend class QuantumNumberCombinations;
    friend class SpinSystem;
};

class QuantumNumberCombinations {
    private:
        int num_qnumbers;
        std::vector<std::vector<float>> Legal_values;
        

        // Define function pointer types for readability
        using QNumToIdxFunc = std::function<int(const QuantumNumber&)>;
        using IdxToQNumFunc = std::function<QuantumNumber(const int)>;
        using GetDegeneracyFunc = std::function<int(const QuantumNumber&)>;
        using GetDegeneracyIndexFunc = std::function<int(const QuantumNumber&)>;
        using GetCoefficientArrayLengthFunc = std::function<int(const float&)>; // Changed to accept const float&

        // Function pointers as member variables
        QNumToIdxFunc qnum_to_idx_func;
        IdxToQNumFunc idx_to_qnum_func;
        GetDegeneracyFunc get_degeneracy_func;
        GetCoefficientArrayLengthFunc get_coefficient_array_length_func;


    public:
        // Constructors
        QuantumNumberCombinations(); // Default constructor prototype
        QuantumNumberCombinations(int N); // N-spin constructor prototype

        // Constructor with function pointers
        QuantumNumberCombinations(int N, int num_qnumbers, std::vector<std::vector<float>> Legal_values, QNumToIdxFunc qnum_to_idx, IdxToQNumFunc idx_to_qnum, GetDegeneracyFunc getDegeneracy, GetDegeneracyIndexFunc getDegeneracyIndex, GetCoefficientArrayLengthFunc get_length)
        : num_qnumbers(num_qnumbers), Legal_values(Legal_values), qnum_to_idx_func(qnum_to_idx), idx_to_qnum_func(idx_to_qnum), get_degeneracy_func(getDegeneracy), get_coefficient_array_length_func(get_length) {}

        // Example method using the function pointers
        int qnum_2_idx(const QuantumNumber& qnums) 
        {
            return qnum_to_idx_func(qnums);
        }

        QuantumNumber idx_2_qnum(const int idx) 
        {
            return idx_to_qnum_func(idx);
        }


        int get_degeneracy(const QuantumNumber& qnums) 
        {
            return get_degeneracy_func(qnums);
        }

        int get_coefficient_array_length(const float& qnums)
        {
            get_coefficient_array_length_func(qnums);
        }
        
        int blockStart(const float& quantumNumber) 
        {
            int total = 0;
            // Ensure there's at least one set of legal values and use the last one
            if (!Legal_values.empty()) {
                const auto& lastLegalValues = Legal_values.back();
                for (const float& value : lastLegalValues) {
                    if (value < quantumNumber) {
                        total += get_coefficient_array_length(value);
                    } else {
                        break; // Since Legal_values are sorted, we can break early
                    }
                }
            }
            return total;
        }

        ~QuantumNumberCombinations(); // Destructor prototype
    friend class SpinSystem;
};




class SpinSystem {
protected:
    int N;
    QuantumNumberCombinations QnumCombos;
    std::vector<std::vector<float>> linearCombinations;

    // Protected constructor for use by derived classes
    SpinSystem(int N, QuantumNumberCombinations& QnumCobos)
        : N(N), QnumCombos(QnumCombos) {}

public:
    // Default constructor
    SpinSystem() : N(0) {}

    // Function to print the spin system (for demonstration)
    void printSpinSystem() const;

    // Accessor for linear combinations using integer index
    std::vector<float>& operator[](int i) {
        return linearCombinations[i];
    }

    // Accessor for linear combinations using QuantumNumber
    std::vector<float>& operator[](const QuantumNumber& Qns) {
        int idx = QnumCombos.qnum_2_idx(Qns);
        return linearCombinations[idx];
    }

    virtual ~SpinSystem() {}
};



class SSzSpinSystem : public SpinSystem {
public:
    // Static method to generate default QuantumNumberCombinations for the S, Sz basis
    static QuantumNumberCombinations defaultSSzQnumCombos(int N) {
        // Implementation that returns a QuantumNumberCombinations instance
        // appropriate for the S, Sz basis with N spins
        // This will likely involve setting up the legal values based on N
    }

    // Constructor for an SSzSpinSystem with a specified number of spins
    SSzSpinSystem(int N) 
        : temp(defaultSSzQnumCombos(N)), // Initialize the QnumCombos member
          SpinSystem(N, temp) { // Pass the member to the base class constructor
        // Initialize the SSzSpinSystem-specific members or states
    }

    // Adds one spin to the system, meant for internal use
    SSzSpinSystem& addSpintoSystem();

    // Postfix increment operator
    SSzSpinSystem operator++(int) {
        SSzSpinSystem temp = *this;
        this->addSpintoSystem();
        return temp;
    }

    // Prefix increment operator
    SSzSpinSystem& operator++() {
        return this->addSpintoSystem();
    }

private:
    QuantumNumberCombinations temp; // Holds the specific QnumCombos for this system
    std::vector<std::vector<bool>>Paths;
};