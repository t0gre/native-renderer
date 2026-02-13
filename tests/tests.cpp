#include <cstdio>
#include <string>

#include "mystl.hpp"

#include "test_helpers.h" 
    
int main(int argc, char** argv) {
    DArray<TestResult> results;

    // triangle tests
    for (const auto &result : runTriangleTests()) {
        results.push_back(result);
    }
    
    // mesh tests
     for (const auto &result : runVerticesTests()) {
        results.push_back(result);
    }

    // scene tests
    for (const auto &result : runSceneTests()) {
        results.push_back(result);
    }

    int total = 0;
    int passed = 0;
    int failed = 0;

    // Print results
    for (size_t i = 0; i < results.size(); ++i) {
        total++;
        std::string result_message;
        const char * test_message = results[i].message;
        bool test_result = results[i].pass;

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