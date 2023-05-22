#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "challengeWindow.h"
#include "questionWindow.h"
#include "ui_questionWindow.h"
#include <Windows.h>
#include <QPainterPath>
#include <QWindow>
#include <QFont>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

static QSqlDatabase db; //数据库由登录界面初始化，并只能通过登录界面的静态方法获得

loginWindow::loginWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::loginWindow)
{
    ui->setupUi(this);

//    ui->mainWidget->setMouseTracking(true); // 允许鼠标跟踪
//    ui->mainDisplayWidget->setMouseTracking(true);
//    setMouseTracking(true);

    QTimer *t = new QTimer(this);
    connect(t, &QTimer::timeout, this, [=](){Init();});
    t->setSingleShot(true);
    t->start(10);

    trans_effect = new QGraphicsOpacityEffect(this);
    trans_effect->setOpacity(0.9); // 设置透明度
    ui->mainWidget->setGraphicsEffect(trans_effect);

//    ui->background->setWindowOpacity(0.5);

//    QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
//    blurEffect->setBlurRadius(2.5); // 设置模糊半径
//    ui->background->setGraphicsEffect(blurEffect);

    ui->pushButton_login->setBackgroundMode(Qt::OpaqueMode);
    ui->pushButton_login->setOverlayStyle(Material::TintedOverlay);
    ui->pushButton_login->setCornerRadius(7);
    ui->pushButton_login->setOverlayColor(QColor::fromRgb(0,200,255));
    ui->pushButton_login->setBackgroundColor(QColor::fromRgb(10,10,10,100));
    ui->pushButton_login->setHaloVisible(false);

    ui->pushButton_regist->setBackgroundMode(Qt::OpaqueMode);
    ui->pushButton_regist->setOverlayStyle(Material::TintedOverlay);
    ui->pushButton_regist->setCornerRadius(7);
    ui->pushButton_regist->setOverlayColor(QColor::fromRgb(0,200,255));
    ui->pushButton_regist->setBackgroundColor(QColor::fromRgb(10,10,10,100));
    ui->pushButton_regist->setHaloVisible(false);

    ui->radioButton_challenger->setTextColor(QColor::fromRgb(255,255,255));
    ui->radioButton_challenger->setUncheckedColor(QColor::fromRgb(255,255,255));
    ui->radioButton_questioner->setTextColor(QColor::fromRgb(255,255,255));
    ui->radioButton_questioner->setUncheckedColor(QColor::fromRgb(255,255,255));

    QStringList* local_username = new QStringList(QString("ZYBER")); //自动补全功能
    local_username->append(QString("123"));
    local_username->append(QString("admin"));
    ui->lineEdit_username->setDataSource(*local_username);

    bar.setParent(this);
    bar.setAutoHideDuration(400);
    bar.setBoxWidth(120);
    bar.setFont( QFont("Microsoft YaHei UI"));
    bar.setFontSize(10);
    bar.setBackgroundColor(QColor::fromRgb(30,150,30,200));

    connect(ui->Button_close, &QPushButton::clicked, this, [=](){
        delete local_username;
        this->close();
    });

    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("root");
    db.setDatabaseName("wordlibrary");
    if(!db.open())
    {
        bar.setBackgroundColor(QColor::fromRgb(150,30,30,200));
        bar.addMessage("数据库未连接");
        return;
    }

    connect(ui->pushButton_login, &QPushButton::clicked, this, [=](){
        if(!(ui->radioButton_challenger->isChecked() || ui->radioButton_questioner->isChecked())){
            bar.setBackgroundColor(QColor::fromRgb(150,150,30,200));
            bar.addMessage(" 请选择身份！");
            return;
        }
        if(ui->lineEdit_username->text().isEmpty() || ui->lineEdit_password->text().isEmpty()){
            bar.setBackgroundColor(QColor::fromRgb(150,150,30,200));
            bar.addMessage("  输入为空！");
            return;
        }
        if(!isrepeat(ui->lineEdit_username->text())){
            bar.setBackgroundColor(QColor::fromRgb(150,150,30,200));
            bar.addMessage(" 用户不存在！");
            return;
        }
        else{
            if(login(ui->lineEdit_username->text(), ui->lineEdit_password->text())){
                bar.setBackgroundColor(QColor::fromRgb(150,30,30,200));
                bar.addMessage("  登录失败！");
                return;
            }
            else{
                bar.setBackgroundColor(QColor::fromRgb(30,150,30,200));
                bar.addMessage("  登录成功！");
            }
        }
        if(ui->radioButton_challenger->isChecked()){
            challengeWindow* w = new challengeWindow(nullptr, ui->lineEdit_username->text());
            w->setWindowFlag(Qt::FramelessWindowHint);
            w->setAttribute(Qt::WA_TranslucentBackground);
            //QSqlQuery q;
            //q.exec(QString("select * from challenger where username = %1").arg(ui->lineEdit_username->text()));
            //q.next();
            //w->setchallenger(ui->lineEdit_username->text(), q.value("level").toInt(), q.value("completedStages").toInt(), q.value("exp").toInt());
            w->show();
            //w->focusline();
            w->loginclosed(this);
            delete local_username;
        }
        else{
            questionWindow* w = new questionWindow(nullptr, ui->lineEdit_username->text());
            w->setWindowFlag(Qt::FramelessWindowHint);
            w->setAttribute(Qt::WA_TranslucentBackground);
//            QSqlQuery q;
//            q.exec(QString("select * from challenger where username = %1").arg(ui->lineEdit_username->text()));
//            q.next();
//            w->setchallenger(ui->lineEdit_username->text(), q.value("level").toInt(), q.value("completedStages").toInt(), q.value("exp").toInt());
            w->show();
          //  w->focusline();
            w->getUi()->lineEdit_input->setFocus();
            w->loginclosed(this);
            delete local_username;
        }
    });

    connect(ui->pushButton_regist, &QPushButton::clicked, this, [=](){
        if(!(ui->radioButton_challenger->isChecked() || ui->radioButton_questioner->isChecked())){
            bar.setBackgroundColor(QColor::fromRgb(150,150,30,200));
            bar.addMessage(" 请选择身份！");
            return;
        }
        if(ui->lineEdit_username->text().isEmpty() || ui->lineEdit_password->text().isEmpty()){
            bar.setBackgroundColor(QColor::fromRgb(150,150,30,200));
            bar.addMessage("  输入为空！");
            return;
        }
        if(isrepeat(ui->lineEdit_username->text())){
            bar.setBackgroundColor(QColor::fromRgb(150,150,30,200));
            bar.addMessage("  重复注册！");
        }
        else{
            if(regist(ui->lineEdit_username->text(), ui->lineEdit_password->text())){
                bar.setBackgroundColor(QColor::fromRgb(150,30,30,200));
                bar.addMessage("  注册失败！");
            }
            else{
                bar.setBackgroundColor(QColor::fromRgb(30,150,30,200));
                bar.addMessage("  注册成功！");
                local_username->append(ui->lineEdit_username->text());
                ui->lineEdit_username->setDataSource(*local_username);
            }
        }
    });
}

loginWindow::~loginWindow()
{
    delete ui;
}

QSqlDatabase loginWindow::database()
{
    return db;
}

Ui::loginWindow *loginWindow::getUi() const
{
    return ui;
}

bool loginWindow::isrepeat(QString username)
{
    QString type;
    if(ui->radioButton_challenger->isChecked()){
        type = "challenger";
    }
    else type = "questioner";
    QSqlQuery q;
    q.exec(QString("select * from %1 where username = '%2'").arg(type).arg(username));
    if(q.next()) return true;
    return false;
}

int loginWindow::regist(QString username, QString password)
{
    QString type;
    QString encrypted = crypto(username, password);
    if(ui->radioButton_challenger->isChecked()){
        type = "challenger";
    }
    else type = "questioner";
    QSqlQuery q;
    if(q.exec(QString("insert into %1(username , password ) values ('%2', '%3' )").arg(type).arg(username).arg(encrypted))){
        return 0;
    }
    return -1;
}

int loginWindow::login(QString username, QString password)
{
    QString type;
    QString encrypted = crypto(username, password);
    if(ui->radioButton_challenger->isChecked()){
        type = "challenger";
    }
    else type = "questioner";
    QSqlQuery q;
    q.exec(QString("select password from %1 where username = '%2'").arg(type).arg(username));
    q.next();
    if(encrypted == q.value("password").toString()){
        return 0;
    }
    return -1;
}

QString loginWindow::crypto(QString username, QString password)
{
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sha256();

    EVP_DigestInit_ex(mdctx, md, nullptr);
    EVP_DigestUpdate(mdctx, username.toStdString().c_str(), username.size());

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;

    EVP_DigestFinal_ex(mdctx, hash, &hashLen); //将用户名进行sha256加密后的值作为该用户的盐值
    EVP_MD_CTX_free(mdctx);


    int iterations = 10000;
    int key_len = 32;

    unsigned char out_key[key_len+1];

    PKCS5_PBKDF2_HMAC(password.toStdString().c_str(), password.size(), hash, hashLen, iterations, EVP_sha256(), key_len, out_key);

    out_key[key_len] = 0;

    QString ret = QString::fromUtf8(reinterpret_cast<char*>(out_key), key_len+1);

    return ret;
}

/* 给centralwidget添加一个mainwidget，设置遮罩及遮挡锯齿边缘的board*/
void loginWindow::Init()
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
void loginWindow::mousePressEvent(QMouseEvent *event)
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
void loginWindow::mouseMoveEvent(QMouseEvent *event)
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
void loginWindow::resizeEvent(QResizeEvent *event)
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
void loginWindow::controlWindowScale(){
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

