#include <opencv2/opencv.hpp>

#include "threads.h"

void humanDetectionHOG(Mat &img)
{
    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    vector<Rect> found, found_filtered;
    hog.detectMultiScale(img, found, 0, Size(8,8), Size(32,32), 1.05, 2);

//    if(found.size() >= 1) {

//        Mat redImg;
//        redImg = img;
//        redImg.setTo(cv::Scalar(255,0,0));

//        addWeighted(img, 0.8, redImg, 0.2, 0.0, img);

////        // convert to HSV
////        cvtColor(img,img,COLOR_RGB2HSV);

////        // increase s of red


////        // convert to RGB
////        cvtColor(img, img, COLOR_HSV2RGB);

//    }

    size_t i, j;
    for (i=0; i<found.size(); i++) {

        Rect r = found[i];
        for (j=0; j<found.size(); j++)
            if (j!=i && (r & found[j])==r)
                break;

        if (j==found.size())
            found_filtered.push_back(r);

    }

    for (i=0; i<found_filtered.size(); i++) {

        Rect r = found_filtered[i];
        r.x += cvRound(r.width*0.1);
        r.width = cvRound(r.width*0.8);
        r.y += cvRound(r.height*0.06);
        r.height = cvRound(r.height*0.9);
        rectangle(img, r.tl(), r.br(), cv::Scalar(0,255,0), 2);
    }

}
