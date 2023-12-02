#ifndef DATE_H
#define DATE_H

#include <string>

class Date
{
public:
    static bool isValidDateFormat(const std::string *date);
    static bool isValidStringFormat(const std::string* input);
static bool parseDateString(const std::string* input, tm* resultDate);
    static bool isEarlier(tm *d1, tm *d2);
    static void convertDate(std::string *date, tm *d);
    static void now(tm* date);
    static int long toLong(std::time_t *t);
    static int long toLong(tm* t);
};
#endif