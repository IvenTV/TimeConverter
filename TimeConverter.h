#pragma once
#include <string>
#include <optional>
#include "Time.h"
#include "sofa.h"

// Результат: JD, представленный двумя частями (целая + дробная)
struct TimeJD {
    double jdInt;   // целая часть юлианского дня
    double jdFrac;  // дробная часть, 0 <= jdFrac < 1

    double jd() const { return jdInt + jdFrac; }

    // Привести к каноническому виду: jdInt — целое, jdFrac ∈ [0, 1)
    TimeJD normalized() const {
        double intPart;
        double fracPart = std::modf(jdInt, &intPart);  // intPart — целое
        double totalFrac = fracPart + jdFrac;          // суммарная дробная часть

        // Если totalFrac >= 1 или < 0 — переносим в целую часть
        double extra;
        double newFrac = std::modf(totalFrac, &extra);
        if (newFrac < 0.0) {
            newFrac += 1.0;
            extra -= 1.0;
        }
        return { intPart + extra, newFrac };
    }
};

class TimeConverter {
public:
    TimeConverter();

    // Установить входное время (календарный формат) в заданной шкале
    bool setTime(TimeScale scale, int year, int month, int day,
                 int hour, int min, double sec);

    // Удобная обёртка: имя шкалы строкой
    bool setTime(const std::string& scaleName, int year, int month, int day,
                 int hour, int min, double sec);

    // Установить время из объекта Time
    bool setTime(const Time& t);

    // Получить время в заданной шкале (в виде JD)
    TimeJD getTime(TimeScale scale) const;
    TimeJD getTime(const std::string& scaleName) const;

    // Получить результат как объект Time
    Time getTimeAsTime(TimeScale scale) const;

    // Получить время в календарном формате (строкой)
    std::string getCalendarString(TimeScale scale, int ndp = 6) const;
    std::string getCalendarString(const std::string& scaleName, int ndp = 6) const;

    bool isInitialized() const { return initialized_; }

    // Настройки разниц между шкалами
    void setDut1(double dut1) { dut1_ = dut1; }   // ΔUT1 = UT1 - UTC, секунды
    void setDtdb(double dtdb) { dtdb_ = dtdb; }   // TDB - TT, секунды

private:
    // Центральное представление — JD в шкале TAI
    TimeJD tai_;
    bool initialized_;

    double dut1_;   // ΔUT1 = UT1 - UTC, секунды
    double dtdb_;   // TDB - TT, секунды (0 = использовать iauDtdb)

    // Прямые конвертации (из TAI)
    TimeJD utcToTai(double utc1, double utc2) const;
    TimeJD taiToUtc(double tai1, double tai2) const;
    TimeJD taiToUt1(double tai1, double tai2) const;
    TimeJD taiToTt(double tai1, double tai2) const;
    TimeJD ttToTcg(double tt1, double tt2) const;
    TimeJD ttToTdb(double tt1, double tt2) const;
    TimeJD tdbToTcb(double tdb1, double tdb2) const;

    // Обратные конвертации (в TAI)
    TimeJD ttToTai(double tt1, double tt2) const;
    TimeJD ut1ToTai(double ut11, double ut12) const;
    TimeJD tcgToTai(double tcg1, double tcg2) const;
    TimeJD tdbToTai(double tdb1, double tdb2) const;
    TimeJD tcbToTai(double tcb1, double tcb2) const;
};