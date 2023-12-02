#include <string>
#include <ctime>

#include "WorkItem.h"

WorkItem::WorkItem(WorkItem* parent)
{
		this->parent = parent;
if (parent == nullptr) { // l'unico WorkItem che avrà parent null è Project
		getNow(&startDate);
} else { // tutti gli altri prendono date dal padre
			setDate(WorkItem::START_DATE, parent->getDateAsString(WorkItem::START_DATE));
	setDate(WorkItem::END_DATE, parent->getDateAsString(WorkItem::END_DATE));
}
}

WorkItem* WorkItem::getParent() {
	return parent;
}

void WorkItem::addText(std::string t) {
	text = t;
}

std::string WorkItem::getText() {
    return text;
}

bool WorkItem::hasComment() {
	return !comment.empty();
}

void WorkItem::addComment(std::string c) {
	comment = c;
}

std::string WorkItem::getComment() {
	return comment;
}

void WorkItem::getNow(tm* date) {

		std::time_t now = std::time(0); // get time now
		tm* localTime = localtime(&now);
		*date = *localTime;
}

void WorkItem::setDate(const int type, unsigned int d, unsigned int m, unsigned int y)
{
	tm date = {0};
	date.tm_year = y - 1900;
	date.tm_mon = m - 1;
	date.tm_mday = d;
	if (type == END_DATE)
		endDate = date;
	else if (type == START_DATE)
		startDate = date;
	dateValid = true;
}

// TODO: passo un puntatore?
void WorkItem::setDate(const int type, std::string s)
{
	std::string delimiter = "/";
	std::string tokens[3];
	size_t pos = 0;
	int arrayPos = 0;
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		tokens[arrayPos++] = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());
	}
	tokens[2] = s;
	unsigned int d = atoi(tokens[0].c_str());
	unsigned int m = atoi(tokens[1].c_str());
	unsigned int y = atoi(tokens[2].c_str());
	setDate(type, d, m, y);
}

void WorkItem::setDate(const int type, tm date) {
if (type == END_DATE) endDate = date;
else if (type == START_DATE) startDate = date;
}

std::string WorkItem::getDateAsString(const int type)
{
	tm date = {0};
	if (type == END_DATE)
		date = endDate;
	else if (type == START_DATE)
		date = startDate;
	char buffer[11];
	strftime(buffer, sizeof(buffer), "%d/%m/%Y", &date);
	return std::string(buffer);
}

tm WorkItem::getDate(const int type)
{
	if (type == END_DATE)
		return endDate;
	else if (type == START_DATE)
		return startDate;
}


int WorkItem::getRemainingDays(tm date)
{
	std::time_t now = std::time(0); // get time now
	std::time_t x = std::mktime(&date);
	return std::difftime(x, now) / 86400;
}

bool WorkItem::isPastDate(tm date)
{
	return getRemainingDays(date) < 0;
}

bool WorkItem::dateIsValid()
{
	return dateValid;
}

void WorkItem::dateIsValid(bool valid)
{
	dateValid = valid;
}

bool WorkItem::sortVector(WorkItem *t1, WorkItem *t2)
{
	if (!t1->dateIsValid())
		return false;
	tm d1 = t1->getDate(END_DATE);
	tm d2 = t2->getDate(END_DATE);
	if (d1.tm_year != d2.tm_year)
		return d1.tm_year < d2.tm_year;
	if (d1.tm_mon != d2.tm_mon)
		return d1.tm_mon < d2.tm_mon;
	return d1.tm_mday < d2.tm_mday;
}