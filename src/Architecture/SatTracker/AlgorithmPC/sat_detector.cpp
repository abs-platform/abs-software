#include "sat_detector.h"

static void getDescriptorVectorFromFile(string& filename, vector<float>& descriptorVector)
{
    cout << "Opening descriptor file " << filename.c_str() << endl;
    string separator = " ";
    fstream File;
    File.open(filename.c_str(), ios::in);
    if (File.good() && File.is_open()) {
        string line;
        while (getline(File, line)) {
            size_t pos = 0;
            string token;
            while ((pos = line.find(separator)) != std::string::npos) {
                token = line.substr(0, pos);
                descriptorVector.push_back(atof(token.c_str()));
                line.erase(0, pos + separator.length());
            }
        }
        File.close();
    }
}

static void rectangleToCircle(Rect& r, CircleData &circle)
{
    int a, b;
    a = round(r.width/2);
    b = round(r.height/2);
    circle.center = Point(r.x + a, r.y + b);
    circle.radius = hypotf(a,b);
}

static void showDetections(const vector<Rect>& found, Mat& imageData, CircleData& cercle)
{
    vector<Rect> found_filtered;
    size_t i, j;
    
    for (i = 0; i < found.size(); ++i) {
        Rect r = found[i];
        for (j = 0; j < found.size(); ++j) {
            if (j != i && (r & found[j]) == r) break;
        }
        if (j == found.size()) {
            found_filtered.push_back(r);
        }
    }
    
    for (i = 0; i < found_filtered.size(); i++) {
        Rect r = found_filtered[i];
        rectangleToCircle(r, cercle);
        circle(imageData, cercle.center, cercle.radius, Scalar(64, 255, 64), 3, 8, 0);
    }
}

void configureHOGDetector(HOGDescriptor& hog, string& descriptorVectorFile)
{
    vector<float> descriptorVector;
    getDescriptorVectorFromFile(descriptorVectorFile, descriptorVector);

    hog.winSize = Size(80, 80); // Default images size used = 80x80 pixels
    hog.setSVMDetector(descriptorVector);
}

void detectSat(HOGDescriptor& hog, const double hitThreshold, Mat& imageData, CircleData& cercle)
{
    vector<Rect> found;
    Size padding(Size(4, 4));
    Size winStride(Size(2, 2));
    hog.detectMultiScale(imageData, found, hitThreshold, winStride, padding);
    showDetections(found, imageData, cercle);
}
