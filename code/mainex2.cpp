#include <iostream>
#include <vector>
#include "convexHull.h"


int main(int argv, char* argc[]) {
    
    convexHull hullObj; // Create an object of the convexHull class
    std::vector<Point> data = hullObj.createConvexHull(argc[1]); 
    std::vector<Point> data2 = hullObj.createConvexHull(argc[2]);

}
