#include <QCoreApplication>
#include <QFuture>
#include <QtCore>
#include <QtConcurrent/QtConcurrent>

#include "threads.h"
#include "imagepainter.h"
//#include "calibParams.h"




void mergeThread(vector<Mat> &imgs)
{

    qDebug("Hello from mergeThread!");

    Mat birdsEyeView;

    birdsEyeView = imgs[0];
    // stitch topView images(x4)

//    imwrite("/opt/images/birdsEyeView.png", birdsEyeView);

    // Convert Mat to QImage
    QImage finalImg(birdsEyeView.data, birdsEyeView.cols, birdsEyeView.rows, QImage::Format_RGB888);

    // Display stitched image
    ImagePainter *painter = ImagePainter::getSingleton();
    if(painter != nullptr){
        painter->setImage(finalImg);
    }

    qDebug("Goodbye from mergeThread!");

}


Mat cameraThread(int &camNo, Mat &img)
{

    qDebug() << "Hello from cameraThread " << camNo;

    Mat topView;
    topView = img;
    imwrite("/opt/images/topView.png", topView);

    // read frame
    // antiFish
    // topView

    // return topView image

//    unsigned long sleep = 1000 * camNo;
//    QThread::msleep(sleep);

    Mat testH = warpParamsCam1[2];

    cout << "topViewH cam 1:" << testH << endl;


    qDebug() << "Goodbye from cameraThread " << camNo;

    return topView;

}


void mainThread(vector<VideoCapture> &captures){

    qDebug("Hello from mainThread!");

    // ------------------ LOOP AT TIME INTERVAL -----------------
    Mat frame;
    QFuture<Mat> future;
    vector<QFuture<Mat>> futures;
    vector<Mat> topViewImgs;

//    timeLoop{
        // ----------------- RUN CAMERA THREADS -----------------
        for(int i = 0; i <= 3; i++){
            captures[i] >> frame;
            cvtColor(frame, frame, COLOR_BGR2RGB);

            imwrite("/opt/images/frame.png", frame);

            future = QtConcurrent::run(cameraThread, i, frame);
            futures.push_back(future);
        }
        // ------------------------------------------------------l


        // ----- RECEIVE TOPVIEW IMAGES FROM CAMERA THREADS -----
        for(int i = 0; i <= 3; i++){
            futures[i].waitForFinished();
            topViewImgs.push_back(futures[i].result());
        }
        // ------------------------------------------------------



        // ----------------- CREATE BIRDS EYE VIEW ------------------
//        Mat birdsEyeView;
        QFuture<void> futureMerge = QtConcurrent::run(mergeThread, topViewImgs);
        futureMerge.waitForFinished();
//        birdsEyeView = futureMerge.result();
        // ----------------------------------------------------------
//    }
    // ----------------------------------------------------------



        qDebug("Goodbye from mainThread!");

}
