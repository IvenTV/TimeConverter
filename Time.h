#pragma once
#include <string>
#include <optional>
#include "sofa.h"

// Перечисление шкал (определено здесь, чтобы Time и TimeConverter могли его использовать)
enum class TimeScale {
    UTC, TAI, TT, UT1, TCG, TCB, TDB
};

std::optional<TimeScale> scaleFromString(const std::string& name);
std::string scaleToString(TimeScale scale);

// Класс времени — хранит целую и дробную части JD и шкалу
class Time {
public:
    // Пустой конструктор (по умолчанию — нулевое время в UTC)
    Time() noexcept;

    // Основной конструктор: целая + дробная часть дня + шкала
    Time(double jdInt, double jdFrac, TimeScale scale = TimeScale::UTC) noexcept;

    // Фабрики из разных форматов
    static Time fromJD(double jd, TimeScale scale = TimeScale::UTC) noexcept;
    static Time fromMJD(double mjd, TimeScale scale = TimeScale::UTC) noexcept;
    static Time fromCalendar(TimeScale scale,
                             int year, int month, int day,
                             int hour = 0, int min = 0, double sec = 0.0) noexcept;

    // Геттеры
    double jdInt() const noexcept { return jdInt_; }
    double jdFrac() const noexcept { return jdFrac_; }
    double jd() const noexcept { return jdInt_ + jdFrac_; }
    double mjd() const noexcept { return jd() - 2400000.5; }
    TimeScale scale() const noexcept { return scale_; }

private:
    double jdInt_;    // целая часть юлианского дня
    double jdFrac_;   // дробная часть юлианского дня
    TimeScale scale_; // шкала времени
};