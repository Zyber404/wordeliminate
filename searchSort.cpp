#include "searchSort.h"
#include "loginwindow.h"
#include "ui_searchSort.h"
#include <QStandardItemModel>
#include <QPainterPath>

searchSort::searchSort(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::framelessWidget)
{


    ui->setupUi(this);
    ui->mainWidget->setMouseTracking(true); // 允许鼠标跟踪
    ui->mainDisplayWidget->setMouseTracking(true);
    setMouseTracking(true);

    QGraphicsOpacityEffect* trans_effect = new QGraphicsOpacityEffect(this);
    trans_effect->setOpacity(0.9); // 设置透明度
    ui->mainWidget->setGraphicsEffect(trans_effect);

    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [=](){Init();});
    t->setSingleShot(true);
    t->start(10);

    q = new QSqlQuery(loginWindow::database());

    modelC = new QStandardItemModel;
    modelC->setHorizontalHeaderItem(0, new QStandardItem("用户名"));
    modelC->setHorizontalHeaderItem(1, new QStandardItem("闯过关卡数"));
    modelC->setHorizontalHeaderItem(2, new QStandardItem("等级"));
    modelC->setHorizontalHeaderItem(3, new QStandardItem("经验"));
    q->exec(QString("select * from challenger"));
    int row = 0;
    while (q->next()) {
        QString username = q->value("username").toString();
        int completed_stages = q->value("completedStages").toInt();
        int exp = q->value("exp").toInt();
        int level = q->value("level").toInt();

        modelC->setItem(row, 0, new QStandardItem(username));
        modelC->setItem(row, 1, new QStandardItem(QString::number(completed_stages)));
        modelC->setItem(row, 2, new QStandardItem(QString::number(level)));
        modelC->setItem(row, 3, new QStandardItem(QString::number(exp)));
        row++;
    }

    modelQ = new QStandardItemModel;
    modelQ->setHorizontalHeaderItem(0, new QStandardItem("用户名"));
    modelQ->setHorizontalHeaderItem(1, new QStandardItem("出题数"));
    modelQ->setHorizontalHeaderItem(2, new QStandardItem("等级"));
    q->exec(QString("select * from questioner"));
    row = 0;
    while (q->next()) {
        QString username = q->value("username").toString();
        int questions = q->value("questions").toInt();
        int level = q->value("level").toInt();

        modelQ->setItem(row, 0, new QStandardItem(username));
        modelQ->setItem(row, 1, new QStandardItem(QString::number(questions)));
        modelQ->setItem(row, 2, new QStandardItem(QString::number(level)));
        row++;
    }

    modelS = new QStandardItemModel;

    connect(ui->Button_close,&QPushButton::clicked, this,[=]()  // 点击关闭按钮
    {
        this->close();
    });

    connect(ui->rankC,&QPushButton::clicked, this,[=]()
            {
                ui->tableView_rank->setModel(modelC);
                ui->tableView_rank->setSortingEnabled(true);
            });

    connect(ui->rankQ,&QPushButton::clicked, this,[=]()
            {
                ui->tableView_rank->setModel(modelQ);
                ui->tableView_rank->setSortingEnabled(true);
            });

    connect(ui->search,&QPushButton::clicked, this,[=]()
            {
                modelS->clear();
                QString name;
                int lv;
                if(ui->comboBox->currentText() == "闯关者"){
                    if(ui->lineEdit_username->text().isEmpty()) name = "like '%'";
                    else name = QString("= '%1'").arg(ui->lineEdit_username->text());
                    lv = ui->spinBox->value();
                    modelS->setHorizontalHeaderItem(0, new QStandardItem("用户名"));
                    modelS->setHorizontalHeaderItem(1, new QStandardItem("闯过关卡数"));
                    modelS->setHorizontalHeaderItem(2, new QStandardItem("等级"));
                    modelS->setHorizontalHeaderItem(3, new QStandardItem("经验"));
                    q->exec(QString("select * from challenger where username %1 and level = %2").arg(name).arg(QString::number(lv)));
                    int row = 0;
                    while (q->next()) {
                        QString username = q->value("username").toString();
                        int completed_stages = q->value("completedStages").toInt();
                        int exp = q->value("exp").toInt();
                        int level = q->value("level").toInt();

                        modelS->setItem(row, 0, new QStandardItem(username));
                        modelS->setItem(row, 1, new QStandardItem(QString::number(completed_stages)));
                        modelS->setItem(row, 2, new QStandardItem(QString::number(level)));
                        modelS->setItem(row, 3, new QStandardItem(QString::number(exp)));
                        row++;
                    }
                }
                else{
                    if(ui->lineEdit_username->text().isEmpty()) name = "like '%'";
                    else name = QString("= '%1'").arg(ui->lineEdit_username->text());
                    lv = ui->spinBox->value();
                    modelS->setHorizontalHeaderItem(0, new QStandardItem("用户名"));
                    modelS->setHorizontalHeaderItem(1, new QStandardItem("出题数"));
                    modelS->setHorizontalHeaderItem(2, new QStandardItem("等级"));
                    q->exec(QString("select * from questioner where username %1 and level = %2").arg(name).arg(QString::number(lv)));
                    int row = 0;
                    while (q->next()) {
                        QString username = q->value("username").toString();
                        int questions = q->value("questions").toInt();
                        int level = q->value("level").toInt();

                        modelS->setItem(row, 0, new QStandardItem(username));
                        modelS->setItem(row, 1, new QStandardItem(QString::number(questions)));
                        modelS->setItem(row, 2, new QStandardItem(QString::number(level)));
                        row++;
                    }
                }
                ui->tableView_search->setModel(modelS);
                ui->tableView_search->setSortingEnabled(true);
            });
}

searchSort::~searchSort()
{
    delete ui;
}

/* 给centralwidget添加一个mainwidget，设置遮罩及遮挡锯齿边缘的board*/
void searchSort::Init()
{
// 获取遮罩的轮廓并给mainWidget设置遮罩
// 遮罩的作用是重叠的部分才可以看到
#ifdef Q_OS_LINUX
    QPainterPath path;
    path.addRect(ui->mainWidget->rect());
#else
    QPainterPath path;
    path.addRoundedRect(ui->mainWidget->rect(), cornerRadius-1 , cornerRadius-1 );
#endif
    QRegion mask(path.toFillPolygon().toPolygon());
    ui->mainWidget->setMask(mask);
    // 给mainwidget添加样式
    QString mainStyle;
    ui->mainWidget->setObjectName("mainWidget");
    mainStyle = "QWidget#mainWidget{background-color:rgb(0, 0, 0)"  + QString::asprintf(";border-radius:%dpx", cornerRadius) + "}";
    ui->mainWidget->setStyleSheet(mainStyle);
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
#ifdef Q_OS_WINDOWS
    windowShadow = new QGraphicsDropShadowEffect(this);
    windowShadow->setBlurRadius(30);
    windowShadow->setColor(QColor(0, 0, 0));
    windowShadow->setOffset(0, 0);
    ui->mainWidget->setGraphicsEffect(windowShadow);
#endif
#endif
    /**********************************************************/

    /* 添加border的 widget以遮盖锯齿边缘 */
#ifdef Q_OS_WINDOWS
    border = new QWidget(this);
    border->move(ui->mainWidget->pos() - QPoint(1, 1));
    border->resize(ui->mainWidget->size() + QSize(2, 2));
    QString borderStyle;
    borderStyle = "background-color:#00FFFFFF;border:1.5px solid #686868; border-radius:" + QString::asprintf("%d",cornerRadius) + "px";
    border->setStyleSheet(borderStyle);
    border->setAttribute(Qt::WA_TransparentForMouseEvents);
    border->show();
#endif
    /*****************************************************************/



}

// 鼠标按下事件(记录拉伸窗口或移动窗口时的起始坐标（左上角）)
void searchSort::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        mousePressed = true;
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        lastPos = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
#else
        lastPos = event->globalPos() - this->frameGeometry().topLeft();
#endif
    }
}

// 鼠标移动事件
void searchSort::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::NoButton)
        mousePressed = false;
    if(!mousePressed){ // 鼠标没按下移动，更新鼠标位置状态
        mouseState = 0;
        if(!maximized && abs(event->pos().x() - ui->mainWidget->pos().x()) < 5)
            mouseState |= AT_LEFT;
        if(!maximized && abs(event->pos().y() - ui->mainWidget->pos().y()) < 5)
            mouseState |= AT_TOP;
        if(!maximized && abs(event->pos().x() - ui->mainWidget->pos().x() - ui->mainWidget->width()) < 5)
            mouseState |= AT_RIGHT;
        if(!maximized && abs(event->pos().y() - ui->mainWidget->pos().y() - ui->mainWidget->height()) < 5)
            mouseState |= AT_BOTTOM;
        if(mouseState == AT_TOP_LEFT  || mouseState == AT_BOTTOM_RIGHT)
            setCursor(Qt::SizeFDiagCursor);
        else if(mouseState == AT_TOP_RIGHT || mouseState == AT_BOTTOM_LEFT)
            setCursor(Qt::SizeBDiagCursor);
        else if(mouseState & (AT_LEFT | AT_RIGHT))
            setCursor(Qt::SizeHorCursor);
        else if(mouseState & (AT_TOP | AT_BOTTOM))
            setCursor(Qt::SizeVerCursor);
        else
            unsetCursor();
    }
    else{ //鼠标按下移动
        if(mouseState == 0){
            if(maximized){
                qreal wRatio = (double)event->pos().x() / (double)ui->mainWidget->width();
                controlWindowScale();
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
                this->move(QPoint(event->globalPosition().x() - ui->mainWidget->width() * wRatio, -30));
#else
                this->move(QPoint(event->globalPos().x() - ui->mainWidget->width() * wRatio, -30));
#endif
                lastPos = QPoint(ui->mainWidget->width() * wRatio, event->pos().y());
            }
            else
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
                this->move(event->globalPosition().toPoint() - lastPos);
#else
                this->move(event->globalPos() - lastPos);
#endif
        }
        else{
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
            QPoint d = event->globalPosition().toPoint() - frameGeometry().topLeft() - lastPos;
#else
            QPoint d = event->globalPos() - frameGeometry().topLeft() - lastPos;
#endif
            if(mouseState & AT_LEFT){
                this->move(this->frameGeometry().x() + d.x(), this->frameGeometry().y());
                this->resize(this->width() - d.x(), this->height());
            }
            if(mouseState & AT_RIGHT){
                this->resize(this->width() + d.x(), this->height());
            }
            if(mouseState & AT_TOP){
                this->move(this->frameGeometry().x(), this->frameGeometry().y() + d.y());
                this->resize(this->width(), this->height() - d.y());
            }
            if(mouseState & AT_BOTTOM){
                this->resize(this->width(), this->height() + d.y());
            }
        }
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
        lastPos = event->globalPosition().toPoint() - this->frameGeometry().topLeft();
#else
        lastPos = event->globalPos() - this->frameGeometry().topLeft();
#endif
    }
}

// 窗口变化事件
void searchSort::resizeEvent(QResizeEvent *event)
{
    //Resize border
    if(border)
        border->resize(ui->mainWidget->size() + QSize(2, 2));

    //Resize mask
    QPainterPath path;
#ifdef Q_OS_WINDOWS
    path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
#else
    path.addRect(ui->mainWidget->rect());
#endif
    QRegion mask(path.toFillPolygon().toPolygon());
    ui->mainWidget->setMask(mask);

    //Resize all pages
//    for(int i = 0; i < pageList.size(); i++){
//        pageList[i]->resize(ui->mainWidget->width() * 0.4 < pageList[i]->preferWidth ? pageList[i]->preferWidth - 1 : ui->mainWidget->width() * 0.4 - 1, ui->mainWidget->height());
//        pageList[i]->resize(pageList[i]->width() + 1, pageList[i]->height());
//    }
}

//点击最大化按钮事件
void searchSort::controlWindowScale(){
#ifdef Q_OS_WINDOWS
    if(!maximized){
        lastGeometry = this->frameGeometry();
      //  windowShadow->setEnabled(false);
        ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
        border->hide();
        QString mainStyle = "QWidget#mainWidget{background-color:rgb(251,251,251) border-radius:0px;}";
        ui->mainWidget->setStyleSheet(mainStyle);
        this->showMaximized();
        maximized = true;
        QPainterPath path;
        path.addRect(ui->mainWidget->rect());
        QRegion mask(path.toFillPolygon().toPolygon());
        ui->mainWidget->setMask(mask);
    }
    else{
        ui->verticalLayout->setContentsMargins(30, 30, 30, 30);
        this->showNormal();
        QString mainStyle = "QWidget#mainWidget{background-color:rgb(251,251,251)" + QString::asprintf(";border-radius:%dpx", cornerRadius) + "}";
        ui->mainWidget->setStyleSheet(mainStyle);
        QPainterPath path;
        path.addRoundedRect(ui->mainWidget->rect(), cornerRadius - 1, cornerRadius - 1);
        QRegion mask(path.toFillPolygon().toPolygon());
        ui->mainWidget->setMask(mask);
        border->show();
     //   windowShadow->setEnabled(true);
        this->resize(lastGeometry.width(), lastGeometry.height());
        this->move(lastGeometry.x(), lastGeometry.y());
        maximized = false;
    }
#endif
}

