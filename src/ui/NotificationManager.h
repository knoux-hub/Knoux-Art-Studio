#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QQueue>
#include <QDateTime>

class GlassPanel;
class QLabel;

// Notification types
enum NotificationType {
    Notification_Info,
    Notification_Success,
    Notification_Warning,
    Notification_Error,
    Notification_AI_Processing
};

// Notification data structure
struct Notification {
    QString id;
    QString title;
    QString message;
    NotificationType type;
    QDateTime timestamp;
    int duration;
    bool persistent;
};

// Individual notification widget
class NotificationWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(int slideOffset READ slideOffset WRITE setSlideOffset)

public:
    explicit NotificationWidget(const Notification &notification, QWidget *parent = nullptr);

    float opacity() const { return m_opacity; }
    void setOpacity(float opacity);

    int slideOffset() const { return m_slideOffset; }
    void setSlideOffset(int offset);

    QString id() const { return m_notification.id; }

signals:
    void dismissed(const QString &id);
    void clicked(const QString &id);

public slots:
    void showAnimated();
    void dismissAnimated();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setupUI();
    QColor getTypeColor() const;
    QString getTypeIcon() const;

    Notification m_notification;
    float m_opacity;
    int m_slideOffset;
    bool m_isHovered;

    QPropertyAnimation *m_opacityAnimation;
    QPropertyAnimation *m_slideAnimation;
    QTimer *m_autoDismissTimer;
};

// Main notification manager
class NotificationManager : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationManager(QWidget *parent = nullptr);

    void showNotification(const QString &title, const QString &message,
                          NotificationType type = Notification_Info,
                          int durationMs = 5000);
    void showNotification(const Notification &notification);

    void dismissNotification(const QString &id);
    void dismissAllNotifications();

    int activeNotificationCount() const;
    bool hasNotifications() const;

signals:
    void notificationShown(const QString &id);
    void notificationDismissed(const QString &id);
    void notificationClicked(const QString &id);
    void allNotificationsDismissed();

public slots:
    void setEnabled(bool enabled);
    void setMaxNotifications(int max);
    void setDefaultDuration(int durationMs);
    void clearHistory();

private slots:
    void onNotificationDismissed(const QString &id);
    void onNotificationClicked(const QString &id);
    void processQueue();
    void repositionNotifications();

private:
    void setupUI();
    void addNotificationWidget(NotificationWidget *widget);
    void removeNotificationWidget(const QString &id);
    NotificationWidget *findWidget(const QString &id);

    QVBoxLayout *m_layout;
    QList<NotificationWidget*> m_activeNotifications;
    QQueue<Notification> m_notificationQueue;
    QList<Notification> m_notificationHistory;

    bool m_enabled;
    int m_maxNotifications;
    int m_defaultDuration;
    int m_notificationSpacing;
    int m_notificationWidth;
};

// Toast notification (simple popup)
class ToastNotification : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

public:
    static void show(QWidget *parent, const QString &message,
                     NotificationType type = Notification_Info,
                     int durationMs = 3000);

    float opacity() const { return m_opacity; }
    void setOpacity(float opacity);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    explicit ToastNotification(QWidget *parent = nullptr);

    void showAnimated();
    void dismissAnimated();

    QString m_message;
    NotificationType m_type;
    float m_opacity;

    QPropertyAnimation *m_animation;
    QTimer *m_timer;
};

#endif // NOTIFICATIONMANAGER_H
