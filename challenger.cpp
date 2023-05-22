
#include "challenger.h"
#include "loginwindow.h"

#include <QSqlQuery>

Challenger::Challenger(QString username, int level)
    :User(username, level)
{

}

void Challenger::setchallenger(QString username, int level, int completed_stages, int exp)
{
    this->username = username;
    this->level = level;
    this->completedStages = completed_stages;
    this->exp = exp;
}

Challenger::~Challenger()
{

}

void Challenger::addexp(int exptoAdd)
{
    exp += exptoAdd;
    if(exp < 0){
        while(exp < 0){
            level--;
            exp += 100;
        }
        return;
    }
    if(exp >= 100){
        while(exp >= 100){
            level++;
            exp -= 100;
        }
        return;
    }
}

int Challenger::getexp()
{
    return this->exp;
}

int Challenger::getlevel()
{
    return this->level;
}

int Challenger::getcompletedStages(){
    return this->completedStages;
}

QString Challenger::getusername()
{
    return this->username;
}

void Challenger::incCompletedStages(int exptoAdd)
{
    if(exptoAdd < 0) return;
    completedStages++;
    addexp(exptoAdd);
    updateDatabase();
    return;
}

void Challenger::updateLocal()
{
    QSqlQuery q(loginWindow::database());
    q.exec(QString("SELECT * FROM challenger WHERE username = '%1'").arg(username));
    q.next();
    level = q.value("level").toInt();
    exp = q.value("exp").toInt();
    completedStages = q.value("completedStages").toInt();
}

void Challenger::updateDatabase()
{
    QSqlQuery q(loginWindow::database());
    q.exec(QString("UPDATE challenger SET level=%1, completedStages=%2, exp=%3 WHERE username = '%4'").arg(QString::number(level)).arg(QString::number(completedStages)).arg(QString::number(exp)).arg(username));
}

