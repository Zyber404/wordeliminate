
#ifndef CHALLENGER_H
#define CHALLENGER_H

#include "user.h"





class Challenger :public User
{ 
public:
    Challenger(QString username, int level = 1);
    void setchallenger(QString username,int level ,int completed_stages,int exp);
    virtual ~Challenger();
    void addexp(int exptoAdd);
    int getexp();
    int getlevel();
    int getcompletedStages();
    QString getusername();
    void incCompletedStages(int exptoAdd);
    void updateLocal();
    void updateDatabase();
private:
    int completedStages = 0;
    int exp = 0;
};

#endif // CHALLENGER_H
