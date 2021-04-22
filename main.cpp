#include <QGuiApplication>
#include <QtCore>
#include <QCoreApplication>
#include <QQmlApplicationEngine>
#include <QTimer>
#include <QDebug>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
//#include <QString>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "imagepainter.h"
#include "threads.h"
#include "calibParams.h"
#include "mergeParams.h"


vector<Mat> map1;
vector<Mat> map2;
vector<Mat> topViewH;
vector<Mat> mergeH;
vector<Mat> stitchH;
vector<Mat> mask;


int main(int argc, char *argv[])
{

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);




    qmlRegisterType<ImagePainter>("CrossControl", 1, 0, "ImagePainter");
    engine.load(url);



    // -------------------- READ FROM FILE --------------------

    Mat map1Cam1, map1Cam2, map1Cam3, map1Cam4;
    Mat map2Cam1, map2Cam2, map2Cam3, map2Cam4;
    Mat topViewHCam1, topViewHCam2, topViewHCam3, topViewHCam4;
    Mat mask1, mask2, mask3, mask4;
    Mat stitchH1, stitchH2, stitchH3, stitchH4;

    FileStorage paramsCam1("/opt/warpParams/warpParamsCam1.xml", FileStorage::READ);
    paramsCam1["map1"] >> map1Cam1;
    paramsCam1["map2"] >> map2Cam1;
    paramsCam1["topViewH"] >> topViewHCam1;
    map1.push_back(map1Cam1);
    map2.push_back(map2Cam1);
    topViewH.push_back(topViewHCam1);

    FileStorage paramsCam2("/opt/warpParams/warpParamsCam2.xml", FileStorage::READ);
    paramsCam2["map1"] >> map1Cam2;
    paramsCam2["map2"] >> map2Cam2;
    paramsCam2["topViewH"] >> topViewHCam2;
    map1.push_back(map1Cam2);
    map2.push_back(map2Cam2);
    topViewH.push_back(topViewHCam2);

    FileStorage paramsCam3("/opt/warpParams/warpParamsCam3.xml", FileStorage::READ);
    paramsCam3["map1"] >> map1Cam3;
    paramsCam3["map2"] >> map2Cam3;
    paramsCam3["topViewH"] >> topViewHCam3;
    map1.push_back(map1Cam3);
    map2.push_back(map2Cam3);
    topViewH.push_back(topViewHCam3);

    FileStorage paramsCam4("/opt/warpParams/warpParamsCam4.xml", FileStorage::READ);
    paramsCam4["map1"] >> map1Cam4;
    paramsCam4["map2"] >> map2Cam4;
    paramsCam4["topViewH"] >> topViewHCam4;
    map1.push_back(map1Cam4);
    map2.push_back(map2Cam4);
    topViewH.push_back(topViewHCam4);

    FileStorage stitchParams("/opt/stitchParams/stitchParams.xml", FileStorage::READ);
    stitchParams["stitchH1"] >> stitchH1;
    stitchParams["stitchH2"] >> stitchH2;
    stitchParams["stitchH3"] >> stitchH3;
    stitchParams["stitchH4"] >> stitchH4;
    stitchH.push_back(stitchH1);
    stitchH.push_back(stitchH2);
    stitchH.push_back(stitchH3);
    stitchH.push_back(stitchH4);
    stitchParams["mask1"] >> mask1;
    stitchParams["mask2"] >> mask2;
    stitchParams["mask3"] >> mask3;
    stitchParams["mask4"] >> mask4;
    mask.push_back(mask1);
    mask.push_back(mask2);
    mask.push_back(mask3);
    mask.push_back(mask4);

    // --------------------------------------------------------



    // ---------------------- LOOP CAMERAS ----------------------
    vector<VideoCapture> captures;

    for(int i = 0; i <= 3; i++){
        // Create pipeline for each cam
        int port = 62464 + i;
        string pipeline = "udpsrc port=" + to_string(port) + " ! application/x-rtp,payload=26,encoding-name=JPEG,media=video ! rtpjpegdepay ! jpegparse ! jpegdec ! videoconvert ! appsink";
//        string pipeline = "udpsrc port=" + to_string(port) + " ! application/x-rtp,payload=26,encoding-name=JPEG,media=video ! rtpjpegdepay ! jpegparse ! v4l2jpegdec ! imxvideoconvert_g2d ! appsink";
        VideoCapture cap;
        cap.open(pipeline, CAP_ANY);

        // Check if stream from cam(i) is OK
        if(!cap.isOpened()) {
            string camStatus = "Cam " + to_string(i) + " NOT OK!";
            cout << "Status " << camStatus << endl;
        }
        else {
            string camStatus = "Cam " + to_string(i) + " OK!";
            cout << "Status " << camStatus << endl;
            captures.push_back(cap);
        }
//        cout << "Port:" << port << endl;
//        cout << "Pipeine:" << pipeline << endl;

    }
    // ----------------------------------------------------------


    QtConcurrent::run(mainThread, captures);



//    // ------------------ LOOP AT TIME INTERVAL -----------------
//    Mat frame;
//    QFuture<Mat> future;
//    vector<QFuture<Mat>> futures;
//    vector<Mat> topViewImgs;

////    timeLoop{
//        // ----------------- RUN CAMERA THREADS -----------------
//        for(int i = 0; i <= 3; i++){
//            captures[i] >> frame;
//            cvtColor(frame, frame, COLOR_BGR2RGB);

//            future = QtConcurrent::run(cameraThread, i, frame);
//            futures.push_back(future);
//        }
//        // ------------------------------------------------------


//        // ----- RECEIVE TOPVIEW IMAGES FROM CAMERA THREADS -----
//        for(int i = 0; i <= 3; i++){
//            futures[i].waitForFinished();
//            topViewImgs.push_back(futures[i].result());
//        }
//        // ------------------------------------------------------



//        // ----------------- CREATE BIRDS EYE VIEW ------------------
//        Mat birdsEyeView;
//        QFuture<void> futureMerge = QtConcurrent::run(mergeThread, topViewImgs);
//        futureMerge.waitForFinished();
////        birdsEyeView = futureMerge.result();
//        // ----------------------------------------------------------
////    }
//    // ----------------------------------------------------------

    return app.exec();
}
