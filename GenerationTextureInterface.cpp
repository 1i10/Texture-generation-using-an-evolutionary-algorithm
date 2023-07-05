#include "GenerationTextureInterface.h"
#include "Random.h"
#include "ui_generationtextureinterface.h"

GenerationTextureInterface::GenerationTextureInterface(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GenerationTextureInterface)
{
    ui->setupUi(this);

    initGraph();

    connect(ui->generateButton, &QAbstractButton::clicked, this, &GenerationTextureInterface::onGenerateButtonClicked);
    connect(ui->saveButton, &QAbstractButton::clicked, this, &GenerationTextureInterface::onSaveButtonClicked);
}

GenerationTextureInterface::~GenerationTextureInterface()
{
    delete ui;
    delete thread1;
    delete nsga3;
}

int GenerationTextureInterface::loadData(std::string typeTexture)
{
    std::string directory = "../GenerationTexture/" + typeTexture;
    std::string filePath = "../GenerationTexture/Data/" + typeTexture + ".csv";

    if (!fs::is_directory(directory))
    {
        std::cout << "Invalid directory path." << std::endl;
        return -1;
    }

    std::ifstream inputFile(filePath);
    if (!inputFile.is_open())
    {
        std::cout << "Failed to open the input file." << std::endl;
        return -1;
    }

    // Загрузка значений из файла
    std::string line;

    while (std::getline(inputFile, line))
    {
        std::istringstream iss(line);
        std::string token;

        // Чтение значений из строки
        std::string imageName;
        std::vector<double> tamuraFeatures(6);
        double mseValue;

        std::getline(iss, imageName, ';');
        for (double& feature : tamuraFeatures)
        {
            std::getline(iss, token, ';');
            feature = std::stod(token);
        }
        std::getline(iss, token, ';');
        mseValue = std::stod(token);

        cv::Mat image = cv::imread(directory + "\\" + imageName, cv::IMREAD_GRAYSCALE);
        if (image.empty())
        {
            std::cout << "Failed to load the image: " << imageName << std::endl;
            continue;
        }

        // Создание объекта ImageData и добавление в начальную популяцию
        TamuraFeatures tamuraFeaturesObj(tamuraFeatures[0], tamuraFeatures[1], tamuraFeatures[2], tamuraFeatures[3], tamuraFeatures[4],
                tamuraFeatures[5]);
        MSE mse(mseValue);
        ImageData imageInfo(imageName, image, tamuraFeaturesObj, mse);
        initialPopulation.push_back(imageInfo);
    }

    inputFile.close();

    return 0;
}

void GenerationTextureInterface::initGraph()
{
    ui->graphicsView->addGraph();
    ui->graphicsView->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->graphicsView->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->graphicsView->xAxis->setLabel("Поколение");
    ui->graphicsView->yAxis->setLabel("Качество текстур");

    ui->graphicsView->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables | QCP::iSelectAxes);

    //подпись точки
    QCPItemText* itemText = new QCPItemText(ui->graphicsView);
    itemText->setLayer("overlay");
    itemText->setPositionAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    itemText->position->setType(QCPItemPosition::ptAxisRectRatio);
    itemText->position->setCoords(0.5,0.1);
    itemText->setPadding(QMargins(5,5,5,5));
    itemText->setPen(QPen(Qt::black));
    itemText->setBrush(QColor(255,255,255));
    mItemText = itemText;
    ui->graphicsView->layer("overlay")->setVisible(false);

    connect(ui->graphicsView, &QCustomPlot::plottableClick, this, &GenerationTextureInterface::clickGraph);
}

void GenerationTextureInterface::clearGraph()
{
    //очищаем график
    ui->graphicsView->graph(0)->data()->clear();
    ui->graphicsView->layer("overlay")->setVisible(false);
    ui->graphicsView->replot();
    ui->graphicsView->update();
}

void GenerationTextureInterface::onGenerateButtonClicked()
{
    clearGraph();
    //блокируем кнопки на момент генерации
    ui->generateButton->setEnabled(false);
    ui->saveButton->setEnabled(false);

    //устанавливаем значение для прогресс бара
    ui->progressBar->reset();
    ui->progressBar->setMaximum(ui->generationsSpinBox->value());

    //получаем данные для выбранного типа текстуры
    QString selectedType = ui->textureTypeComboBox->currentText();
    if(selectedType == "Дерево")
    {
        loadData("TextureTree");
    }

    //выбираем данные, которые будут учавствовать в генерации
    std::vector<ImageData> population;

    if (ui->populationSizeSpinBox->value() < static_cast<int>(initialPopulation.size()))
    {
        // Генерация случайной перестановки индексов для отбора
        std::vector<int> index1(initialPopulation.size());

        randomPermutation(index1.data(), static_cast<int>(initialPopulation.size()));

        // Выбираем первые populationSize индексов и добавляем соответствующие элементы в популяцию
        for (int i = 0; i < ui->populationSizeSpinBox->value(); ++i)
        {
            population.push_back(initialPopulation[index1[i]]);
        }
    }
    else
    {
        population = initialPopulation;
    }

    //создаем новый поток
    thread1 = new QThread();

    //выполняем выбранный алгоритм
    QString selectedAlgorithm = ui->evolutionAlgorithmComboBox->currentText();
    if(selectedAlgorithm == "NSGA-III")
    {
        nsga3 = new NSGA3(population, ui->generationsSpinBox->value(),
                    ui->mutationProbabilitySpinBox->value(), ui->crossoverProbabilitySpinBox->value(), 7);

        connect(nsga3, &NSGA3::bestIndividChanged, this, &GenerationTextureInterface::updateBestIndivid);
        connect(thread1, &QThread::started, nsga3, &NSGA3::run);
        connect(nsga3, &NSGA3::finished, this, &GenerationTextureInterface::onThreadFinished);

        nsga3->moveToThread(thread1);
    }

    thread1->start();
}

void GenerationTextureInterface::onThreadFinished()
{
    thread1->quit();
    ui->generateButton->setEnabled(true);
    ui->saveButton->setEnabled(true);
}

void GenerationTextureInterface::updateBestIndivid(ImageData bestIndivid, int currentGeneration)
{
    cv::Mat pixelMatrix = bestIndivid.getPixelMatrix();
    QPixmap pixmap = QPixmap::fromImage(QImage(pixelMatrix.data, pixelMatrix.cols, pixelMatrix.rows, pixelMatrix.step, QImage::Format_Grayscale8));
    ui->textureImageLabel->setPixmap(pixmap);
    ui->textureImageLabel->setScaledContents(true);
    ui->textureImageLabel->update();

    //ставим точку лучшего индивида на график
    ui->graphicsView->graph(0)->addData(currentGeneration, bestIndivid.getTargetFunction());
    ui->graphicsView->rescaleAxes(true);
    ui->graphicsView->yAxis->moveRange(0.01);
    ui->graphicsView->xAxis->moveRange(0.1);
    ui->graphicsView->replot();
    ui->graphicsView->update();

    //увеличиваем прогресс бар
    ui->progressBar->setValue(currentGeneration);
}

void GenerationTextureInterface::clickGraph(QCPAbstractPlottable * potItem, int num, QMouseEvent *event)
{
    double dX = potItem->interface1D()->dataMainKey(num);
    double dY = potItem->interface1D()->dataMainValue(num);

    QString textPoint = QString("Поколение = %1\nКачество = %2").arg(QString::number(dX)).arg(QString::number(dY));

    ui->graphicsView->layer("overlay")->setVisible(true);
    mItemText->setText(textPoint);

    ui->graphicsView->replot();
}


void GenerationTextureInterface::onSaveButtonClicked()
{
    if(!ui->textureImageLabel->pixmap().isNull())
    {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save Image File"), QString(), tr("Images (*.jpg)"));
        if(!filename.isEmpty())
        {
            ui->textureImageLabel->pixmap().save(filename);
        }
    }
}

