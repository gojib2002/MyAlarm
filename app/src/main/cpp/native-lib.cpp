#include <jni.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace cv;
using namespace std;

int drawing = -1;
vector<int> pattern;
vector<int> vecPt;
vector<Point> drawLine;

Point getHandCenter(const Mat& mask, double& radius){

    Mat dst;
    distanceTransform(mask, dst, DIST_L2, 5);

    int maxIdx[2];
    minMaxIdx(dst, NULL, &radius, NULL, maxIdx, mask);

    return Point(maxIdx[1], maxIdx[0]);
}

int distanceSquare(Point a, Point b){
    return ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y));
}

bool notInList(vector<int> vec, int num){
    for(int i : vec){
        if(i == num){
            return false;
        }
    }
    return true;
}

// 패턴 번호에 해당하는 num을 vec에 넣음
// num은 중복되어 삽입되지 않음
// 0에서 2로 바로 이동할 때, 2를 삽입하기 앞서 1을 삽입함
void patternPush(vector<int> *_vec, int num){
    vector<int> vec;
    vec = *_vec;
    if(num < 0 || num > 8){
        return;
    }
    if(notInList(vec, num)){
        if(vec.size() != 0){
            if(num == 0){
                if(vec[vec.size() - 1] == 2){if(notInList(vec, 1)){vec.push_back(1);}}
                else if(vecPt[vec.size() - 1] == 6){if(notInList(vec, 3)){vec.push_back(3);}}
                else if(vec[vec.size() - 1] == 8){if(notInList(vec, 4)){vec.push_back(4);}}}
            else if(num == 1){
                if(vec[vec.size() - 1] == 7){if(notInList(vec, 4)){vec.push_back(4);}}}
            else if(num == 2){
                if(vec[vec.size() - 1] == 0){if(notInList(vec, 1)){vec.push_back(1);}}
                else if(vecPt[vec.size() - 1] == 6){if(notInList(vec, 4)){vec.push_back(4);}}
                else if(vec[vec.size() - 1] == 8){if(notInList(vec, 5)){vec.push_back(5);}}}
            else if(num == 3){
                if(vec[vec.size() - 1] == 5){if(notInList(vec, 4)){vec.push_back(4);}}}
            else if(num == 5){
                if(vec[vec.size() - 1] == 3){if(notInList(vec, 4)){vec.push_back(4);}}}
            else if(num == 6){
                if(vec[vec.size() - 1] == 0){if(notInList(vec, 3)){vec.push_back(3);}}
                else if(vecPt[vec.size() - 1] == 2){if(notInList(vec, 4)){vec.push_back(4);}}
                else if(vec[vec.size() - 1] == 8){if(notInList(vec, 7)){vec.push_back(7);}}}
            else if(num == 7){
                if(vec[vec.size() - 1] == 1){if(notInList(vec, 4)){vec.push_back(4);}}}
            else if(num == 8){
                if(vec[vec.size() - 1] == 0){if(notInList(vec, 4)){vec.push_back(4);}}
                else if(vecPt[vec.size() - 1] == 2){if(notInList(vec, 5)){vec.push_back(5);}}
                else if(vec[vec.size() - 1] == 6){if(notInList(vec, 7)){vec.push_back(7);}}}
        }
        vec.push_back(num);
    }
    *_vec = vec;
    return;
}

extern "C"
JNIEXPORT int JNICALL
Java_com_example_myalarm_CameraActivity_UnlockPattern(JNIEnv *env, jobject instance, jlong matAddrInput, jlong matAddrResult, jint _pattern) {

    // jint형 parameter를 string을 거쳐 vector<int>로 변환
    if(pattern.size() == 0){
        int tempInt = ((int) _pattern);
        string tempStr = to_string(tempInt);
        for(int i = 0; i < tempStr.size(); i++){
            pattern.push_back(tempStr[i]-'1');
        }
    }

    Mat &matInput = *(Mat *)matAddrInput;
    Mat &matResult = *(Mat *)matAddrResult;
    Mat matTemp = matInput.clone();
    Mat matTemp2 = matInput.clone();

    vector<vector<Point>> contours;
    vector<Point> approx;

    vector<Point> vecPtCir = {Point(600,180),Point(960,180),Point(1320,180),Point(600,540),Point(960,540),Point(1320,540),Point(600,900),Point(960,900),Point(1320,900)};

    matResult = matInput;
    putText(matResult, to_string(drawing), Point(80,160), 1, 10.0, Scalar(0,0,0));

    // 손 영역 검출
    cvtColor(matInput, matTemp, COLOR_RGB2YCrCb);
    inRange(matTemp, Scalar(0,133,77), Scalar(255,173,127), matTemp);

    // 잡음 제거 / 모폴로지
    erode(matTemp, matTemp, Mat(10,10,CV_8U,Scalar(1)),Point(-1,-1),2);
    dilate(matTemp, matTemp, Mat(10,10,CV_8U,Scalar(1)),Point(-1,-1),2);

    // canny edge / Edge 검출
    Canny(matTemp,matTemp,100,200);

    findContours(matTemp,contours,RETR_LIST,CHAIN_APPROX_SIMPLE);

    // 기본적인 contour를 그림
//    for(size_t i = 0; i < contours.size(); i++){
//        Scalar color = Scalar(0,125,0);
//        drawContours(matResult, contours, (int)i, color, 10);
//    }

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
//            Scalar color = Scalar(0,0,150);
//            drawContours(matResult, contours, maxIndex, color, 10);

            // 손목 밑 부분 제거 / 높이 1080 기준
//            for(int j = 0; j < contours[maxIndex].size(); j++){
//                if(contours[maxIndex][j].y > 1000){
//                    contours[maxIndex].erase(contours[maxIndex].begin() + j);
//                    j--;
//                }
//            }

            // 너무 가까운 점들 제거
            for(int j = 0; j < contours[maxIndex].size(); j++){
                for(int k = j + 1; k < contours[maxIndex].size(); k++){
                    if(distanceSquare(contours[maxIndex][j], contours[maxIndex][k]) < 4000){
                        contours[maxIndex].erase(contours[maxIndex].begin() + k);
                        k--;
                    }
                }
            }

            // 손가락 끝 점
//            for(int j = 0; j < contours[maxIndex].size(); j++){
//                circle(matResult,contours[maxIndex][j], 30, Scalar(0,0,255),5);
//            }
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

//             1. 원 안에 손이 있는지 확인
//             2. 있다면 vector에 추가
//             3. vector에 추가된 정보를 바탕으로 draw

            int pointCir = -1;
            for(int j = 0; j < vecPtCir.size(); j++){
                if(distanceSquare(contours[maxIndex][first], vecPtCir[j]) < 8000){
                    pointCir = j;
                }
            }
            patternPush(&vecPt, pointCir);
        }
    }


    // 빨간 점 전체
//    for(int j = 0; j < drawLine.size(); j++){
//        circle(matResult,drawLine[j], 5, Scalar(255,0,0), -1);
//    }

    // 초록 원
    for(int j = 0; j < vecPtCir.size(); j++){
        circle(matResult, vecPtCir[j], 100, Scalar(0,255,0), 10);
    }
    // 초록 원 안 색칠
    for(int j = 0; j < vecPt.size(); j++){
        int pre;
        circle(matResult, vecPtCir[vecPt[j]], 100, Scalar(0,150,0), -1);
        if(j != 0){
            line(matResult, vecPtCir[vecPt[pre]], vecPtCir[vecPt[j]], Scalar(0,255,0), 7);
        }
        pre = j;
    }

    // 빨간 점
    if(!drawLine.empty()){
        circle(matResult, drawLine[drawLine.size()-1], 10, Scalar(255,0,0), -1);
    }

    if(drawing > 0){
        drawing--;
    }
    if(drawing <= 0){
        drawLine.resize(0);
        vecPt.resize(0);
    }

//    vector<int> inputCode = {7,4,1,2,3};
//    bool check = false;
//    if(vecPt.size() == inputCode.size()){
//        check = true;
//        for(int i = 0; i < vecPt.size(); i++){
//
//            if(vecPt[i] != inputCode[i] - 1){
//                check = false;
//            }
//        }
//    }

    bool end = false;
    if(vecPt.size() == pattern.size()){
        end = true;
        for(int i = 0; i < vecPt.size() && i < pattern.size(); i++){
            if(vecPt[i] != pattern[i]){
                end = false;
            }
        }
    }
    if(end){
        return 1;
    }

    return 0;
}