//
//
//

#include "the_player.h"

// all buttons have been setup, store pointers here
void ThePlayer::setContent(std::vector<TheButton*>* b, std::vector<TheButtonInfo>* i) {
    buttons = b;
    infos = i;
    jumpTo(buttons -> at(0) -> info);
}

// change the image and video for one button every one second
void ThePlayer::shuffle() {
    TheButtonInfo* i = & infos -> at (rand() % infos->size() );
//        setMedia(*i->url);
    buttons -> at( updateCount++ % buttons->size() ) -> init( i );
}

//视频结束后重发播放
void ThePlayer::playStateChanged (QMediaPlayer::State ms) {
    switch (ms) {
        case QMediaPlayer::State::StoppedState:
            play(); // starting playing again...
            break;
    default:
        break;
    }
}

void ThePlayer::jumpTo (TheButtonInfo* button) {
    setMedia( * button -> url);
    play();
}

void ThePlayer::playPause(QPushButton* playPauseButton) {
    if (isPaused) {
        play();  // 如果当前是暂停状态，继续播放
        isPaused = false;
        playPauseButton ->setIcon(playPauseButton ->style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        pause();  // 如果当前正在播放，暂停
        isPaused = true;
        playPauseButton ->setIcon(playPauseButton ->style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void ThePlayer::nextVideo() {
    updateCount = (updateCount + 1) % infos->size();  // 循环到下一个视频
    jumpTo(&infos->at(updateCount));
}

void ThePlayer::previousVideo() {
    updateCount = (updateCount - 1 + infos->size()) % infos->size();  // 循环到上一个视频
    jumpTo(&infos->at(updateCount));
}
