#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>

int main() {
    //Значение для гамма-коррекции
    const double gamma = 1.5;

    // Открытие и чтение файла данных
    std::ifstream inputFile("dump_13122019_145433.bin", std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Не удалось открыть файл." << std::endl;
        return 1;
    }

    // Создание видеозаписи(указание разрешения)
    int frameWidth = 640;
    int frameHeight = 512;
    cv::VideoWriter videoWriter("main_example5.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 30, cv::Size(frameWidth, frameHeight), true);

    // Буфер для считанных данных одного кадра
    std::vector<uint16_t> frameData(frameWidth * frameHeight);

    //Перемещаем маркер чтения на 32 байта вперед(пропуск системной информации бинарного файла с тепловизора)
    inputFile.seekg(32, std::ios::beg);

    //Начало цикла, считывающего по 640*512*2 байтов(одного кадра из файла), обрабатывающего и записывающего в видеоряд полученного изображения
    while (!inputFile.read(reinterpret_cast<char*>(frameData.data()), frameData.size() * sizeof(uint16_t)).eof()) {
        // Создание изображения OpenCV из данных кадра
        cv::Mat frame(frameHeight, frameWidth, CV_16UC1, frameData.data());
        
        // Гамма-коррекция(с конвертацией в необходимый формат)
        frame.convertTo(frame, CV_32F);
        cv::Mat gammaCorrectedFrame;
        cv::pow(frame, gamma, gammaCorrectedFrame);

        // Масштабирование значений в диапазоне 0-255
        cv::normalize(gammaCorrectedFrame, gammaCorrectedFrame, 0, 255, cv::NORM_MINMAX, CV_8UC1);

        // Преобразование в 3-канальное черно-белое изображение
        cv::Mat grayscaleFrame;
        cv::cvtColor(gammaCorrectedFrame, grayscaleFrame, cv::COLOR_GRAY2BGR);

        // Запись кадра в видеофайл
        videoWriter.write(grayscaleFrame);
    }

    // Закрытие файлов и освобождение ресурсов
    inputFile.close();
    videoWriter.release();

    return 0;
}