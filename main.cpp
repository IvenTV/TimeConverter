#include <iostream>
#include <iomanip>
#include "TimeConverter.h"
#include "Time.h"

int main() {
    std::cout << "========= Тест TimeConverter =========" << std::endl;

    // Входные параметры
    TimeScale inputScale = TimeScale::TAI; //ШКАЛА ВРЕМЕНИ КОТОРУЮ МОЖНО ПОМЕНЯТЬ
    int year   = 2026;
    int month  = 9;
    int day    = 24;
    int hour   = 15;
    int min    = 30;
    double sec = 0.0;

    // Установка времени
    TimeConverter converter;
    if (!converter.setTime(inputScale, year, month, day, hour, min, sec)) {
        std::cerr << "Не удалось установить время!" << std::endl;
        return 1;
    }

    // Вывод введённого времени
    std::cout << "\nВремя установлено в " << scaleToString(inputScale) << ": "
              << std::setfill('0')
              << std::setw(4) << year << "-"
              << std::setw(2) << month << "-"
              << std::setw(2) << day << " "
              << std::setw(2) << hour << ":"
              << std::setw(2) << min << ":"
              << std::fixed << std::setprecision(3)
              << std::setw(6) << sec
              << std::endl;

    // Конвертация во все шкалы
    std::cout << "\n--- Конвертация во все шкалы ---\n" << std::endl;

    const TimeScale allScales[] = {
        TimeScale::UTC, TimeScale::TAI, TimeScale::TT,
        TimeScale::UT1, TimeScale::TCG, TimeScale::TDB, TimeScale::TCB
    };

    std::cout << std::setfill(' ');

    for (TimeScale s : allScales) {
        TimeJD jd = converter.getTime(s);
        TimeJD norm = jd.normalized();

        // Название шкалы — слева, фиксированная ширина 4 (UTC, TAI, TT, UT1, TCG, TDB, TCB)
        std::cout << std::left << std::setw(4) << scaleToString(s) << " | "
                  // Календарная дата
                  << converter.getCalendarString(s, 6)
                  // JD: целая часть без точки + дробная часть отдельно
                  << "  |  JD = "
                  << std::right << std::fixed << std::setprecision(1)
                  << std::setw(10) << norm.jdInt
                  << " + " << std::setprecision(9) << norm.jdFrac
                  << std::endl;
    }

    std::cout << "\n========= Тест завершён =========" << std::endl;
    return 0;
}