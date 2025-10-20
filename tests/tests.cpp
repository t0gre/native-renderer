#include "stdbool.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>

#include "test_helpers.cpp" // include before tests
#include "raycast_triangle_tests.cpp"
#include "raycast_vertices_tests.cpp"
#include "raycast_scene_tests.cpp"

    
int main(int argc, char** argv) {
    std::vector<TestResult> results;

    // triangle tests
    results.push_back(intersect_triangle());
    results.push_back(dont_intersect_because_origin());
    results.push_back(dont_intersect_because_direction());
    
    // mesh tests
    results.push_back(intersect_vertices_first());
    results.push_back(intersect_vertices_last());

    // scene tests
    results.push_back(intersect_node_with_position_transform());
    results.push_back(intersect_node_with_multiple_position_transform());
    results.push_back(intersect_node_with_roation_transform());

    int total = 0;
    int passed = 0;
    int failed = 0;

    // Print results
    for (size_t i = 0; i < results.size(); ++i) {
        total++;
        std::string result_message;
        const char * test_message = results.at(i).message;
        bool test_result = results.at(i).pass;

        auto stringified_total = std::to_string(total);
        
        result_message = "test result " + 
                            stringified_total + 
                            ": " +
                            test_message + 
                            " -- ";
        
        if (test_result) {
            passed++;
            result_message += "PASS";
            printf("\033[0;32m"); // set the output color to green
        }else {
            failed++;
            result_message += "FAIL";
            printf("\033[0;31m"); // set the output color to red

        }
        printf("%s\n", result_message.c_str());
    }

    // set the output color to green
    printf("\033[0;32m");

    if (failed) {

        printf("Total Tests Passed: %d\n", passed);
        printf("\033[0;31m"); // set the output color to red
        printf("Total Tests Failed: %d\n", failed);
        printf("Test Suite Result -- FAIL\n");
        return 1;
    } else {
       
        printf("Total Tests Passed: %d\n", passed);
        printf("Total Tests Failed: %d\n", failed);
        printf("Test Suite Result -- PASS\n");

        return 0;
    }



    
   
}