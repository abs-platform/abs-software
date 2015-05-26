#include "detection.h"

using namespace std;
using namespace cv;

// Features file
static string featuresFile = "genfiles/features.dat";
// SVM model file
static string svmModelFile = "genfiles/svmlightmodel.dat";
// Detecting descriptor vector
static string descriptorVectorFile = "genfiles/descriptorvector.dat";
// Samples images to test
static string samplesDir = "carpeta_test/";

static string toLowerCase(const string &in)
{
    string t;
    for (string::const_iterator i = in.begin(); i != in.end(); ++i) {
        t += tolower(*i);
    }
    return t;
}

static void getFilesInDirectory(const string &dirName, vector<string> &fileNames, \
                                const vector<string> &validExtensions)
{
    printf("Opening directory %s\n", dirName.c_str());
    struct dirent* ep;
    size_t extensionLocation;
    
    DIR* dp = opendir(dirName.c_str());
    if (dp != NULL) {
        while ((ep = readdir(dp))) {
            // Ignore (sub-)directories like . , .. , .svn, etc.
            if (ep->d_type & DT_DIR) {
                continue;
            }
            // Assume the last point marks beginning of extension like file.ext
            extensionLocation = string(ep->d_name).find_last_of(".");
            // Check if extension is matching the wanted ones
            string tempExt = toLowerCase(string(ep->d_name).substr(extensionLocation + 1));
            if (find(validExtensions.begin(), validExtensions.end(), tempExt) != validExtensions.end()) {
                printf("Found matching data file '%s'\n", ep->d_name);
                fileNames.push_back((string) dirName + ep->d_name);
            } else {
                printf("Found file does not match required file type, skipping: '%s'\n", ep->d_name);
            }
        }
        (void) closedir(dp);
    } else {
        printf("Error opening directory '%s'!\n", dirName.c_str());
    }
}

static void showDetections(const vector<Rect> &found, Mat &imageData)
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
        rectangle(imageData, r.tl(), r.br(), Scalar(64, 255, 64), 3);
    }
}

static void detectSat(const HOGDescriptor &hog, const double hitThreshold, Mat &imageData)
{
    vector<Rect> found;
    Size padding(Size(32, 32));
    Size winStride(Size(4, 4));
    hog.detectMultiScale(imageData, found, hitThreshold, winStride, padding);
    showDetections(found, imageData);
}

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


int main(int argc, char** argv)
{
    //HOG settings - Default images size used = 320x320 pixels
    HOGDescriptor hog;
    hog.winSize = Size(320, 320);

    vector<float> descriptorVector;
    static vector<string> validExtensions;
    validExtensions.push_back("jpg");
    validExtensions.push_back("png");
    validExtensions.push_back("ppm");
    
    SVM_MODEL::getInstance()->loadModelFromFile(const_cast<char*> (svmModelFile.c_str()));
    getDescriptorVectorFromFile(descriptorVectorFile, descriptorVector);

    // Detector detection tolerance threshold
    const double hitThreshold = SVM_MODEL::getInstance()->getThreshold();

    // Set our custom detecting vector
    hog.setSVMDetector(descriptorVector);

    printf("Testing...\n");

    static vector<string> sampleImages;
    getFilesInDirectory(samplesDir, sampleImages, validExtensions);
    unsigned long overallSamples = sampleImages.size();

    // Make sure there are actually samples to test the algorithm
    if (overallSamples == 0) {
        printf("No sample files found, nothing to do!\n");
        return EXIT_FAILURE;
    }

    for (unsigned long sample = 0; sample < sampleImages.size(); sample++) {
        cout << "Testing image:\t " << sampleImages[sample] << endl;
        
        Mat testImage;
        testImage = imread(sampleImages[sample]);
        if (!testImage.data) {
            printf("Couldnt open file\n");
        }
        
        detectSat(hog, hitThreshold, testImage);
        namedWindow("HOG Custom Detection", 80);
        imshow("HOG Custom Detection", testImage);
        
        int c = waitKey(0) & 255;
        if (c == 'q' || c == 'Q') {
            break;
        }
    }

    return EXIT_SUCCESS;
}
