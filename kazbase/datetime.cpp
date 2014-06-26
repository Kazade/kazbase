#include <chrono>
#include "datetime.h"

namespace datetime {

DateTime strptime(const unicode &date_string, const unicode &fmt) {
    std::tm tm;
    ::strptime(date_string.encode().c_str(), fmt.encode().c_str(), &tm);
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    return tp;
}

unicode strftime(const DateTime& t, const unicode& fmt) {
    time_t tt = std::chrono::system_clock::to_time_t(t);

    char buffer[256];
    ::strftime(buffer, 256, fmt.encode().c_str(), localtime(&tt));
    return unicode(buffer);
}

DateTime now(void) {
    return std::chrono::system_clock::now();
}

float timedelta_in_seconds(TimeDelta d) {
    return std::chrono::duration_cast<std::chrono::seconds>(d).count();
}

float timedelta_in_mins(TimeDelta d) {
    return std::chrono::duration_cast<std::chrono::minutes>(d).count();
}

}
