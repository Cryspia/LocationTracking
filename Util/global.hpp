#pragma once

// Standard C libraries
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <ctime>

// C++11 STL containers
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <map>
#include <unordered_map>
#include <string>

// C++11 STL utility libraries
#include <algorithm>
#include <utility>
#include <memory>
#include <exception>

// C++11 in/out streams libraries
#include <iostream>
#include <sstream>

// Opencv 2.4.x libraries
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/legacy/legacy.hpp>

// g++ --std=c++11 `pkg-config --libs --cflags opencv`

namespace TractUtil
{
    using namespace std;
    using namespace cv;


    struct Entry            // The entries in the dictionary
    {
        Mat descriptor;     // The 128d vector as a descriptor.
        size_t belong;      // The article that the entry belongs to.
    };

    #define FLOAT_CV CV_64F // The opencv float format used for programs.
    #define CV_TYPE double
    #define EPS 0.0000001
}
