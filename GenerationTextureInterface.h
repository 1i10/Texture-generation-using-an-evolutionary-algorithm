#ifndef GENERATIONTEXTUREINTERFACE_H
#define GENERATIONTEXTUREINTERFACE_H

#include <QMainWindow>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "ImageData.h"
#include "NSGA3.h"
#include "QCustomPlot.h"
#include <QThread>

namespace fs = std::filesystem;

QT_BEGIN_NAMESPACE
namespace Ui { class GenerationTextureInterface; }
QT_END_NAMESPACE

class GenerationTextureInterface : public QMainWindow
{
    Q_OBJECT

public:
    GenerationTextureInterface(QWidget *parent = nullptr);
    ~GenerationTextureInterface();

    int loadData(std::string typeTexture);
    void initGraph();
    void clearGraph();

private slots:
    void onGenerateButtonClicked();
    void onThreadFinished();
    void updateBestIndivid(ImageData bestIndivid, int currentGeneration);
    void clickGraph(QCPAbstractPlottable * potItem, int num, QMouseEvent *event);
    void onSaveButtonClicked();

private:
    Ui::GenerationTextureInterface *ui;
    std::vector<ImageData> initialPopulation;
    QThread *thread1;
    NSGA3 *nsga3;
    QCPItemText* mItemText;
};
#endif // GENERATIONTEXTUREINTERFACE_H
