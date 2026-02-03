#include "VideoEditor.h"
#include "../ui/GlassButton.h"
#include <QPainter>
#include <QVBoxLayout>

VideoEditor::VideoEditor(QWidget *parent) : QWidget(parent) {
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);
    setupUI();
}

void VideoEditor::importVideo() {
    QString path = QFileDialog::getOpenFileName(this, "Import Sovereign Media", "F:/KnouxArtStudio/projects/media");
    if(!path.isEmpty()) {
        m_mediaPlayer->setSource(QUrl::fromLocalFile(path));
        emit statusMessage("Media Loaded to F: Cache");
    }
}

void VideoEditor::play() { m_mediaPlayer->play(); }
