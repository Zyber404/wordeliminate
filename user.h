
#ifndef USER_H
#define USER_H

#include <QString>

class User
{
protected:
    QString username;
    int level = 1;

public:
    User(QString username,  int level = 1);
    virtual ~User() ;
};

#endif // USER_H
