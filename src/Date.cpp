#include <string>
#include <ctime>
#include <sstream>
#include <unordered_set>
#include <regex>
#include "Date.h"

bool Date::isValidDateFormat(const std::string *date)
{
    tm tm;
    if (strptime(date->c_str(), "%d/%m/%Y", &tm))
        return true;
    return false;
}

bool Date::isValidStringFormat(const std::string *input)
{
    std::regex formatRegex("^([-+]\\d+)\\s*(d|day|days|w|week|weeks|m|month|months|y|year|years)?$", std::regex_constants::icase);

    if (std::regex_match(*input, formatRegex))
    {
        return true;
    }

    return false;
}

bool Date::parseDateString(const std::string *input, tm *resultDate)
{
    int number = 0;
    std::string timeIndicator;
    std::stringstream ss(*input);
    while (!ss.eof())
    {
        if (isdigit(ss.peek()))
        {
            ss >> number;
        }
        else if (isalpha(ss.peek()))
        {
            ss >> timeIndicator;
            break;
        }
        else
        {
            ss.ignore();
        }
    }

    if (input->at(0) == '-')
        number = number * -1;
    // Converti l'indicatore di tempo in minuscolo
    for (char &c : timeIndicator)
    {
        c = tolower(c);
    }

    if (timeIndicator == "d" || timeIndicator == "day" || timeIndicator == "days")
    {
        resultDate->tm_mday += number;
    }
    else if (timeIndicator == "w" || timeIndicator == "week" || timeIndicator == "weeks")
    {
        resultDate->tm_mday += number * 7;
    }
    else if (timeIndicator == "m" || timeIndicator == "month" || timeIndicator == "months")
    {
        resultDate->tm_mon += number;
    }
    else if (timeIndicator == "y" || timeIndicator == "year" || timeIndicator == "years")
    {
        resultDate->tm_year += number;
    }
    std::mktime(resultDate);
}

void Date::now(tm *date)
{
    std::time_t now = std::time(0); // get time now
    tm *localTime = localtime(&now);
    *date = *localTime;
}

void Date::convertDate(std::string *date, tm *d)
{
    strptime(date->c_str(), "%d/%m/%Y", d);
}

// ritorna true se d1 è predente a d2
bool Date::isEarlier(tm *d1, tm *d2)
{
    return difftime(mktime(d1), mktime(d2)) < 0;
}

int long Date::toLong(std::time_t *t)
{
    return static_cast<long int>(*t);
}

int long Date::toLong(tm *t)
{
    time_t time = {0};
    time = mktime(t);
    return toLong(&time);
}