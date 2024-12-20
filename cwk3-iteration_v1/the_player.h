//
//
//

#ifndef CW2_THE_PLAYER_H
#define CW2_THE_PLAYER_H


#include <QApplication>
#include <QMediaPlayer>
#include "the_button.h"
#include <vector>
#include <QStyle>
#include <QPushButton>
#include <QTimer>
#include <QIcon>
#include <QStyle>


class ThePlayer : public QMediaPlayer {

    Q_OBJECT

    private:
        std::vector<TheButtonInfo>* infos;
        std::vector<TheButton*>* buttons;
        QTimer* mTimer;
        long updateCount = 0;
        bool isPaused = false;  // 用来跟踪当前播放状态

    public:
        ThePlayer() : QMediaPlayer(NULL) {
            setVolume(0); // be slightly less annoying
            connect (this, SIGNAL (stateChanged(QMediaPlayer::State)), this, SLOT (playStateChanged(QMediaPlayer::State)) );

            mTimer = new QTimer(NULL);
            mTimer->setInterval(10000); // 1000ms is one second between ...
            mTimer->start();
            connect( mTimer, SIGNAL (timeout()), SLOT ( shuffle() ) ); // ...running shuffle method
        }

        // all buttons have been setup, store pointers here
        void setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i);

    private slots:

        // change the image and video for one button every one second
        void shuffle();

        void playStateChanged (QMediaPlayer::State ms);

    public slots:

        // start playing this ButtonInfo
        void jumpTo (TheButtonInfo* button);
        void playPause(QPushButton* playPauseButton);        // 暂停/播放切换
        void nextVideo();        // 播放下一个视频
        void previousVideo();    // 播放上一个视频
};

#endif //CW2_THE_PLAYER_H
