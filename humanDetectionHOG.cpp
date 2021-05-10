#include <opencv2/opencv.hpp>

#include "threads.h"
#include <string>

void humanDetectionHOG(Mat &img)
{
    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    vector<Rect> found, found_filtered;
    hog.detectMultiScale(img, found, 0, Size(8,8), Size(), 1.05, 2);

    size_t i, j;
    for (i=0; i<found.size(); i++) {

        Rect r = found[i];
        for (j=0; j<found.size(); j++)
            if (j!=i && (r & found[j])==r)
                break;

        if (j==found.size())
            found_filtered.push_back(r);

    }

    if(found_filtered.size() > 0) {

//        Rect r = found_filtered[i];
//        r.x += cvRound(r.width*0.1);
//        r.width = cvRound(r.width*0.8);
//        r.y += cvRound(r.height*0.06);
//        r.height = cvRound(r.height*0.9);
//        rectangle(img, r.tl(), r.br(), cv::Scalar(0,255,0), 2);

//        Mat redImg(img.rows, img.cols, CV_8UC3, Scalar(0,0,255));

        addWeighted(img, 0.9, Mat(img.rows, img.cols, CV_8UC3, Scalar(255, 0, 0)), 0.3, 0, img, CV_8UC3);

    }

//    if(!found_filtered.empty()){
//        cout << "Human found!" << endl;
//    }
}
