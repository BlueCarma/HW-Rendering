#ifndef MERGEPARAMS_H
#define MERGEPARAMS_H

#include <QTimer>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

QT_BEGIN_NAMESPACE
using namespace cv;
using namespace std;
QT_END_NAMESPACE


extern vector<Mat> mask;
extern vector<Mat> stitchH;

#endif // MERGEPARAMS_H
