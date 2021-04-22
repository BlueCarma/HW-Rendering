#include <QCoreApplication>
#include <QFuture>
#include <QtCore>
#include <QtConcurrent/QtConcurrent>

#include "threads.h"
#include "imagepainter.h"
#include "calibParams.h"
#include "mergeParams.h"




void mergeThread(vector<Mat> &imgs)
{

    Mat img1 = imgs[0], img2 = imgs[1], img3 = imgs[2], img4 = imgs[3];
//    Mat img3 = imgs[2];
    Mat birdsEyeView;

    rotate(img3, img3, ROTATE_180);

    copyMakeBorder(img1, img1, 0, 1000, 500, 500, BORDER_CONSTANT);
    copyMakeBorder(img2, img2, 0, 1000, 0, 1000, BORDER_CONSTANT);
    copyMakeBorder(img3, img3, 0, 1000, 0, 1000, BORDER_CONSTANT);
    copyMakeBorder(img4, img4, 0, 1000, 0, 1000, BORDER_CONSTANT);

    // warp img2, img3, and img4 with respect to img1
    warpPerspective(img2, img2, stitchH[1], img2.size());
    warpPerspective(img3, img3, stitchH[2], img3.size());
    warpPerspective(img4, img4, stitchH[3], img4.size());

    // apply masks
    Mat maskedImg1, maskedImg2, maskedImg3, maskedImg4;
//    maskedImg1 = img1;
//    maskedImg2 = img2;
//    maskedImg3 = img3;
//    maskedImg4 = img4;

    bitwise_and(img1, img1, maskedImg1, mask[0]);
    bitwise_and(img2, img2, maskedImg2, mask[1]);
    bitwise_and(img3, img3, maskedImg3, mask[2]);
    bitwise_and(img4, img4, maskedImg4, mask[3]);

    // stitch topView images(x4)
    add(maskedImg1, maskedImg2, birdsEyeView);
    add(birdsEyeView, maskedImg3, birdsEyeView);
    add(birdsEyeView, maskedImg4, birdsEyeView);

//    birdsEyeView = img4;

    Rect crop_region(810-240, 520-400, 480, 800);
    birdsEyeView = birdsEyeView(crop_region);

    rotate(birdsEyeView, birdsEyeView, cv::ROTATE_90_CLOCKWISE);




    // Convert Mat to QImage
    QImage finalImg(birdsEyeView.data, birdsEyeView.cols, birdsEyeView.rows, birdsEyeView.step, QImage::Format_RGB888);

    // Display stitched image
    ImagePainter *painter = ImagePainter::getSingleton();
    if(painter != nullptr){
        painter->setImage(finalImg);
    }

}


Mat cameraThread(int &camNo, Mat &img)
{

//    humanDetectionHOG(img);

    // antiFish
    remap(img, img, map1[camNo], map2[camNo], INTER_LINEAR, BORDER_CONSTANT);

    // topView
    warpPerspective(img, img, topViewH[camNo], img.size(), WARP_INVERSE_MAP);

    // return topView image
    return img;
}


void mainThread(vector<VideoCapture> &captures)
{

    // ------------------ LOOP AT TIME INTERVAL -----------------
    vector<Mat> frame(4);
//    QFuture<Mat> future;
    vector<QFuture<Mat>> futures(4);
    Mat topViewImg;
    vector<Mat> topViewImgs(4);

    while(1){

        // ----------------- RUN CAMERA THREADS -----------------
        for(int i = 0; i <= 3; i++){
            captures[i] >> frame[i];
            cvtColor(frame[i], frame[i], COLOR_BGR2RGB);

            futures[i] = QtConcurrent::run(cameraThread, i, frame[i]);
//            futures[i] = future;
        }
//        captures[0] >> frame;
//        cvtColor(frame, frame, COLOR_BGR2RGB);

//        future = QtConcurrent::run(cameraThread, 0, frame);
        // ------------------------------------------------------

        // ----- RECEIVE TOPVIEW IMAGES FROM CAMERA THREADS -----
        for(int i = 0; i <= 3; i++){
            futures[i].waitForFinished();
            topViewImgs[i] = futures[i].result();
        }
//            future.waitForFinished();
//            topViewImg = future.result();
        // ------------------------------------------------------
//while(1){
        // ----------------- CREATE BIRDS EYE VIEW --------------
//        QFuture<void> futureMerge = QtConcurrent::run(mergeThread, topViewImgs);
//        Mat topViewImg = topViewImgs[0];
//        topViewImg = topViewImgs[1];

//        for(int i = 0; i <= 3; i++){
//            string filename = "/opt/images/topViewImg" + to_string(i) + ".png";
//            imwrite(filename,frame);
//        }
        QFuture<void> futureMerge = QtConcurrent::run(mergeThread, topViewImgs);
        futureMerge.waitForFinished();
        // ------------------------------------------------------
//        QThread::msleep(10);
    }
    // ----------------------------------------------------------

}
