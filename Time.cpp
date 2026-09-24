#include "Time.h"

Time::Time() noexcept
    : jdInt_(0.0), jdFrac_(0.0), scale_(TimeScale::UTC)
{
}

Time::Time(double jdInt, double jdFrac, TimeScale scale) noexcept
    : jdInt_(jdInt), jdFrac_(jdFrac), scale_(scale)
{
}

Time Time::fromJD(double jd, TimeScale scale) noexcept {
    // Разделяем одно число на целую и дробную части
    double intPart;
    double fracPart = std::modf(jd, &intPart);
    return Time(intPart, fracPart, scale);
}

Time Time::fromMJD(double mjd, TimeScale scale) noexcept {
    // MJD = JD - 2400000.5
    return fromJD(mjd + 2400000.5, scale);
}

Time Time::fromCalendar(TimeScale scale,
                        int year, int month, int day,
                        int hour, int min, double sec) noexcept {
    double jd1, jd2;
    int status = iauDtf2d(scaleToString(scale).c_str(),
                          year, month, day, hour, min, sec,
                          &jd1, &jd2);
    if (status != 0) {
        return Time();  // Возвращаем «нулевое» время при ошибке
    }
    return Time(jd1, jd2, scale);
}