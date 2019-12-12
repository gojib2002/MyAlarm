#include <jni.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <android/log.h>

using namespace cv;
using namespace std;

int drawing = -1;
int timer = -1;
int index = 0;
int selection = 0;

int selectionHat = 0;
int selectionGlasses = 0;
int selectionThird = 0;

bool onCreate = true;

string condition = "Fine";

vector<int> pattern;
vector<int> vecPt;
vector<Point> drawLine;
vector<Rect> vecEye;
vector<Rect> vecFace;

Point redDot = Point(960, 540);

const Scalar black = Scalar(0,0,0);
const Scalar brightGray = Scalar(225,225,225);
const Scalar darkGray = Scalar(150,150,150);


const int setTimer = 30;

const int px0 = 1110;
const int px1 = 1380;
const int px2 = 1650;
const int px3 = 1920;
const int py0 = 0;
const int py1 = 270;
const int py2 = 540;
const int py3 = 810;
const int py4 = 1080;

const Point point00 = Point(px0, py0);
const Point point01 = Point(px1, py0);
const Point point02 = Point(px2, py0);
const Point point03 = Point(px3, py0);
const Point point10 = Point(px0, py1);
const Point point11 = Point(px1, py1);
const Point point12 = Point(px2, py1);
const Point point13 = Point(px3, py1);
const Point point20 = Point(px0, py2);
const Point point21 = Point(px1, py2);
const Point point22 = Point(px2, py2);
const Point point23 = Point(px3, py2);
const Point point30 = Point(px0, py3);
const Point point31 = Point(px1, py3);
const Point point32 = Point(px2, py3);
const Point point33 = Point(px3, py3);
const Point point40 = Point(px0, py4);
const Point point41 = Point(px1, py4);
const Point point42 = Point(px2, py4);
const Point point43 = Point(px3, py4);

vector<vector<int>> imageFactor(6, {0});

Mat hatA = imread("/storage/emulated/0/hatA.png", IMREAD_UNCHANGED);
Mat hatB = imread("/storage/emulated/0/hatB.png", IMREAD_UNCHANGED);
Mat hatX = imread("/storage/emulated/0/hatX.png", IMREAD_UNCHANGED);

Mat glassesA = imread("/storage/emulated/0/glassesA.png", IMREAD_UNCHANGED);
Mat glassesB = imread("/storage/emulated/0/glassesB.png", IMREAD_UNCHANGED);
Mat glassesX = imread("/storage/emulated/0/glassesX.png", IMREAD_UNCHANGED);

Mat thirdA = imread("/storage/emulated/0/thirdA.png", IMREAD_UNCHANGED);
Mat thirdB = imread("/storage/emulated/0/thirdB.png", IMREAD_UNCHANGED);
Mat thirdX = imread("/storage/emulated/0/thirdX.png", IMREAD_UNCHANGED);

Mat cameraX = imread("/storage/emulated/0/camera.png", IMREAD_UNCHANGED);

Mat hat1 = hatA.clone();
Mat hat2 = hatB.clone();
Mat glasses1 = glassesA.clone();
Mat glasses2 = glassesB.clone();
Mat third1 = thirdA.clone();
Mat third2 = thirdB.clone();

Mat hat01 = hatA.clone();
Mat hat02 = hatB.clone();
Mat glasses01 = glassesA.clone();
Mat glasses02 = glassesB.clone();
Mat third01 = thirdA.clone();
Mat third02 = thirdB.clone();

int distanceSquare(Point a, Point b){
    return ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y));
}

void overlayImage(const Mat &background, const Mat &foreground,
                  Mat &output, Point2i location)
{
    background.copyTo(output);


    // start at the row indicated by location, or at row 0 if location.y is negative.
    for (int y = std::max(location.y, 0); y < background.rows; ++y)
    {
        int fY = y - location.y; // because of the translation

        // we are done of we have processed all rows of the foreground image.
        if (fY >= foreground.rows)
            break;

        // start at the column indicated by location,

        // or at column 0 if location.x is negative.
        for (int x = std::max(location.x, 0); x < background.cols; ++x)
        {
            int fX = x - location.x; // because of the translation.

            // we are done with this row if the column is outside of the foreground image.
            if (fX >= foreground.cols)
                break;

            // determine the opacity of the foregrond pixel, using its fourth (alpha) channel.
            double opacity =
                    ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3])

                    / 255.;


            // and now combine the background and foreground pixel, using the opacity,

            // but only if opacity > 0.
            for (int c = 0; opacity > 0 && c < output.channels(); ++c)
            {
                unsigned char foregroundPx =
                        foreground.data[fY * foreground.step + fX * foreground.channels() + c];
                unsigned char backgroundPx =
                        background.data[y * background.step + x * background.channels() + c];
                output.data[y*output.step + output.channels()*x + c] =
                        backgroundPx * (1. - opacity) + foregroundPx * opacity;
            }
        }
    }
}

// 버튼을 누르는 것을 인식, 결과 : index, timer
void buttonSelect(){
    if(redDot.x >= px2 && redDot.x < px3 && redDot.y >= py0 && redDot.y < py1){ if(index != 10){ timer = setTimer; } index = 10; timer--;}
    else if(redDot.x >= px2 && redDot.x < px3 && redDot.y >= py1 && redDot.y < py2){ if(index != 20){ timer = setTimer; } index = 20;  timer--; }
    else if(redDot.x >= px2 && redDot.x < px3 && redDot.y >= py2 && redDot.y < py3){ if(index != 30){ timer = setTimer; } index = 30;  timer--; }
    else if(redDot.x >= px2 && redDot.x < px3 && redDot.y >= py3 && redDot.y < py4){ index = 40;  timer--; }

    else if(index / 10 == 1 && redDot.x >= px0 && redDot.x < px1 && redDot.y >= py0 && redDot.y < py1){ if(index != 11){ timer = setTimer; } index = 11; timer--; }
    else if(index / 10 == 1 && redDot.x >= px1 && redDot.x < px2 && redDot.y >= py0 && redDot.y < py1){ if(index != 12){ timer = setTimer; } index = 12; timer--; }
    else if(index / 10 == 2 && redDot.x >= px0 && redDot.x < px1 && redDot.y >= py1 && redDot.y < py2){ if(index != 21){ timer = setTimer; } index = 21; timer--; }
    else if(index / 10 == 2 && redDot.x >= px1 && redDot.x < px2 && redDot.y >= py1 && redDot.y < py2){ if(index != 22){ timer = setTimer; } index = 22; timer--; }
    else if(index / 10 == 3 &&  redDot.x >= px0 && redDot.x < px1 && redDot.y >= py2 && redDot.y < py3){ if(index != 31){ timer = setTimer; } index = 31; timer--; }
    else if(index / 10 == 3 &&  redDot.x >= px1 && redDot.x < px2 && redDot.y >= py2 && redDot.y < py3){ if(index != 32){ timer = setTimer; } index = 32; timer--; }

    else { index = 0; }

    if(timer <= 0){
        if(index == 10){selectionHat = 0; } else if(index == 11){ selectionHat = 1; } else if(index == 12){ selectionHat = 2; }
        else if(index == 20){ selectionGlasses = 0; } else if(index == 21){ selectionGlasses = 1; } else if(index == 22){ selectionGlasses = 2; }
        else if(index == 30){ selectionThird = 0; } else if(index == 31){ selectionThird = 1; } else if(index == 32){ selectionThird = 2; }
    }
    else {
        timer--;
    }
}

void showHat(Mat &matInput, Mat &matResult, int i){
    int num = i + 1;

    int dist_head = vecFace[0].br().x - vecFace[0].tl().x;
    double sizeRatio_hat = (double) dist_head / imageFactor[num][0];

    int overlayX = vecFace[0].tl().x - (sizeRatio_hat * imageFactor[num][1]);
    int overlayY = vecFace[0].tl().y - (sizeRatio_hat * imageFactor[num][2]);

    if(i == 1){
        resize(hatA, hat1, Size(), sizeRatio_hat, sizeRatio_hat);
        overlayImage(matInput, hat1, matResult, Point(overlayX, overlayY));
    }
    else if(i == 2){
        resize(hatB, hat2, Size(), sizeRatio_hat, sizeRatio_hat);
        overlayImage(matInput, hat2, matResult, Point(overlayX, overlayY));
    }
}

void showGlasses(Mat &matInput, Mat &matResult, int i){
    int num = i - 1;

    Point leftEye;
    Point rightEye;

    if(vecEye[0].tl().x < vecEye[1].tl().x){
        leftEye = (vecEye[0].tl() + vecEye[0].br()) / 2;
        rightEye = (vecEye[1].tl() + vecEye[1].br()) / 2;
    } else {
        leftEye = (vecEye[1].tl() + vecEye[1].br()) / 2;
        rightEye = (vecEye[0].tl() + vecEye[0].br()) / 2;
    }

    int dist_eyes = rightEye.x - leftEye.x;
    if(dist_eyes < 0){
        dist_eyes = - dist_eyes;
    }
    // 원래 이미지 대비 확대 비율
    double sizeRatio_glasses = (double) dist_eyes / imageFactor[num][0];

    int overlayX = leftEye.x - (sizeRatio_glasses * imageFactor[num][1]);
    int overlayY = leftEye.y - (sizeRatio_glasses * imageFactor[num][2]);

    if(i == 1){
        resize(glassesA, glasses1, Size(), sizeRatio_glasses, sizeRatio_glasses);
        overlayImage(matInput, glasses1, matResult, Point(overlayX, overlayY));
    }
    else if(i == 2) {
        resize(glassesB, glasses2, Size(), sizeRatio_glasses, sizeRatio_glasses);
        overlayImage(matInput, glasses2, matResult, Point(overlayX, overlayY));
    }
}

void showThird(Mat &matInput, Mat &matResult, int i){
    int num = i + 3;

    int dist_head = vecFace[0].br().x - vecFace[0].tl().x;
    double sizeRatio_third = (double) dist_head / imageFactor[num][0];

    if(i == 1){
        int overlayX = (vecFace[0].tl().x * 0.75 + vecFace[0].br().x * 0.25) - (sizeRatio_third * imageFactor[num][1]);
        int overlayY = (vecFace[0].tl().y * 0.4 + vecFace[0].br().y * 0.6) - (sizeRatio_third * imageFactor[num][2]);

        resize(thirdA, third1, Size(), sizeRatio_third, sizeRatio_third);
        overlayImage(matInput, third1, matResult, Point(overlayX, overlayY));
    }
    else if(i == 2){
        int overlayX = vecFace[0].br().x - (sizeRatio_third * imageFactor[num][1]);
        int overlayY = vecFace[0].tl().y - (sizeRatio_third * imageFactor[num][2]);

        resize(thirdB, third2, Size(), sizeRatio_third, sizeRatio_third);
        overlayImage(matInput, third2, matResult, Point(overlayX, overlayY));
    }
}

extern "C"
JNIEXPORT int JNICALL
Java_com_example_myalarm_CameraActivity_Accessory(JNIEnv *env, jobject instance, jlong cascadeClassifierEye, jlong cascadeClassifierFace, jlong matAddrInput, jlong matAddrResult) {

    // 이미지 가로길이, 눈 사이 거리, 눈 옆 가로 길이, 눈 옆 세로 길이
    if(onCreate){
        onCreate = false;

        if(glassesX.data == NULL || glassesA.data == NULL || glassesB.data == NULL
            || hatX.data == NULL || hatA.data == NULL || hatB.data == NULL){
            return -1;
        }

        // 안경
        imageFactor[0] = {144, 74, 125};
        imageFactor[1] = {100, 58, 110};
        // 모자
        imageFactor[2] = {105,60,155};
        imageFactor[3] = {110,71,100};
        // 치장
        imageFactor[4] = {1200,100,100};
        imageFactor[5] = {100,0,100};

        resize(hatX, hatX, Size(px1 - px0, py1 - py0));
        resize(hat01, hat01, Size(px1 - px0, py1 - py0));
        resize(hat02, hat02, Size(px1 - px0, py1 - py0));
        resize(glassesX, glassesX, Size(px1 - px0, py1 - py0));
        resize(glasses01, glasses01, Size(px1 - px0, py1 - py0));
        resize(glasses02, glasses02, Size(px1 - px0, py1 - py0));
        resize(thirdX, thirdX, Size(px1 - px0, py1 - py0));
        resize(third01, third01, Size(px1 - px0, py1 - py0));
        resize(third02, third02, Size(px1 - px0, py1 - py0));
        resize(cameraX, cameraX, Size(px1 - px0, py1 - py0));
    }

    Mat &matInput = *(Mat *)matAddrInput;
    Mat &matResult = *(Mat *)matAddrResult;
    Mat matTemp = matInput.clone();
    Mat matGray = matInput.clone();

    vector<vector<Point>> contours;
    vector<Point> approx;

    matResult = matInput;

    ((CascadeClassifier *) cascadeClassifierEye)->detectMultiScale(matGray, vecEye, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(100,100));
    ((CascadeClassifier *) cascadeClassifierFace)->detectMultiScale(matGray, vecFace, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(300,300));

    if(selectionGlasses == 1 && vecEye.size() == 2){
        showGlasses(matInput, matResult, 1);
    }
    else if(selectionGlasses == 2 && vecEye.size() == 2){
        showGlasses(matInput, matResult, 2);
    }

    if(selectionHat == 1 && vecFace.size() == 1){
        showHat(matInput, matResult, 1);
    }
    else if(selectionHat == 2 && vecFace.size() == 1){
        showHat(matInput, matResult, 2);
    }

    if(selectionThird == 1 && vecFace.size() == 1){
        showThird(matInput, matResult, 1);
    }
    else if(selectionThird == 2 && vecFace.size() == 1){
        showThird(matInput, matResult, 2);
    }

//[손인식 주석 시작]

    for(int i = 0; i < vecFace.size(); i++){
        rectangle(matTemp,Point(0,0),Point(1080,1080),black,-1);
    }

//    // 얼굴 영역 제거?
//    for(int i = 0; i < vecFace.size(); i++){
//        rectangle(matTemp,Point(vecFace[i].tl().x,0),Point(vecFace[i].br().x,1080),black,-1);
//    }

//    // 얼굴 및 눈 영역 표시
//    for(int i = 0; i < vecFace.size(); i++){
//        rectangle(matResult,vecFace[i].tl(),vecFace[i].br(),Scalar(255,255,0),3);
//    }
//    for(int i = 0; i < vecEye.size(); i++){
//        rectangle(matResult,vecEye[i].tl(),vecEye[i].br(),Scalar(0,255,255),3);
//    }

    // 손 영역 검출
    cvtColor(matTemp, matTemp, COLOR_RGB2YCrCb);
    inRange(matTemp, Scalar(0,133,77), Scalar(255,173,127), matTemp);

    // 잡음 제거 / 모폴로지
    erode(matTemp, matTemp, Mat(10,10,CV_8U,Scalar(1)),Point(-1,-1),2);
    dilate(matTemp, matTemp, Mat(10,10,CV_8U,Scalar(1)),Point(-1,-1),2);

    // canny edge / Edge 검출
    Canny(matTemp,matTemp,100,200);

    findContours(matTemp,contours,RETR_LIST,CHAIN_APPROX_SIMPLE);

    // 선 간소화?
    for(int i = 0; i < contours.size(); i++){
        approxPolyDP(Mat(contours[i]), approx, 0.02 * arcLength(contours[i], true), true);
        convexHull(Mat(approx), contours[i]);
    }

    if(contours.size() != 0){
        int maxIndex = 0;
        int maxArea = contourArea(contours[0]);
        for(int i = 1; i < contours.size(); i++){
            if(contourArea(contours[i]) > maxArea){
                maxIndex = i;
                maxArea = contourArea(contours[i]);
            }
        }

        // 최대 크기가 너무 작으면 손으로 인식 안 함
        if(maxArea > 5000){
            // 너무 가까운 점들 제거
            for(int j = 0; j < contours[maxIndex].size(); j++){
                for(int k = j + 1; k < contours[maxIndex].size(); k++){
                    if(distanceSquare(contours[maxIndex][j], contours[maxIndex][k]) < 4000){
                        contours[maxIndex].erase(contours[maxIndex].begin() + k);
                        k--;
                    }
                }
            }
        }

        if(contours[maxIndex].size() > 1){

            // 다른 점보다 몇 px 위에 있는 점을 포인팅한다고 간주
            // 그 점을 drawLine 벡터에 넣음
            int first = 0;
            int second = 0;
            for(int j = 0; j < contours[maxIndex].size(); j++) {
                if(contours[maxIndex][j].y < contours[maxIndex][first].y){
                    second = first;
                    first = j;
                }
            }
            if(contours[maxIndex][second].y - contours[maxIndex][first].y > 300 || contours[maxIndex][first].y > 800){
                drawing = 30;
                drawLine.push_back(contours[maxIndex][first]);
            }
        }
    }

    // 버튼 출력부

    if(index == 0){
        rectangle(matResult, point02, point13, brightGray, -1);
        rectangle(matResult, point12, point23, brightGray, -1);
        rectangle(matResult, point22, point33, brightGray, -1);
        rectangle(matResult, point32, point43, brightGray, -1);
    }
    else if(index == 10){
        rectangle(matResult, point00, point11, brightGray, -1);
        rectangle(matResult, point01, point12, brightGray, -1);
    }
    else if(index == 20){
        rectangle(matResult, point10, point21, brightGray, -1);
        rectangle(matResult, point11, point22, brightGray, -1);
    }
    else if(index == 30){
        rectangle(matResult, point20, point31, brightGray, -1);
        rectangle(matResult, point21, point32, brightGray, -1);
    }
    else if(index == 40){
        rectangle(matResult, point02, point13, brightGray, -1);
        rectangle(matResult, point12, point23, brightGray, -1);
        rectangle(matResult, point22, point33, brightGray, -1);
        rectangle(matResult, point32, point43, darkGray, -1);
    }
    else if(index == 11){
        rectangle(matResult, point00, point11, darkGray, -1);
        rectangle(matResult, point01, point12, brightGray, -1);
    }
    else if(index == 12){
        rectangle(matResult, point00, point11, brightGray, -1);
        rectangle(matResult, point01, point12, darkGray, -1);
    }
    else if(index == 21){
        rectangle(matResult, point10, point21, darkGray, -1);
        rectangle(matResult, point11, point22, brightGray, -1);
    }
    else if(index == 22){
        rectangle(matResult, point10, point21, brightGray, -1);
        rectangle(matResult, point11, point22, darkGray, -1);
    }
    else if(index == 31){
        rectangle(matResult, point20, point31, darkGray, -1);
        rectangle(matResult, point21, point32, brightGray, -1);
    }
    else if(index == 32){
        rectangle(matResult, point20, point31, brightGray, -1);
        rectangle(matResult, point21, point32, darkGray, -1);
    }

    if(index / 10 == 1){
        rectangle(matResult, point02, point13, darkGray, -1);
        rectangle(matResult, point12, point23, brightGray, -1);
        rectangle(matResult, point22, point33, brightGray, -1);
        rectangle(matResult, point32, point43, brightGray, -1);

        rectangle(matResult, point00, point11, black, 3);
        rectangle(matResult, point01, point12 , black, 3);

        overlayImage(matInput, hat01, matResult, point00);
        overlayImage(matInput, hat02, matResult, point01);
    }
    else if(index / 10 == 2){
        rectangle(matResult, point02, point13, brightGray, -1);
        rectangle(matResult, point12, point23, darkGray, -1);
        rectangle(matResult, point22, point33, brightGray, -1);
        rectangle(matResult, point32, point43, brightGray, -1);

        rectangle(matResult, point10, point21, black, 3);
        rectangle(matResult, point11, point22 , black, 3);

        overlayImage(matInput, glasses01, matResult, point10);
        overlayImage(matInput, glasses02, matResult, point11);
    }
    else if(index / 10 == 3){
        rectangle(matResult, point02, point13, brightGray, -1);
        rectangle(matResult, point12, point23, brightGray, -1);
        rectangle(matResult, point22, point33, darkGray, -1);
        rectangle(matResult, point32, point43, brightGray, -1);

        rectangle(matResult, point20, point31, black, 3);
        rectangle(matResult, point21, point32 , black, 3);

        overlayImage(matInput, third01, matResult, point20);
        overlayImage(matInput, third02, matResult, point21);
    }

    rectangle(matResult, point02, point13, black, 3);
    rectangle(matResult, point12, point23, black, 3);
    rectangle(matResult, point22, point33, black, 3);
    rectangle(matResult, point32, point43, black, 3);

    overlayImage(matInput,hatX, matResult, point02);
    overlayImage(matInput,glassesX, matResult, point12);
    overlayImage(matInput, thirdX, matResult, point22);
    overlayImage(matInput, cameraX, matResult, point32);

    // 빨간 점
    if(!drawLine.empty()){
        redDot = drawLine[drawLine.size()-1];
        circle(matResult, redDot, 10, Scalar(255,0,0), -1);
    }

    buttonSelect();

    if(selectionHat == 1){
        rectangle(matResult, Point(1470,900), Point(1570,1000), Scalar(255,0,0), 4);
    } else if(selectionHat == 2){
        rectangle(matResult, Point(1470,900), Point(1570,1000), Scalar(0,0,255), 4);
    } else {
        rectangle(matResult, Point(1470,900), Point(1570,1000), Scalar(0,255,0), 4);
    }
    if(selectionGlasses == 1){
        rectangle(matResult, Point(1290,900), Point(1390,1000), Scalar(255,0,0), 4);
    } else if(selectionGlasses == 2){
        rectangle(matResult, Point(1290,900), Point(1390,1000), Scalar(0,0,255), 4);
    } else {
        rectangle(matResult, Point(1290,900), Point(1390,1000), Scalar(0,255,0), 4);
    }
    if(selectionThird == 1){
        rectangle(matResult, Point(1110,900), Point(1210,1000), Scalar(255,0,0), 4);
    } else if(selectionThird == 2){
        rectangle(matResult, Point(1110,900), Point(1210,1000), Scalar(0,0,255), 4);
    } else {
        rectangle(matResult, Point(1110,900), Point(1210,1000), Scalar(0,255,0), 4);
    }
//[손인식 주석 끝]



//[디버깅 중엔 사용 안함]
//    if(index == 4){
//        redDot = Point(0,0);
//        return 1;
//    }

    return 0;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_myalarm_CameraActivity_loadCascade(JNIEnv *env, jobject instance, jstring cascadeFileName_) {

    const char *nativeFileNameString = env->GetStringUTFChars(cascadeFileName_, 0);


    string baseDir("/storage/emulated/0/");

    baseDir.append(nativeFileNameString);

    const char *pathDir = baseDir.c_str();


    jlong ret = 0;

    ret = (jlong) new CascadeClassifier(pathDir);

    if (((CascadeClassifier *) ret)->empty()) {
        condition = "Bad";
        __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ",

                            "CascadeClassifier로 로딩 실패  %s", nativeFileNameString);

    }

    else
        condition = "Good";
        __android_log_print(ANDROID_LOG_DEBUG, "native-lib :: ",

                            "CascadeClassifier로 로딩 성공 %s", nativeFileNameString);

    env->ReleaseStringUTFChars(cascadeFileName_, nativeFileNameString);

    return ret;

    //env->ReleaseStringUTFChars(cascadeFileName_, cascadeFileName);
}