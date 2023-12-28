#ifndef WORKITEM_H
#define WORKITEM_H

#include <string>
#include <ctime>
class WorkItem
{

protected:
    WorkItem *parent = nullptr;

private:
    std::string text;
    std::string comment;
    tm endDate = {0};
    tm startDate = {0};
    bool dateValid = false;
    void dateIsValid(bool valid);

public:
    WorkItem(WorkItem* parent);
    WorkItem();
    virtual ~WorkItem();
    WorkItem *getParent();

    void addComment(std::string comment);
    std::string getComment();
    bool hasComment();
    void addText(std::string text);
    std::string getText();
    virtual std::string getFormattedInfo() = 0;

    static const int START_DATE = 1;
    static const int END_DATE = 0;
    static void getNow(tm *date);
    void setDate(const int type, unsigned int day, unsigned int month, unsigned int year);
    void setDate(const int type, std::string date);
    void setDate(const int type, tm date);
    std::string getDateAsString(const int type);
    tm getDate(const int type);
    bool dateIsValid();
    bool isPastDate(tm date);
    int getRemainingDays(tm date);
    static bool sortVector(WorkItem *t1, WorkItem *t2);
    virtual bool isDone() = 0;
    virtual void setDone(bool done) = 0;
};

#endif