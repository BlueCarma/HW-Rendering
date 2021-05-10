#ifndef CALIBPARAMS_H
#define CALIBPARAMS_H

#include <QTimer>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

QT_BEGIN_NAMESPACE
using namespace cv;
using namespace std;
QT_END_NAMESPACE


extern vector<UMat> map1;
extern vector<UMat> map2;
extern vector<UMat> topViewH;
extern vector<UMat> mergeH;


#endif // CALIBPARAMS_H
