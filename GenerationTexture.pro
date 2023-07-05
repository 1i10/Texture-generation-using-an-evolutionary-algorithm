QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    EvolutionaryAlgorithm.cpp \
    ImageData.cpp \
    MSE.cpp \
    NSGA3.cpp \
    TamuraFeatures.cpp \
    main.cpp \
    GenerationTextureInterface.cpp \
    QCustomPlot.cpp \
    Random.cpp

HEADERS += \
    EvolutionaryAlgorithm.h \
    ImageData.h \
    MSE.h \
    NSGA3.h \
    TamuraFeatures.h \
    GenerationTextureInterface.h \
    QCustomPlot.h \
    Random.h

FORMS += \
    GenerationTextureInterface.ui

INCLUDEPATH += D:\\Projects\\lib\\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\\include
LIBS += -LD:\\Projects\\lib\\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\\x64\\mingw\\bin \
    -llibopencv_calib3d455 \
    -llibopencv_core455 \
    -llibopencv_dnn455 \
    -llibopencv_features2d455 \
    -llibopencv_flann455 \
    -llibopencv_gapi455 \
    -llibopencv_highgui455 \
    -llibopencv_imgcodecs455 \
    -llibopencv_imgproc455 \
    -llibopencv_ml455 \
    -llibopencv_objdetect455 \
    -llibopencv_photo455 \
    -llibopencv_stitching455 \
    -llibopencv_video455

QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

