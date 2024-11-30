//
//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//

#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QPushButton>
#include <QIcon>
#include <QScrollArea>
#include <QStyle>
#include <QSlider>
#include <QComboBox>
#include "the_player.h"
#include "the_button.h"

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

            if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
        }
    }

    return out;
}


QWidget * init_controllWidget(ThePlayer * player){
    //控制容器
    QWidget *controllWidget = new QWidget();
    QHBoxLayout* controll_layout = new QHBoxLayout();
    controllWidget->setLayout(controll_layout);

    // 创建控制按钮
    QPushButton* previousButton = new QPushButton();
    QPushButton* playPauseButton = new QPushButton();
    QPushButton* nextButton = new QPushButton();
    QPushButton* muteButton = new QPushButton();
    muteButton->setFixedSize(100, 50); // 静音按钮设置固定大小为 100x50 像素

    // 创建音量滑块
    QSlider *volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100); // 范围从 0 到 100
    volumeSlider->setValue(0); // 默认音量 0%
    volumeSlider->setMaximumWidth(100);

    //设置控制按钮样式
    playPauseButton ->setIcon(playPauseButton ->style()->standardIcon(QStyle::SP_MediaPause));
    playPauseButton ->setIconSize(QSize(40, 40));
    previousButton->setIcon(previousButton->style()->standardIcon(QStyle::SP_MediaSkipBackward));
    previousButton->setIconSize(QSize(40, 40));
    nextButton ->setIcon(nextButton->style()->standardIcon(QStyle::SP_MediaSkipForward));
    nextButton ->setIconSize(QSize(40, 40));
    muteButton ->setIcon(muteButton->style()->standardIcon(QStyle::SP_MediaVolumeMuted));
    muteButton ->setIconSize(QSize(40, 40));

    // 将按钮添加到布局中
    controll_layout->addWidget(previousButton);
    controll_layout->addWidget(playPauseButton);
    controll_layout->addWidget(nextButton);
    controll_layout->addWidget(muteButton);
    controll_layout->addWidget(volumeSlider);

    // 连接按钮的信号和槽
    previousButton->connect(previousButton, SIGNAL(clicked()), player, SLOT(previousVideo()));
    playPauseButton->connect(playPauseButton, &QPushButton::clicked, [=]() {player->playPause(playPauseButton);});
    nextButton->connect(nextButton, SIGNAL(clicked()), player, SLOT(nextVideo()));
    volumeSlider->connect(volumeSlider, SIGNAL(valueChanged(int)), player, SLOT(setVolume(int)));

    return controllWidget;
}


QWidget * init_progressWidget(ThePlayer * player){
    //进度条容器
    QSlider* progressbar = new QSlider(Qt::Horizontal);//Video progress bar
    QComboBox * speed = new QComboBox;
    QWidget *progressWidget = new QWidget();
    QHBoxLayout* progress_layout = new QHBoxLayout();
    progressWidget->setLayout(progress_layout);
    progress_layout -> addWidget(progressbar);
    progress_layout -> addWidget(speed);
    //绑定进度条动作
    player->connect(player, &QMediaPlayer::durationChanged, progressbar, &QSlider::setMaximum);
    player->connect(player, &QMediaPlayer::positionChanged, progressbar, &QSlider::setValue);
    progressbar->connect(progressbar, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);

    //时速
    speed->addItem("0.5x", QVariant(0.5));
    speed->addItem("1x", QVariant(1));
    speed->addItem("1.5x", QVariant(1.5));
    speed->addItem("2x", QVariant(2));
    speed->addItem("2.5x", QVariant(2.5));
    speed->addItem("3x", QVariant(3));
    speed->setCurrentIndex(1);
    speed->setMaximumWidth(80);
    // 连接信号与槽，直接设置播放速度
    speed->connect(speed, QOverload<int>::of(&QComboBox::currentIndexChanged), player,
            [player, speed] {
                double videospeed = speed->currentData().toDouble(); // 获取当前选中项的播放速度
                player->setPlaybackRate(videospeed); // 设置播放速度
            });
    return progressWidget;
}

QWidget * init_totalWidget(QVideoWidget *videoWidget, QWidget *progressWidget, QWidget *buttonWidget, QWidget *controllWidget){
    //加入进度条 垂直容器
    QWidget *videoAndprogressWidget = new QWidget;
    QVBoxLayout *videoAndprogresslayout = new QVBoxLayout();
    videoAndprogressWidget->setLayout(videoAndprogresslayout);
    videoAndprogresslayout -> addWidget(videoWidget);
    videoAndprogresslayout -> addWidget(progressWidget);

    //加入选项 水平容器
    QWidget *videoAndbuttonWidget = new QWidget;
    QHBoxLayout *videoAndbuttonlayout = new QHBoxLayout();
    videoAndbuttonWidget->setLayout(videoAndbuttonlayout);
    videoAndbuttonlayout -> addWidget(videoAndprogressWidget);
    videoAndbuttonlayout -> addWidget(buttonWidget);

    //加入控制 垂直容器
    QWidget *totalWidget = new QWidget();
    QVBoxLayout *totallayout = new QVBoxLayout();
    totalWidget->setLayout(totallayout);
    totallayout -> addWidget(videoAndbuttonWidget);
    totallayout -> addWidget(controllWidget);
    return totalWidget;
}


int main(int argc, char *argv[]) {

    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn( std::string(argv[1]) );

    if (videos.size() == 0) {

        const int result = QMessageBox::information(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! Add command line argument to \"quoted\" file location."));
        exit(-1);
    }

    //初始化视频容器
    QVideoWidget *videoWidget = new QVideoWidget;
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    //初始化控制容器
    QWidget *controllWidget = init_controllWidget(player);

    //初始化进度条容器
    QWidget *progressWidget = init_progressWidget(player);

    //初始化按钮容器
    QWidget *buttonWidget = new QWidget();
    std::vector<TheButton*> buttons;
    QVBoxLayout *layout = new QVBoxLayout();
    buttonWidget->setLayout(layout);
    for ( int i = 0; i < 4; i++ ) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
        buttons.push_back(button);
        layout->addWidget(button);
        button->init(&videos.at(i));
    }
    player->setContent(&buttons, & videos);

    //组合视频,进度条,按钮,控制容器
    QWidget *totalWidget = init_totalWidget(videoWidget, progressWidget, buttonWidget, controllWidget);

    // create the main window and layout
    QWidget window;
    QVBoxLayout *top = new QVBoxLayout();
    window.setLayout(top);
    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);

    // add the video and the buttons to the top level widget
    top->addWidget(totalWidget);

    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
