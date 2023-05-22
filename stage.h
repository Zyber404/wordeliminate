
#ifndef STAGE_H
#define STAGE_H

#include <QStringList>

#define INITTIME 10000
#define DECTIME 1000


class Stage
{
public:
    Stage(int hardlevel);
    ~Stage();
    QStringList getques();
    int getshowtime();
    void inc_hardlevel();
    void init();

    int getRoundnum() const;

private:
    int hardlevel = 1;
    int roundnum = 1;
    int showtime = 10000;
    int minlength = 1;
    int maxlength = 4;
    QStringList ques;

};

#endif // STAGE_H
