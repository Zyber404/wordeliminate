#ifndef _CHALLENGEWINDOW_H
#define _CHALLENGEWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QKeyEvent>
#include "stage.h"
#include "loginwindow.h"
#include "challenger.h"

QT_BEGIN_NAMESPACE
namespace Ui { class challengeWindow; }
QT_END_NAMESPACE

class challengeWindow : public QWidget
{
    Q_OBJECT

public:
    challengeWindow(QWidget *parent = nullptr, QString username = "ZYBER");
    ~challengeWindow();
    void setchallenger(QString username, int level, int completed_stages, int exp);
    void focusline();
    void lose();
    void win();

public slots:
    void loginclosed(loginWindow* ptr_loginWindow);
protected:
    void keyPressEvent(QKeyEvent *ev);

private:
    Ui::challengeWindow *ui;
    int cornerRadius = 15;
    QPoint lastPos;
    QWidget *border = nullptr;
    QGraphicsDropShadowEffect *windowShadow;

    Challenger challenger;
    Stage* stage;
    QStringList* ques;
    QTimer* t;
    QTimer* count;
    QPalette* palette;
    int distime;
    QStringList::const_iterator it;
    int use_time = 0;

    bool mousePressed = false;
    enum {AT_LEFT = 1, AT_TOP = 2,  AT_RIGHT = 4, AT_BOTTOM = 8,
          AT_TOP_LEFT = 3, AT_TOP_RIGHT = 6, AT_BOTTOM_LEFT = 9, AT_BOTTOM_RIGHT = 12};
    int mouseState;
    bool maximized = false;

    void gameStart();
    void Init();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event){
        mousePressed = false;
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        if(event->globalPosition().y() < 2)
            controlWindowScale();
#else
        if(event->globalPos().y() < 2)
            controlWindowScale();
#endif
    }
    void mouseDoubleClickEvent(QMouseEvent *event)
    {
        if(event->y()<60)
            controlWindowScale();
    };
    void resizeEvent(QResizeEvent *event);

    QRect lastGeometry;
    void controlWindowScale();
};
#endif // _CHALLENGEWINDOW_H
