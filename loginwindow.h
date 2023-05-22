#ifndef _LOGINWINDOW_H
#define _LOGINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QSqlDatabase>
#include <qtmaterialsnackbar.h>

QT_BEGIN_NAMESPACE
namespace Ui { class loginWindow; }
QT_END_NAMESPACE


class loginWindow : public QWidget
{
    Q_OBJECT
//    signals:
//    void closed(loginWindow* ptr_loginWindow); //由后续界面对该界面的对象析构

public:

    loginWindow(QWidget *parent = nullptr);
    ~loginWindow();

    static  QSqlDatabase database();
    Ui::loginWindow *getUi() const;

private:

    QGraphicsOpacityEffect* trans_effect;
    Ui::loginWindow *ui;
    int cornerRadius = 15;
    QPoint lastPos;
    QWidget *border = nullptr;
    QGraphicsDropShadowEffect *windowShadow;
    QtMaterialSnackbar bar;
    bool mousePressed = false;
    enum {AT_LEFT = 1, AT_TOP = 2,  AT_RIGHT = 4, AT_BOTTOM = 8,
          AT_TOP_LEFT = 3, AT_TOP_RIGHT = 6, AT_BOTTOM_LEFT = 9, AT_BOTTOM_RIGHT = 12};
    int mouseState;
    bool maximized = false;

    bool isrepeat(QString username);
    int regist(QString username, QString password);
    int login(QString username, QString password);
    QString crypto(QString username, QString password);

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
#endif // _LOGINWINDOW_H
