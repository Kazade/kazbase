
#include "kazbase/datetime.h"

namespace datetime {

boost::posix_time::ptime strptime(const std::string& date_string, const std::string& fmt) {
    boost::posix_time::ptime created_time;

    std::stringstream ss(date_string);
    ss.exceptions(std::ios_base::failbit);
    boost::posix_time::time_input_facet format(fmt.c_str(), 1);
    ss.imbue(std::locale(ss.getloc(), &format));
    ss >> created_time;

    return created_time;
}

std::string strftime(const boost::posix_time::ptime& t, const std::string& fmt) {
    std::ostringstream out;
    out.exceptions(std::ios_base::failbit);
    boost::posix_time::time_facet* format = new boost::posix_time::time_facet(fmt.c_str());
    out.imbue(std::locale(out.getloc(), format));
    out << t;

    return out.str();
}

DateTime now(void) {
    return boost::posix_time::second_clock::local_time();
}

float timedelta_in_seconds(TimeDelta d) {
    return float(d.total_seconds()) + (float(d.total_milliseconds()) / 1000.0f);
}

}
