#ifndef DATETIME_H_INCLUDED
#define DATETIME_H_INCLUDED

#include <chrono>
#include "unicode.h"

namespace datetime {

typedef std::chrono::system_clock::time_point DateTime;
typedef std::chrono::duration<float> TimeDelta;

DateTime strptime(const unicode& date_string, const unicode& format);
unicode strftime(const DateTime& t, const unicode& format);
DateTime now(void);
float timedelta_in_seconds(TimeDelta d);
float timedelta_in_mins(TimeDelta d);

}

#endif // DATETIME_H_INCLUDED
