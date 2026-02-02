#ifndef HOMESCREEN_H
#define HOMESCREEN_H

#include <QWidget>
#include <QPropertyAnimation>

class GlassButton;
class GlassCard;
class RecentProjectCard;
class QuickActionCard;

class HomeScreen : public QWidget
{
    Q_OBJECT

public:
    explicit HomeScreen(QWidget *parent = nullptr);

signals:
    void openPhotoEditor();
    void openVideoEditor();
    void openAIStudio();
    void openBodyEditor();
    void openFaceRetouch();
    void openMakeupStudio();
    void openProject(const QString &path);

private slots:
    void onNewPhotoProject();
    void onNewVideoProject();
    void onOpenExistingProject();
    void onOpenAIStudio();
    void onRecentProjectClicked(const QString &path);
    void onTemplateClicked(int templateId);
    void animateEntry();

private:
    void setupUI();
    void createHeader();
    void createQuickActions();
    void createRecentProjects();
    void createTemplates();
    void loadRecentProjects();

    // Header
    QLabel *m_welcomeLabel;
    QLabel *m_dateLabel;

    // Quick actions
    QuickActionCard *m_photoCard;
    QuickActionCard *m_videoCard;
    QuickActionCard *m_aiCard;
    QuickActionCard *m_bodyCard;
    QuickActionCard *m_faceCard;
    QuickActionCard *m_makeupCard;
    QuickActionCard *m_batchCard;

    // Recent projects
    QWidget *m_recentProjectsContainer;
    QVector<RecentProjectCard*> m_recentCards;

    // Templates
    QWidget *m_templatesContainer;

    // Animations
    QPropertyAnimation *m_entryAnimation;
};

// Quick action card widget
class QuickActionCard : public QWidget
{
    Q_OBJECT

public:
    QuickActionCard(const QString &icon, const QString &title, 
                    const QString &subtitle, const QColor &accentColor,
                    QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_icon;
    QString m_title;
    QString m_subtitle;
    QColor m_accentColor;
    bool m_isHovered;
    float m_glowIntensity;
};

// Recent project card
class RecentProjectCard : public QWidget
{
    Q_OBJECT

public:
    RecentProjectCard(const QString &path, const QPixmap &thumbnail,
                      const QString &name, const QDateTime &date,
                      QWidget *parent = nullptr);

signals:
    void clicked(const QString &path);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QString m_path;
    QPixmap m_thumbnail;
    QString m_name;
    QDateTime m_date;
};

#endif // HOMESCREEN_H
