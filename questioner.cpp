
#include "questioner.h"

#include <QSqlError>
#include <QSqlQuery>
#include "loginwindow.h"

Questioner::Questioner(QString username, int level)
    :User(username, level)
{

}

Questioner::~Questioner()
{

}

void Questioner::incQuestions()
{
    questions++;
    questions_to_levelup--;
    if(questions_to_levelup == 0){
        questions_to_levelup = DEFAULT_QUES_TO_UP;
        level++;
    }
    return;
}

int Questioner::questioning(QString word)
{
    QSqlQuery query(loginWindow::database());
    query.exec(QString("SELECT * FROM enwords WHERE word = '%1'").arg(word));
    if (query.next()){
        return 1;
    }

    query.prepare("INSERT INTO enwords (word, length) VALUES (:word, :length)");
    query.bindValue(":word", word);
    query.bindValue(":length", word.length());

    if (query.exec()) {
        return 0;
    } else {
        return -1;
    }
}

void Questioner::updateLocal()
{
    QSqlQuery q(loginWindow::database());
    q.exec(QString("SELECT * FROM questioner WHERE username = '%1'").arg(username));
    q.next();
    level = q.value("level").toInt();
    questions = q.value("questions").toInt();
    questions_to_levelup = q.value("questions_to_levelup").toInt();
}

void Questioner::updateDatabase()
{
    QSqlQuery q(loginWindow::database());
    q.exec(QString("UPDATE questioner SET level=%1, questions=%2, questions_to_levelup=%3 WHERE username = '%4'").arg(QString::number(level)).arg(QString::number(questions)).arg(QString::number(questions_to_levelup)).arg(username));
}

int Questioner::getQuestions() const
{
    return questions;
}

int Questioner::getLevel() const
{
    return level;
}

QString Questioner::getUsername() const
{
    return username;
}

int Questioner::getQuestions_to_levelup() const
{
    return questions_to_levelup;
}
