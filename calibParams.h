#ifndef CALIBPARAMS_H
#define CALIBPARAMS_H

#include <QTimer>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

QT_BEGIN_NAMESPACE
using namespace cv;
using namespace std;
QT_END_NAMESPACE


extern vector<Mat> map1;
extern vector<Mat> map2;
extern vector<Mat> topViewH;
extern vector<Mat> mergeH;


#endif // CALIBPARAMS_H
