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


vector<Mat> map1;
vector<Mat> map2;
vector<Mat> topViewH;
vector<Mat> mergeH;


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



    Mat map1Cam1, map1Cam2, map1Cam3, map1Cam4;
    Mat map2Cam1, map2Cam2, map2Cam3, map2Cam4;
    Mat topViewHCam1, topViewHCam2, topViewHCam3, topViewHCam4;

    FileStorage paramsCam1("/opt/warpParams/warpParamsCam1.xml", FileStorage::READ);
    FileStorage paramsCam2("/opt/warpParams/warpParamsCam2.xml", FileStorage::READ);
    FileStorage paramsCam3("/opt/warpParams/warpParamsCam3.xml", FileStorage::READ);
    FileStorage paramsCam4("/opt/warpParams/warpParamsCam4.xml", FileStorage::READ);

    paramsCam1["map1"] >> map1Cam1;
    paramsCam2["map1"] >> map1Cam2;
    paramsCam3["map1"] >> map1Cam3;
    paramsCam4["map1"] >> map1Cam4;

    map1.push_back(map1Cam1);
    map1.push_back(map1Cam2);
    map1.push_back(map1Cam3);
    map1.push_back(map1Cam4);

    paramsCam1["map2"] >> map2Cam1;
    paramsCam2["map2"] >> map2Cam2;
    paramsCam3["map2"] >> map2Cam3;
    paramsCam4["map2"] >> map2Cam4;

    map2.push_back(map2Cam1);
    map2.push_back(map2Cam2);
    map2.push_back(map2Cam3);
    map2.push_back(map2Cam4);

    paramsCam1["topViewH"] >> topViewHCam1;
    paramsCam2["topViewH"] >> topViewHCam2;
    paramsCam3["topViewH"] >> topViewHCam3;
    paramsCam4["topViewH"] >> topViewHCam4;

    topViewH.push_back(topViewHCam1);
    topViewH.push_back(topViewHCam2);
    topViewH.push_back(topViewHCam3);
    topViewH.push_back(topViewHCam4);



    // ---------------------- LOOP CAMERAS ----------------------
    vector<VideoCapture> captures;

    for(int i = 0; i <= 3; i++){
        // Create pipeline for each cam
        int port = 62464 + i;
        string pipeline = "udpsrc port=" + to_string(port) + " ! application/x-rtp,payload=26,encoding-name=JPEG,media=video ! rtpjpegdepay ! jpegparse ! jpegdec ! videoconvert ! appsink";
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
        cout << "Port:" << port << endl;
        cout << "Pipeine:" << pipeline << endl;

    }
    // ----------------------------------------------------------


//    QtConcurrent::run(mainThread, captures);



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

            future = QtConcurrent::run(cameraThread, i, frame);
            futures.push_back(future);
        }
        // ------------------------------------------------------


        // ----- RECEIVE TOPVIEW IMAGES FROM CAMERA THREADS -----
        for(int i = 0; i <= 3; i++){
            futures[i].waitForFinished();
            topViewImgs.push_back(futures[i].result());
        }
        // ------------------------------------------------------



        // ----------------- CREATE BIRDS EYE VIEW ------------------
        Mat birdsEyeView;
        QFuture<void> futureMerge = QtConcurrent::run(mergeThread, topViewImgs);
        futureMerge.waitForFinished();
//        birdsEyeView = futureMerge.result();
        // ----------------------------------------------------------
//    }
    // ----------------------------------------------------------




    // This is needed to be able to use ImagePainter in qml
    // You will also need to add:
    // import CrossControl 1.0
    // at the top of the qml-file


//    QTimer::singleShot(1000, [&finalImg]() {

//        ImagePainter * painter = ImagePainter::getSingleton();

////        QImage image(":/landscape.png");
//        if(painter == nullptr)
//        {
//            qDebug() << "No painter";
//            exit(EXIT_FAILURE);
//        }

////        painter->setImage(image);
//        painter->setImage(finalImg);

//    } );


    return app.exec();
}
