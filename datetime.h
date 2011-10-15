#ifndef DATETIME_H_INCLUDED
#define DATETIME_H_INCLUDED

#include <boost/date_time.hpp>

namespace datetime {

typedef boost::posix_time::ptime DateTime;
typedef boost::posix_time::time_duration TimeDelta;

DateTime strptime(const std::string& date_string, const std::string& format);
std::string strftime(const DateTime& t, const std::string& format);
DateTime now(void);
float timedelta_in_seconds(TimeDelta d);

}

#endif // DATETIME_H_INCLUDED
