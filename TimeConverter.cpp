#include "TimeConverter.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

// ==================== Преобразование строк <-> enum ====================

std::optional<TimeScale> scaleFromString(const std::string& name) {
    if (name == "UTC") return TimeScale::UTC;
    if (name == "TAI") return TimeScale::TAI;
    if (name == "TT")  return TimeScale::TT;
    if (name == "UT1") return TimeScale::UT1;
    if (name == "TCG") return TimeScale::TCG;
    if (name == "TCB") return TimeScale::TCB;
    if (name == "TDB") return TimeScale::TDB;
    return std::nullopt;
}

std::string scaleToString(TimeScale scale) {
    switch (scale) {
        case TimeScale::UTC: return "UTC";
        case TimeScale::TAI: return "TAI";
        case TimeScale::TT:  return "TT";
        case TimeScale::UT1: return "UT1";
        case TimeScale::TCG: return "TCG";
        case TimeScale::TCB: return "TCB";
        case TimeScale::TDB: return "TDB";
    }
    return "???";
}

// ==================== Конструктор ====================

TimeConverter::TimeConverter()
    : initialized_(false)
    , dut1_(0.0)   // По умолчанию UT1 = UTC (грубое приближение)
    , dtdb_(0.0)   // 0 означает: использовать iauDtdb
{
}

// ==================== Прямые конвертации (из TAI) ====================

TimeJD TimeConverter::utcToTai(double utc1, double utc2) const {
    double tai1, tai2;
    iauUtctai(utc1, utc2, &tai1, &tai2);
    return TimeJD{ tai1, tai2 }.normalized();
}

TimeJD TimeConverter::taiToUtc(double tai1, double tai2) const {
    double utc1, utc2;
    iauTaiutc(tai1, tai2, &utc1, &utc2);
    return { utc1, utc2 };
}

TimeJD TimeConverter::taiToUt1(double tai1, double tai2) const {
    TimeJD utc = taiToUtc(tai1, tai2);
    double ut11, ut12;
    iauUtcut1(utc.jdInt, utc.jdFrac, dut1_, &ut11, &ut12);
    return { ut11, ut12 };
}

TimeJD TimeConverter::taiToTt(double tai1, double tai2) const {
    double tt1, tt2;
    iauTaitt(tai1, tai2, &tt1, &tt2);
    return { tt1, tt2 };
}

TimeJD TimeConverter::ttToTcg(double tt1, double tt2) const {
    double tcg1, tcg2;
    iauTttcg(tt1, tt2, &tcg1, &tcg2);
    return { tcg1, tcg2 };
}

TimeJD TimeConverter::ttToTdb(double tt1, double tt2) const {
    double dtr;
    if (dtdb_ != 0.0) {
        dtr = dtdb_;
    } else {
        dtr = iauDtdb(tt1, tt2, 0.0, 0.0, 0.0, 0.0);
    }
    double tdb1, tdb2;
    iauTttdb(tt1, tt2, dtr, &tdb1, &tdb2);
    return { tdb1, tdb2 };
}

TimeJD TimeConverter::tdbToTcb(double tdb1, double tdb2) const {
    double tcb1, tcb2;
    iauTdbtcb(tdb1, tdb2, &tcb1, &tcb2);
    return { tcb1, tcb2 };
}

// ==================== Обратные конвертации (в TAI) ====================

TimeJD TimeConverter::ttToTai(double tt1, double tt2) const {
    double tai1, tai2;
    iauTttai(tt1, tt2, &tai1, &tai2);
    return { tai1, tai2 };
}

TimeJD TimeConverter::ut1ToTai(double ut11, double ut12) const {
    double utc1, utc2;
    iauUt1utc(ut11, ut12, dut1_, &utc1, &utc2);
    return utcToTai(utc1, utc2);
}

TimeJD TimeConverter::tcgToTai(double tcg1, double tcg2) const {
    double tt1, tt2;
    iauTcgtt(tcg1, tcg2, &tt1, &tt2);
    return ttToTai(tt1, tt2);
}

TimeJD TimeConverter::tdbToTai(double tdb1, double tdb2) const {
    double dtr;
    if (dtdb_ != 0.0) {
        dtr = dtdb_;
    } else {
        dtr = iauDtdb(tdb1, tdb2, 0.0, 0.0, 0.0, 0.0);
    }
    double tt1, tt2;
    iauTdbtt(tdb1, tdb2, dtr, &tt1, &tt2);
    return ttToTai(tt1, tt2);
}

TimeJD TimeConverter::tcbToTai(double tcb1, double tcb2) const {
    double tdb1, tdb2;
    iauTcbtdb(tcb1, tcb2, &tdb1, &tdb2);
    return tdbToTai(tdb1, tdb2);
}

// ==================== setTime ====================

bool TimeConverter::setTime(TimeScale scale, int year, int month, int day,
                            int hour, int min, double sec) {
    double jd1, jd2;
    int status = iauDtf2d(scaleToString(scale).c_str(),
                          year, month, day, hour, min, sec,
                          &jd1, &jd2);
    if (status != 0) {
        std::cerr << "Ошибка iauDtf2d для шкалы " << scaleToString(scale)
                  << ", код: " << status << std::endl;
        return false;
    }

    switch (scale) {
        case TimeScale::TAI:
            tai_ = { jd1, jd2 };
            break;
        case TimeScale::UTC:
            tai_ = utcToTai(jd1, jd2);
            break;
        case TimeScale::TT:
            tai_ = ttToTai(jd1, jd2);
            break;
        case TimeScale::UT1:
            tai_ = ut1ToTai(jd1, jd2);
            break;
        case TimeScale::TCG:
            tai_ = tcgToTai(jd1, jd2);
            break;
        case TimeScale::TDB:
            tai_ = tdbToTai(jd1, jd2);
            break;
        case TimeScale::TCB:
            tai_ = tcbToTai(jd1, jd2);
            break;
    }

    initialized_ = true;
    return true;
}

bool TimeConverter::setTime(const std::string& scaleName, int year, int month, int day,
                            int hour, int min, double sec) {
    auto scale = scaleFromString(scaleName);
    if (!scale) {
        std::cerr << "Неизвестная шкала времени: " << scaleName << std::endl;
        return false;
    }
    return setTime(*scale, year, month, day, hour, min, sec);
}

bool TimeConverter::setTime(const Time& t) {
    double jd1 = t.jdInt();
    double jd2 = t.jdFrac();

    switch (t.scale()) {
        case TimeScale::TAI: tai_ = { jd1, jd2 }; break;
        case TimeScale::UTC: tai_ = utcToTai(jd1, jd2); break;
        case TimeScale::TT:  tai_ = ttToTai(jd1, jd2);  break;
        case TimeScale::UT1: tai_ = ut1ToTai(jd1, jd2); break;
        case TimeScale::TCG: tai_ = tcgToTai(jd1, jd2); break;
        case TimeScale::TDB: tai_ = tdbToTai(jd1, jd2); break;
        case TimeScale::TCB: tai_ = tcbToTai(jd1, jd2); break;
    }

    initialized_ = true;
    return true;
}

// ==================== getTime ====================

TimeJD TimeConverter::getTime(TimeScale scale) const {
    if (!initialized_) {
        return { 0.0, 0.0 };
    }

    switch (scale) {
        case TimeScale::TAI:
            return tai_;

        case TimeScale::UTC:
            return taiToUtc(tai_.jdInt, tai_.jdFrac);

        case TimeScale::TT:
            return taiToTt(tai_.jdInt, tai_.jdFrac);

        case TimeScale::UT1:
            return taiToUt1(tai_.jdInt, tai_.jdFrac);

        case TimeScale::TCG: {
            TimeJD tt = taiToTt(tai_.jdInt, tai_.jdFrac);
            return ttToTcg(tt.jdInt, tt.jdFrac);
        }

        case TimeScale::TDB: {
            TimeJD tt = taiToTt(tai_.jdInt, tai_.jdFrac);
            return ttToTdb(tt.jdInt, tt.jdFrac);
        }

        case TimeScale::TCB: {
            TimeJD tt = taiToTt(tai_.jdInt, tai_.jdFrac);
            TimeJD tdb = ttToTdb(tt.jdInt, tt.jdFrac);
            return tdbToTcb(tdb.jdInt, tdb.jdFrac);
        }
    }

    return { 0.0, 0.0 };
}

TimeJD TimeConverter::getTime(const std::string& scaleName) const {
    auto scale = scaleFromString(scaleName);
    if (!scale) {
        std::cerr << "Неизвестная шкала времени: " << scaleName << std::endl;
        return { 0.0, 0.0 };
    }
    return getTime(*scale);
}

Time TimeConverter::getTimeAsTime(TimeScale scale) const {
    TimeJD jd = getTime(scale);
    return Time(jd.jdInt, jd.jdFrac, scale);
}

// ==================== getCalendarString ====================

std::string TimeConverter::getCalendarString(TimeScale scale, int ndp) const {
    TimeJD jd = getTime(scale);
    if (jd.jdInt == 0.0 && jd.jdFrac == 0.0) {
        return "N/A";
    }

    int iy, im, id, ihmsf[4];
    int status = iauD2dtf(scaleToString(scale).c_str(), ndp,
                          jd.jdInt, jd.jdFrac,
                          &iy, &im, &id, ihmsf);
    if (status != 0) {
        return "Ошибка форматирования";
    }

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << iy << "-"
        << std::setw(2) << im << "-"
        << std::setw(2) << id << " "
        << std::setw(2) << ihmsf[0] << ":"
        << std::setw(2) << ihmsf[1] << ":"
        << std::setw(2) << ihmsf[2] << "."
        << std::setw(ndp) << ihmsf[3];

    return oss.str();
}

std::string TimeConverter::getCalendarString(const std::string& scaleName, int ndp) const {
    auto scale = scaleFromString(scaleName);
    if (!scale) {
        std::cerr << "Неизвестная шкала времени: " << scaleName << std::endl;
        return "N/A";
    }
    return getCalendarString(*scale, ndp);
}