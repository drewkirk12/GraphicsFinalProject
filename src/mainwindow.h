#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "realtime.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectParam1();
    void connectParam2();
    void connectNear();
    void connectFar();
    void connectUploadFile();
    void connectFog();
    void connectFogType();

    Realtime *realtime;
    //QCheckBox *filter1;
    QPushButton *uploadFile;
    QSlider *p1Slider;
    QSlider *p2Slider;
    QSpinBox *p1Box;
    QSpinBox *p2Box;
    QSlider *nearSlider;
    QSlider *farSlider;
    QDoubleSpinBox *nearBox;
    QDoubleSpinBox *farBox;

    QSlider *fogSlider;
    QDoubleSpinBox *fogBox;
    QSlider *fogTypeSlider;
    QSpinBox *fogTypeBox;

    // Extra Credit:
    //QCheckBox *ec1;

private slots:
    //void onPerPixelFilter();
    //void onKernelBasedFilter();
    void onUploadFile();
    void onValChangeP1(int newValue);
    void onValChangeP2(int newValue);
    void onValChangeNearSlider(int newValue);
    void onValChangeFarSlider(int newValue);
    void onValChangeNearBox(double newValue);
    void onValChangeFarBox(double newValue);
    void onValChangeFogSlider(int newValue);
    void onValChangeFogBox(double newValue);
    void onValChangeFogType(int newValue);



    // Extra Credit:
//    void onExtraCredit1();
};
