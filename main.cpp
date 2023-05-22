
#include "loginwindow.h"
#include "challengeWindow.h"
#include <QApplication>
#include <QStandardItemModel>
#include <QTableView>
#include <ui_searchSort.h>
#include "searchSort.h"
#include "ui_loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication::setStyle("fusion");
    QApplication a(argc, argv);

    loginWindow* loginW = new loginWindow;
    loginW->setWindowFlag(Qt::FramelessWindowHint);
    loginW->setAttribute(Qt::WA_TranslucentBackground);
    loginW->setFixedSize(600,700);
    loginW->show();
    loginW->getUi()->lineEdit_username->setFocus();

//    searchSort* w = new searchSort;
//    w->setWindowFlag(Qt::FramelessWindowHint);
//    w->setAttribute(Qt::WA_TranslucentBackground);
//    w->show();
    return a.exec();
}
