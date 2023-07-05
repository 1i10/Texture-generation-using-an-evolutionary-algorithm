## Навигация   
[1. Краткое описание](#Description)  
[2. Структура проекта](#Structure)  
[3. Установка](#Installation)  
[4. Описание функционала](#FunctionalDescription)  
[5. Примеры](#Example)   

<a name="Description"><h2>Краткое описание</h2></a>
Приложение было разработано на языке C++ (std c++17) в среде QT Creator 9.0.2 с использованием фреймворка QT 6.4.2, библиотеки компьютерного зрения OpenCV 4.5.5 под ОС Windows.  
Приложение позволяет пользователю выполнить генерацию текстуры с использованием эволюционного алгоритма, а также отслеживать процесс генерации, что позволяет проанализировать работу выбранного алгоритма.  
  
**Данный проект является частью моей дипломной работы. Ознакомиться с ней и получить более подробную информацию по проекту можно [здесь](https://drive.google.com/file/d/10iDUbzhwNAOBEf9riqyyyjMus497zyBn/view?usp=sharing).**
  
<a name="Structure"><h2>Структура проекта</h2></a> 
 
* [*Data*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/Data) -  содержит файлы .csv с вычисленными характеристиками (Признаки Тамуры, MSE) для определенного набора изображений текстуры;

* [*TextureTree*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/TextureTree) - содержит набор изображений текстур дерева;

* [*Example*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/Example) - содержит изображения для описания данного репозитория;

* [*EvolutionaryAlorithm.cpp*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/EvolutionaryAlorithm.cpp) / [*EvolutionaryAlorithm.h*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/EvolutionaryAlorithm.h) - базовый класс для эволюционных алгоритмов. Содержит данные о популяции, количестве поколений, вероятности мутации,вероятности скрещивания, лучшем индивиде (текстуре), векторе весов для признаков Тамуры и MSE. Обеспечивает основную структуру и функциональность эволюционного алгоритма;

* [*GenerationTexture.pro*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/GenerationTexture.pro) - файл проекта, используемый qmake для создания приложения;

* [*GenerationTextureInterface.cpp*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/GenerationTextureInterface.cpp) / [*GenerationTextureInterface.h*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/GenerationTextureInterface.h) / [*GenerationTextureInterface.ui*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/GenerationTextureInterface.ui) - интерфейс для генерации текстуры, предоставляющий графический пользовательский интерфейс. Обеспечивает взаимодействие с пользователем для настройки параметров генерации текстуры и отображения результатов;

* [*ImageData.cpp*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/ImageData.cpp) / [*ImageData.h*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/ImageData.h) - класс, представляющий данные об изображении. Включает имя изображения, матрицу пикселей, признаки Тамуры, значение MSE, значение целеой функции, ранг особи. Предоставляет методы для установки матрицы пикселей, вычисления признаков Тамуры и MSE, вычисления целевой функции и получения значений свойств изображения;

* [*MSE.cpp*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/MSE.cpp) / [*MSE.h*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/MSE.h) - класс, вычисляющий среднеквадратичную ошибку (MSE) между двумя изображениями. Предоставляет методы для вычисления MSE между двумя матрицами изображений, вычисления MSE для одного изображения и набора изображений, а также для печати значения MSE;

* [*NSGA3.cpp*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/NSGA3.cpp) / [*NSGA.h*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/NSGA3.h) - класс, наследуемый от EvolutionaryAlgorithm, который реализует алгоритм NSGA-III (Non-dominated Sorting Genetic Algorithm III). Он предоставляет конкретную реализацию метода «run» для выполнения NSGA-III;

* [*QCustomPlot.cpp*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/QCustomPlot.cpp) / [*QCustomPlot.h*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/QCustomPlot.h) - виджет для построения графиков и визуализации данных [(Источник)](https://www.qcustomplot.com/);

* [*Random.cpp*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/Random.cpp) / [*Random.h*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/Random.h) - модуль, содержащий функции для работы с генерацией случайных чисел. Включает функцию для генерации случайной перестановки, функцию для генерации случайного числа от 0.0 до 1.0;

* [*TamuraFeatures.cpp*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/TamuraFeatures.cpp) / [*TamuraFeatures.h*](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/TamuraFeatures.h) - класс, вычисляющий признаки Тамуры для изображения. Особенности включают грубость, контраст, направленность, линейность, регулярность и зернистость. Предоставляет методы для вычисления каждого из признаков на основе матрицы изображения. Также содержит вспомогательные методы для вычисления различных параметров, используемых при расчете признаков.
  
<a name="Installation"><h2>Установка</h2></a>
  
1. Загрузить проект на локальную машину, воспользовавшись командой git clone https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm.git  
2. Открыть GenerationTexture.pro в Qt Creator (используемый компилятор MinGW) 
3. Изменить путь к библиотеке OpenCV в файле GenerationTexture.pro (35 и 36 строка)
```c++
INCLUDEPATH += D:\\Projects\\lib\\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\\include 
LIBS += -LD:\\Projects\\lib\\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\\x64\\mingw\\bin \
```
Более подробно по установке OpenCV в QT ---> [ютуб с индусом](https://youtu.be/ZOSu-2Oju-A)

<a name="FunctionalDescription"><h2>Описание функционала</h2></a>
  
Функционал программы включает в себя следующие основные возможности, приведенные в таблице ниже.  
| № п./п. | Наименование                        | Примечание                                                                                                                                                                                                                        |
|---------|-------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1       | Выбор типа генерируемой текстуры    | Пока будет доступен только один вариант для выбора – дерево                                                                                                                                                                      |
| 2       | Выбор алгоритма эволюции            | Программа поддерживает различные алгоритмы эволюции, такие как NSGA-III, MOEA/D-DE, MA-ES, LMEA, SparseEA, PM-MOEA. Каждый из этих алгоритмов имеет свои особенности и преимущества, и пользователь может выбрать наиболее подходящий для своей конкретной задачи генерации текстур |
| 3       | Настройка параметров генерации       | Пользователь имеет возможность настраивать различные параметры для контроля процесса генерации текстур. Это включает в себя настройку вероятности мутации, скрещивания, размер популяции, количество поколений.                     |
| 4       | Просмотр процесса генерации текстур | Программа предоставляет визуальные инструменты для наблюдения и анализа процесса генерации текстур. Это включает в себя отображение промежуточных результатов (лучшая текстура в поколении), график изменения качества текстур в режиме реального времени, что позволяет пользователю наблюдать процесс генерации |
| 5       | Сохранение сгенерированной текстуры | Программное обеспечение предоставляет возможность сохранить созданную текстуру в формате JPEG                                                                                                                                   |
  
<a name="Example"><h2>Примеры</h2></a>
  
Ниже представлены результаты двух экспериментов.  
*Эксперимент 1*  
![Результат эксперимента 1](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/Example/Experiment1.png)  

![Сгенерированная текстура](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/Example/Texture1.jpg)  
  
*Эксперимент 2*  
![Результат эксперимента 2](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/Example/Experiment2.png)  

![Сгенерированная текстура](https://github.com/1i10/Texture-generation-using-an-evolutionary-algorithm/tree/main/Example/Texture12.jpg)  