#ifndef THREADS_H
#define THREADS_H


#include <QTimer>
#include <QString>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <QQuickItem>
#include <QImage>
#include <QDebug>

#include <iostream>

#include "calibParams.h"

QT_BEGIN_NAMESPACE
using namespace cv;
using namespace std;
QT_END_NAMESPACE




//class threads //: public QQuickItem
//{
////    Q_OBJECT

//public:
//    threads();

    void mergeThread(vector<Mat> &imgs);
    Mat cameraThread(int &camNo, Mat &img);
    void mainThread(vector<VideoCapture> &captures);

//    void cameraThread(int &camNo, Mat &img);
//    void mergeThread(int &merge, Mat &img);

//private:

//    QImage qt_image;
//};

#endif // THREADS_H
