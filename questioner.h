
#ifndef QUESTIONER_H
#define QUESTIONER_H

#include "user.h"



#define DEFAULT_QUES_TO_UP 3

class Questioner: public User
{
public:

    Questioner(QString username, int level = 1);
    virtual ~Questioner();
    void incQuestions();
    int questioning(QString word);
    void updateLocal();
    void updateDatabase();
    int getQuestions() const;
    int getLevel() const;
    int getQuestions_to_levelup() const;
    QString getUsername() const;

private:
    int questions = 0;
    int questions_to_levelup = DEFAULT_QUES_TO_UP;
};

#endif // QUESTIONER_H
