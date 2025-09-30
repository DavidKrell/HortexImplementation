
# Hortex Hash Function Thesis
 **Disclaimer**  
The implemented hash function is not secure and should not be used to hash confidential data. The code here is only for verifying my claims in the my bachelor thesis.

----------
The repository contains the following C++ scripts:

-   `hortex.cpp`
    
-   `ieee_754_test.cpp`
    
-   `bijectivity_test.cpp`
    
-   `search_elm_collisions.cpp`
    
-   `find_non_bijectivity_source.cpp`
    
-   `check_test_vector.cpp`
- `attack_different_interpretations.cpp`
    
----------

## Compilation

`g++ name_of_script.cpp -o name_of_output_file -std=c++23` 

Example:

`g++ bijectivity_test.cpp -o bijectivity_test -std=c++23` 

----------

## Bijectivity Test
The executable accepts up to three console arguments:

`./bijectivity_test <counting> <timing> <timing_iterations>` 

### Arguments

1.  **`counting`** (`true` / `false`)
    
    -   `false`: Stop execution once the first collision is found and print it.
        
    -   `true`: Count **all collisions** in the entire output space before finishing.
        
2.  **`timing`** (`true` / `false`)
    
    -   `false`: Run the bijectivity test once (with or without counting collisions).
        
    -   `true`: Measure execution time across multiple iterations.
        
3.  **`timing_iterations`** (integer ≥ 1, optional, only relevant if `timing=true`)
    
    -   Defines how many times the test will be repeated for timing measurements.
        
    -   The script outputs the **average runtime** over these iterations.
        

### Examples

-   Run until the **first collision** is found:
    
    `./bijectivity_test false  false` 
    
-   Count **all collisions** without timing:
    
    `./bijectivity_test true  false` 
    
-   Measure average execution time for finding the **first collision** over 10 runs:
    
    `./bijectivity_test false  true 10` 
    
-   Measure average execution time for **counting all collisions** over 5 runs:
    
    `./bijectivity_test true  true 5` 
    