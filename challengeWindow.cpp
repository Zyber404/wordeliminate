#include "challengeWindow.h"
#include "searchSort.h"
#include "ui_challengeWindow.h"

#include <QMessageBox>
#include <QPainterPath>
#include <QPalette>
#include <QtMaterialDialog.h>

challengeWindow::challengeWindow(QWidget *parent, QString username )
    : QWidget(parent)
    , ui(new Ui::challengeWindow)
    ,challenger(username)
{
    challenger.updateLocal();
    ui->setupUi(this);
    ui->mainWidget->setMouseTracking(true); // 允许鼠标跟踪
    ui->mainDisplayWidget->setMouseTracking(true);
    setMouseTracking(true);

    QGraphicsOpacityEffect* trans_effect = new QGraphicsOpacityEffect(this);
    trans_effect->setOpacity(0.9); // 设置透明度
    ui->mainWidget->setGraphicsEffect(trans_effect);

    ui->exp->setProgressType(Material::ProgressType::DeterminateProgress);
    ui->exp->setProgressColor(QColor::fromRgb(0,255,127));
    ui->exp->setValue(challenger.getexp());

    ui->timeleft->setProgressType(Material::ProgressType::DeterminateProgress);
    ui->timeleft->setProgressColor(QColor::fromRgb(0,200,255));
    ui->timeleft->setValue(1000);

    ui->label_lv_val->setText(QString::number(challenger.getlevel()));
    ui->label_user_val->setText(challenger.getusername());
    ui->label_stage_val->setText(QString::number(challenger.getcompletedStages()));

    ui->pushButton_start->setBackgroundMode(Qt::OpaqueMode);
    ui->pushButton_start->setOverlayStyle(Material::TintedOverlay);
    ui->pushButton_start->setCornerRadius(7);
    ui->pushButton_start->setOverlayColor(QColor::fromRgb(0,200,255));
    ui->pushButton_start->setBackgroundColor(QColor::fromRgb(10,10,10,100));
    ui->pushButton_start->setHaloVisible(false);

    ui->lineEdit_input->setEnabled(false);

    connect(ui->pushButton_start, &QPushButton::clicked, this, [=]()  // 开始游戏
            {
                gameStart();
                ui->pushButton_start->setVisible(false);
                ui->pushButton_start->setEnabled(false);
                ui->pushButton_search->setVisible(false);
                ui->pushButton_search->setEnabled(false);
            });

    connect(ui->pushButton_search, &QPushButton::clicked, this, [=]()  // 排序查询
            {
                searchSort* sswindow = new searchSort;
                sswindow->setWindowFlag(Qt::FramelessWindowHint);
                sswindow->setAttribute(Qt::WA_TranslucentBackground);
                sswindow->show();
//                ui->pushButton_start->setVisible(false);
//                ui->pushButton_start->setEnabled(false);
//                ui->pushButton_search->setVisible(false);
//                ui->pushButton_search->setEnabled(false);
            });

    ui->pushButton_search->setBackgroundMode(Qt::OpaqueMode);
    ui->pushButton_search->setOverlayStyle(Material::TintedOverlay);
    ui->pushButton_search->setCornerRadius(7);
    ui->pushButton_search->setOverlayColor(QColor::fromRgb(0,200,255));
    ui->pushButton_search->setBackgroundColor(QColor::fromRgb(10,10,10,100));
    ui->pushButton_search->setHaloVisible(false);

    connect(ui->Button_close, &QPushButton::clicked, this, [=](){
        this->close();
    });

}

challengeWindow::~challengeWindow()
{
    delete ui;
    delete this->stage;
    delete this->ques;
}

void challengeWindow::setchallenger(QString username, int level, int completed_stages, int exp)
{
    challenger.setchallenger( username,level, completed_stages, exp);
    ui->label_lv_val->setText(QString::number(level));
    ui->label_user_val->setText(username);
    ui->label_stage_val->setText(QString::number(completed_stages));
}

void challengeWindow::focusline()
{
    ui->lineEdit_input->setFocus();
}

void challengeWindow::lose()
{
    count->stop();
    int ret = QMessageBox::question(this, "You Lose!", "游戏结束，是否重新开始？",
              QMessageBox::Yes|QMessageBox::No,
              QMessageBox::Yes);
    if(ret == QMessageBox::Yes)
    {
        ui->lineEdit_input->clear();
        stage->init();
        *ques = stage->getques();
        it = ques->constBegin();
        ui->label_rounds_val->setText(QString::number(ques->size()));
        if(!ques->isEmpty()) ui->word->setText(*it);
        else return;
        ui->timeleft->setValue(1000);
        ui->timeleft->setProgressColor(QColor::fromRgb(0,200,255));
        t->start(stage->getshowtime()*1/1000);
    }
    else{
        this->close();
    }
}

void challengeWindow::win()
{
    count->stop();
    ui->lineEdit_input->clear();
    int rounds = ui->label_rounds_val->text().toInt() - 1;
    ui->label_rounds_val->setText(QString::number(rounds));
    it++; //进行一次迭代
    use_time+=(1000 - ui->timeleft->value()); //累计关卡用时
    if(it != ques->constEnd()) { //完成一轮
        ui->word->setText(*it);
        ui->timeleft->setValue(1000);
        ui->timeleft->setProgressColor(QColor::fromRgb(0,200,255));
        t->start(stage->getshowtime()*1/1000);
    }
    else{ //完成一关
        this->challenger.incCompletedStages(10+10*((challenger.getcompletedStages()+1)/2 + (1000*stage->getRoundnum() - use_time)/(200*stage->getRoundnum())));
        //保底经验10，每闯过两关保底经验+10，每轮平均剩余时间每多出当前难度一轮时间的20%，经验值再加10
        this->stage->inc_hardlevel();
        ui->exp->setValue(challenger.getexp());
        ui->label_lv_val->setText(QString::number(challenger.getlevel()));
        ui->label_stage_val->setText(QString::number(challenger.getcompletedStages()));
        int ret = QMessageBox::question(this, "You Win!", "恭喜过关，是否继续？",
                                        QMessageBox::Yes|QMessageBox::No,
                                        QMessageBox::Yes);
        if(ret == QMessageBox::Yes){
            stage->init();
            *ques = stage->getques();
            it = ques->constBegin();
            ui->label_rounds_val->setText(QString::number(ques->size()));
            if(!ques->isEmpty()) ui->word->setText(*it);
            else return;
            ui->timeleft->setValue(1000);
            ui->timeleft->setProgressColor(QColor::fromRgb(0,200,255));
            t->start(stage->getshowtime()*1/1000);
        }
        else{
            this->close();
        }
    }
}

void challengeWindow::loginclosed(loginWindow *ptr_loginWindow)
{
    ptr_loginWindow->close();
    delete ptr_loginWindow;
}

void challengeWindow::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Enter) {
        qDebug()<<"ENTER!!";
        gameStart();
        ev->accept();
    }
    else if(ev->key() == Qt::Key_Space) {
        gameStart();
        ev->accept();
    }//search();
    else ev->ignore();
}

void challengeWindow::gameStart()
{
    stage = new Stage(challenger.getcompletedStages()+1);
    distime = 300;
    ques = new QStringList;
    *ques = stage->getques();
    it = ques->constBegin(); //迭代问题
    ui->label_rounds_val->setText(QString::number(ques->size()));
    if(!ques->isEmpty()) ui->word->setText(*it);

    t = new QTimer(this);
    count = new QTimer(this);
    palette = new QPalette;
    connect(t, &QTimer::timeout, this, [=](){
        use_time = 0;

        if(distime == 0){
            distime = 300;
            ui->word->setVisible(false);
            ui->lineEdit_input->setEnabled(true);
            ui->lineEdit_input->setFocus();
            count->start(stage->getshowtime()*1/1000);
            t->stop();
            return;
        }
        else if(distime <= 100){
            *palette = ui->word->palette();

            palette->setColor(QPalette::WindowText, QColor(255,255,255,2*distime));

            ui->word->setPalette(*palette);
            distime--;
            return;
        }
        else if(distime >= 140 && distime <= 175){
            *palette = ui->word->palette();

            palette->setColor(QPalette::WindowText, QColor(255,0,0/*,2*distime*/));

            ui->word->setPalette(*palette);
            distime--;
            return;
        }
        else if(distime >= 260 && distime < 300){
            *palette = ui->word->palette();

            palette->setColor(QPalette::WindowText, QColor(255,0,0/*,2*distime*/));

            ui->word->setPalette(*palette);
            distime--;
            return;
        }
        ui->word->setVisible(true);
        *palette = ui->word->palette();

        palette->setColor(QPalette::WindowText, QColor(255,255,255,200));

        ui->word->setPalette(*palette);
        ui->lineEdit_input->setEnabled(false);
        distime--;
    });

    connect(count, &QTimer::timeout, this, [=](){
        int value = ui->timeleft->value();
        if(value == 200) ui->timeleft->setProgressColor(QColor::fromRgb(255,30,30)); //改变进度条颜色

        //↓↓↓↓↓ 花了大心血实现的倒计时变色！！！！！ ↓↓↓↓↓↓

        else if(200 < value && value <= 350 && value % 2 == 0) ui->timeleft->setProgressColor(QColor::fromRgb(255,
                                                           ui->timeleft->progressColor().green() - 3,
                                                           30));
        else if(value == 500) ui->timeleft->setProgressColor(QColor::fromRgb(255,255,30));
        else if(500 < value && value <= 650) {
            if(value % 2 == 0) ui->timeleft->setProgressColor(QColor::fromRgb(ui->timeleft->progressColor().red() ,
                                                               ui->timeleft->progressColor().green(),
                                                               ui->timeleft->progressColor().blue() - 3));
            if(value % 14 == 0) ui->timeleft->setProgressColor(QColor::fromRgb(ui->timeleft->progressColor().red(),
                                                               ui->timeleft->progressColor().green() + 5,
                                                               ui->timeleft->progressColor().blue()));
            if(value % 3 == 0) ui->timeleft->setProgressColor(QColor::fromRgb(ui->timeleft->progressColor().red() + 5,
                                                               ui->timeleft->progressColor().green(),
                                                               ui->timeleft->progressColor().blue()));
        }
        else if(value == 1000)ui->timeleft->setProgressColor(QColor::fromRgb(0,200,255));

        if (value > 0) ui->timeleft->setValue(value - 1); //进度条动画
        else if(value == 0) {
            this->lose();
            return;
        }

        if(ui->lineEdit_input->text() == ui->word->text()){
            this->win();
        }
    });

    t->start(stage->getshowtime()*1/1000);

}

/* 给centralwidget添加一个mainwidget，设置遮罩及遮挡锯齿边缘的board*/
void challengeWindow::Init()
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
void challengeWindow::mousePressEvent(QMouseEvent *event)
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
void challengeWindow::mouseMoveEvent(QMouseEvent *event)
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
void challengeWindow::resizeEvent(QResizeEvent *event)
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
void challengeWindow::controlWindowScale(){
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

