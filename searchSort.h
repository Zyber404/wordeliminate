#ifndef _SEARCHSORT_H
#define _SEARCHSORT_H

#include <QMainWindow>
#include <QTimer>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QStandardItemModel>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class framelessWidget; }
QT_END_NAMESPACE

class searchSort : public QWidget
{
    Q_OBJECT

public:
    searchSort(QWidget *parent = nullptr);
    ~searchSort();

private:
    Ui::framelessWidget *ui;
    int cornerRadius = 15;
    QPoint lastPos;
    QWidget *border = nullptr;
    QGraphicsDropShadowEffect *windowShadow;

    bool mousePressed = false;
    enum {AT_LEFT = 1, AT_TOP = 2,  AT_RIGHT = 4, AT_BOTTOM = 8,
          AT_TOP_LEFT = 3, AT_TOP_RIGHT = 6, AT_BOTTOM_LEFT = 9, AT_BOTTOM_RIGHT = 12};
    int mouseState;
    bool maximized = false;

    QStandardItemModel* modelC;
    QStandardItemModel* modelQ;
    QStandardItemModel* modelS;
    QSqlQuery* q;

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
#endif // _SEARCHSORT_H
