#include "stage.h"
#include "loginwindow.h"

#include <QDebug>
#include <QSqlQuery>

Stage::Stage(int hardlevel)
    : hardlevel(hardlevel)
{
    roundnum = hardlevel/2 + 1;
    showtime = INITTIME - (hardlevel - 1)/2 * DECTIME;
    minlength = 1 + (hardlevel - 1)/2;
    maxlength = 4 + (hardlevel - 1)/2;
    init();
}

Stage::~Stage()
{

}

void Stage::init() //初始化本关题目
{

    QSqlQuery q(loginWindow::database());
    if(!q.exec(QString("select * from enwords where length >= %1 and length <= %2 ORDER BY RAND() LIMIT %3").arg(QString::number(minlength)).arg(QString::number(maxlength)).arg(QString::number(roundnum)))){
         //qDebug() << "ERROR!";
    }
    ques.clear();
    while(q.next()){     
        ques.append(q.value("word").toString());
        //qDebug() << "APPEND!";
    }
    return;
}

int Stage::getRoundnum() const
{
    return roundnum;
}

QStringList Stage::getques()
{
    return this->ques;
}

int Stage::getshowtime()
{
    return this->showtime;
}

void Stage::inc_hardlevel()
{
    this->hardlevel++;
    roundnum = hardlevel/2 + 1;
    showtime = INITTIME - (hardlevel - 1)/2 * DECTIME;
    minlength = 1 + (hardlevel - 1)/2;
    maxlength = 4 + (hardlevel - 1)/2;
    init();
}
