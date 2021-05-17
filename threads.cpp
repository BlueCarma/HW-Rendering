#include <QCoreApplication>
#include <QFuture>
#include <QtCore>
#include <QtConcurrent/QtConcurrent>
#include <QPainter>
#include <qpainter.h>

#include "threads.h"
#include "imagepainter.h"
#include "calibParams.h"
#include "mergeParams.h"




void mergeThread(vector<Mat> &imgs)
{
//    TickMeter tmMerge;
//    tmMerge.start();

    Mat img1 = imgs[0], img2 = imgs[1], img3 = imgs[2], img4 = imgs[3];
    Mat birdsEyeView;

    // Stitch images(x4)
    add(img1, img2, birdsEyeView);
    add(birdsEyeView, img3, birdsEyeView);
    add(birdsEyeView, img4, birdsEyeView);

    // Crop image to the size of the screen
    Rect crop_region(320-240, 490-400, 480, 800);
    birdsEyeView = birdsEyeView(crop_region);

    // Rotate image to fit the screen
    rotate(birdsEyeView, birdsEyeView, cv::ROTATE_90_CLOCKWISE);

    // Convert Mat to QImage
    QImage finalImg(birdsEyeView.data, birdsEyeView.cols, birdsEyeView.rows, birdsEyeView.step, QImage::Format_RGB888);

    // Display stitched image
    ImagePainter *painter = ImagePainter::getSingleton();
    if(painter != nullptr){
        painter->setImage(finalImg);
    }

//    tmMerge.stop();
//    cout << "" << tmMerge.getTimeSec() << endl;

//    cvtColor(birdsEyeView, birdsEyeView, COLOR_BGR2RGB);
//    imwrite("/opt/images/birdsEyeViewHumanDetected2Cams.png", birdsEyeView);
}




Mat cameraThread(int &camNo, VideoCapture &capture)
{
//    TickMeter tmCam;
//    tmCam.start();

    Mat img, maskedImg;

    capture >> img;
    cvtColor(img, img, COLOR_BGR2RGB);

    // Human detection using HOG
    if(camNo == 0)
        humanDetectionHOG(img);

//    qDebug() << img.size().height << img.size().width;
    // Dewarp(anti-fish) images
    remap(img, img, map1[camNo], map2[camNo], INTER_LINEAR, BORDER_CONSTANT);

//    qDebug() << "After remap";

    // Change perspective to top view
    warpPerspective(img, img, topViewH[camNo], img.size(), WARP_INVERSE_MAP);

    copyMakeBorder(img, img, 0, 500, 0, 0, BORDER_CONSTANT);

    warpPerspective(img, img, stitchH[camNo], img.size());

    // Add mask
    bitwise_and(img, img, maskedImg, mask[camNo]);

//    tmCam.stop();
//    cout << "" << tmCam.getTimeSec() << endl;

    return maskedImg;
}




void mainThread(vector<VideoCapture> &captures)
{
    vector<QFuture<Mat>> futures(4);
    vector<Mat> topViewImgs(4);

    int time = 0;

    // Loop forever
    while(time < 10){

//        TickMeter tm;
//        tm.start();

        // ------------------ RUN CAMERA THREADS ------------------
        for(int i = 0; i <= 3; i++){
            futures[i] = QtConcurrent::run(cameraThread, i, captures[i]);
        }
        // --------------------------------------------------------


        // ------ RECEIVE TOPVIEW IMAGES FROM CAMERA THREADS ------
        for(int i = 0; i <= 3; i++){
            futures[i].waitForFinished();
            topViewImgs[i] = futures[i].result();
        }
        // --------------------------------------------------------


        // ------------------ CREATE BIRDS EYE VIEW ---------------
        QFuture<void> futureMerge = QtConcurrent::run(mergeThread, topViewImgs);
        futureMerge.waitForFinished();
        // --------------------------------------------------------

//        tm.stop();
//        cout << "" << tm.getTimeSec() << endl;
        time ++;
    }
}
