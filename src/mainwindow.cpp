#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <iostream>

void MainWindow::initialize() {
    realtime = new Realtime;

    QHBoxLayout *hLayout = new QHBoxLayout; // horizontal alignment
    QVBoxLayout *vLayout = new QVBoxLayout(); // vertical alignment
    vLayout->setAlignment(Qt::AlignTop);
    hLayout->addLayout(vLayout);
    hLayout->addWidget(realtime, 1);
    this->setLayout(hLayout);

    // Create labels in sidebox
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    QLabel *tesselation_label = new QLabel(); // Parameters label
    tesselation_label->setText("Tesselation");
    tesselation_label->setFont(font);
    QLabel *camera_label = new QLabel(); // Camera label
    camera_label->setText("Camera");
    camera_label->setFont(font);

    QLabel *fog_label = new QLabel(); //  fog label
    fog_label->setText("Fog Intensity");

    QLabel *fog_type_label = new QLabel(); //  fog label
    fog_type_label->setText("Fog Type");

    QLabel *skybox_label = new QLabel(); //  fog label
    skybox_label->setText("Skybox Type");

    QLabel *param1_label = new QLabel(); // Parameter 1 label
    param1_label->setText("Parameter 1:");
    QLabel *param2_label = new QLabel(); // Parameter 2 label
    param2_label->setText("Parameter 2:");
    QLabel *near_label = new QLabel(); // Near plane label
    near_label->setText("Near Plane:");
    QLabel *far_label = new QLabel(); // Far plane label
    far_label->setText("Far Plane:");



    // Create checkbox for per-pixel filter
    clouds_checkbox = new QCheckBox();
    clouds_checkbox->setText(QStringLiteral("Clouds Toggle"));
    clouds_checkbox->setChecked(false);

    // Create file uploader for scene file
    uploadFile = new QPushButton();
    uploadFile->setText(QStringLiteral("Upload Scene File"));

    // Creates the boxes containing the parameter sliders and number boxes
    QGroupBox *p1Layout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *l1 = new QHBoxLayout();
    QGroupBox *p2Layout = new QGroupBox(); // horizonal slider 2 alignment
    QHBoxLayout *l2 = new QHBoxLayout();

    // Create slider controls to control parameters
    p1Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    p1Slider->setTickInterval(1);
    p1Slider->setMinimum(1);
    p1Slider->setMaximum(25);
    p1Slider->setValue(1);

    p1Box = new QSpinBox();
    p1Box->setMinimum(1);
    p1Box->setMaximum(25);
    p1Box->setSingleStep(1);
    p1Box->setValue(1);

    p2Slider = new QSlider(Qt::Orientation::Horizontal); // Parameter 2 slider
    p2Slider->setTickInterval(1);
    p2Slider->setMinimum(1);
    p2Slider->setMaximum(25);
    p2Slider->setValue(1);

    p2Box = new QSpinBox();
    p2Box->setMinimum(1);
    p2Box->setMaximum(25);
    p2Box->setSingleStep(1);
    p2Box->setValue(1);

    // Adds the slider and number box to the parameter layouts
    l1->addWidget(p1Slider);
    l1->addWidget(p1Box);
    p1Layout->setLayout(l1);

    l2->addWidget(p2Slider);
    l2->addWidget(p2Box);
    p2Layout->setLayout(l2);

    // Creates the boxes containing the camera sliders and number boxes
    QGroupBox *nearLayout = new QGroupBox(); // horizonal near slider alignment
    QHBoxLayout *lnear = new QHBoxLayout();
    QGroupBox *farLayout = new QGroupBox(); // horizonal far slider alignment
    QHBoxLayout *lfar = new QHBoxLayout();

    // Create slider controls to control near/far planes
    nearSlider = new QSlider(Qt::Orientation::Horizontal); // Near plane slider
    nearSlider->setTickInterval(1);
    nearSlider->setMinimum(1);
    nearSlider->setMaximum(1000);
    nearSlider->setValue(10);

    nearBox = new QDoubleSpinBox();
    nearBox->setMinimum(0.01f);
    nearBox->setMaximum(10.f);
    nearBox->setSingleStep(0.1f);
    nearBox->setValue(0.1f);

    farSlider = new QSlider(Qt::Orientation::Horizontal); // Far plane slider
    farSlider->setTickInterval(1);
    farSlider->setMinimum(1000);
    farSlider->setMaximum(10000);
    farSlider->setValue(10000);

    farBox = new QDoubleSpinBox();
    farBox->setMinimum(10.f);
    farBox->setMaximum(100.f);
    farBox->setSingleStep(0.1f);
    farBox->setValue(100.f);

    // Adds the slider and number box to the parameter layouts
    lnear->addWidget(nearSlider);
    lnear->addWidget(nearBox);
    nearLayout->setLayout(lnear);

    lfar->addWidget(farSlider);
    lfar->addWidget(farBox);
    farLayout->setLayout(lfar);

    // Creates the boxes containing the sliders for fog
    QGroupBox *fogLayout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *f1 = new QHBoxLayout();

    // Create slider controls to control fog
    fogSlider = new QSlider(Qt::Orientation::Horizontal); // fog slider
    fogSlider->setTickInterval(1);
    fogSlider->setMinimum(0);
    fogSlider->setMaximum(1000);
    fogSlider->setValue(1000);

    fogBox = new QDoubleSpinBox();
    fogBox->setMinimum(0.f);
    fogBox->setMaximum(10.f);
    fogBox->setSingleStep(0.001f);
    fogBox->setValue(0.0f);

    // Adds the slider and number box to the fog layouts
    f1->addWidget(fogSlider);
    f1->addWidget(fogBox);
    fogLayout->setLayout(f1);

    QGroupBox *fogTypeLayout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *ft1 = new QHBoxLayout();

    // Create slider controls to control parameters
    fogTypeSlider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    fogTypeSlider->setTickInterval(1);
    fogTypeSlider->setMinimum(0);
    fogTypeSlider->setMaximum(2);
    fogTypeSlider->setValue(1);

    fogTypeBox = new QSpinBox();
    fogTypeBox->setMinimum(0);
    fogTypeBox->setMaximum(2);
    fogTypeBox->setSingleStep(1);
    fogTypeBox->setValue(1);

    // Adds the slider and number box to the parameter layouts
    ft1->addWidget(fogTypeSlider);
    ft1->addWidget(fogTypeBox);
    fogTypeLayout->setLayout(ft1);

    QGroupBox *skyboxLayout = new QGroupBox(); // horizonal slider 1 alignment
    QHBoxLayout *sbl = new QHBoxLayout();

    // Create slider controls to control parameters
    skyboxSlider = new QSlider(Qt::Orientation::Horizontal); // Parameter 1 slider
    skyboxSlider->setTickInterval(1);
    skyboxSlider->setMinimum(0);
    skyboxSlider->setMaximum(1);
    skyboxSlider->setValue(1);

    skyboxBox = new QSpinBox();
    skyboxBox->setMinimum(0);
    skyboxBox->setMaximum(1);
    skyboxBox->setSingleStep(1);
    skyboxBox->setValue(1);

    // Adds the slider and number box to the parameter layouts
    sbl->addWidget(skyboxSlider);
    sbl->addWidget(skyboxBox);
    skyboxLayout->setLayout(sbl);


//    // Extra Credit:
//    ec1 = new QCheckBox();
//    ec1->setText(QStringLiteral("Extra Credit 1"));
//    ec1->setChecked(false);

    vLayout->addWidget(uploadFile);
    vLayout->addWidget(tesselation_label);
    vLayout->addWidget(param1_label);
    vLayout->addWidget(p1Layout);
    vLayout->addWidget(param2_label);
    vLayout->addWidget(p2Layout);
    vLayout->addWidget(camera_label);
    vLayout->addWidget(near_label);
    vLayout->addWidget(nearLayout);
    vLayout->addWidget(far_label);
    vLayout->addWidget(farLayout);

    vLayout->addWidget(fog_label);
    vLayout->addWidget(fogLayout);

    vLayout->addWidget(fog_type_label);
    vLayout->addWidget(fogTypeLayout);

    vLayout->addWidget(clouds_checkbox);
    vLayout->addWidget(skybox_label);
    vLayout->addWidget(skyboxLayout);
    // Extra Credit:
//    vLayout->addWidget(ec_label);


    connectUIElements();

    // Set default values of 5 for tesselation parameters
    onValChangeP1(5);
    onValChangeP2(5);

    // Set default values for near and far planes
    onValChangeNearBox(0.1f);
    onValChangeFarBox(100.f);
    onValChangeFogBox(0.0f);
}

void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    connectUploadFile();
    connectParam1();
    connectParam2();
    connectNear();
    connectFar();
    connectFog();
    connectFogType();
    connectSkybox();
    connectCloudsToggle();
}


void MainWindow::connectUploadFile() {
    connect(uploadFile, &QPushButton::clicked, this, &MainWindow::onUploadFile);
}

void MainWindow::connectParam1() {
    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1);
    connect(p1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP1);
}

void MainWindow::connectParam2() {
    connect(p2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP2);
    connect(p2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP2);
}

void MainWindow::connectNear() {
    connect(nearSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNearSlider);
    connect(nearBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeNearBox);
}

void MainWindow::connectFar() {
    connect(farSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFarSlider);
    connect(farBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFarBox);
}
void MainWindow::connectFog() {
    connect(fogSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFogSlider);
    connect(fogBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFogBox);
}
void MainWindow::connectFogType() {
    connect(fogTypeSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFogType);
    connect(fogTypeBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeFogType);
}
void MainWindow::connectSkybox() {
    connect(skyboxSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeSkybox);
    connect(skyboxBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeSkybox);
}

void MainWindow::connectCloudsToggle() {
    connect(clouds_checkbox, &QCheckBox::toggled, this, &MainWindow::onCloudsToggle);
}



//void MainWindow::connectExtraCredit() {
//    connect(ec1, &QCheckBox::clicked, this, &MainWindow::onExtraCredit1);
//    connect(ec2, &QCheckBox::clicked, this, &MainWindow::onExtraCredit2);
//    connect(ec3, &QCheckBox::clicked, this, &MainWindow::onExtraCredit3);
//    connect(ec4, &QCheckBox::clicked, this, &MainWindow::onExtraCredit4);
//}

void MainWindow::onCloudsToggle() {
    settings.cloudsToggle = !settings.cloudsToggle;
    realtime->settingsChanged();
}

void MainWindow::onUploadFile() {
    // Get abs path of scene file
    QString configFilePath = QFileDialog::getOpenFileName(this, tr("Upload File"), QDir::homePath(), tr("Scene Files (*.xml)"));
    if (configFilePath.isNull()) {
        std::cout << "Failed to load null scenefile." << std::endl;
        return;
    }

    settings.sceneFilePath = configFilePath.toStdString();

    std::cout << "Loaded scenefile: \"" << configFilePath.toStdString() << "\"." << std::endl;

    realtime->sceneChanged();
}

void MainWindow::onValChangeP1(int newValue) {
    p1Slider->setValue(newValue);
    p1Box->setValue(newValue);
    settings.shapeParameter1 = p1Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeP2(int newValue) {
    p2Slider->setValue(newValue);
    p2Box->setValue(newValue);
    settings.shapeParameter2 = p2Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearSlider(int newValue) {
    //nearSlider->setValue(newValue);
    nearBox->setValue(newValue/100.f);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarSlider(int newValue) {
    //farSlider->setValue(newValue);
    farBox->setValue(newValue/100.f);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearBox(double newValue) {
    nearSlider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarBox(double newValue) {
    farSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFogSlider(int newValue) {
    fogBox->setValue(newValue/100.f);
    settings.fogValue = fogBox->value();
    realtime->settingsChanged();
}
void MainWindow::onValChangeFogBox(double newValue) {
    fogSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.fogValue = fogBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFogType(int newValue) {
    fogTypeSlider->setValue(newValue);
    fogTypeBox->setValue(newValue);
    settings.fogType = fogTypeSlider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeSkybox(int newValue) {
    skyboxSlider->setValue(newValue);
    skyboxBox->setValue(newValue);
    settings.m_skybox_type = skyboxSlider->value();
    realtime->settingsChanged();
}


//// Extra Credit:

//void MainWindow::onExtraCredit1() {
//    settings.extraCredit1 = !settings.extraCredit1;
//    realtime->settingsChanged();
//}
