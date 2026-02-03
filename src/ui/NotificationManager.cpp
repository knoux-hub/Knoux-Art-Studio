#include "NotificationManager.h"
#include "GlassPanel.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

// Cyberpunk colors
#define CP_BG_DARK      QColor(10, 10, 25)
#define CP_ACCENT_CYAN  QColor(0, 255, 255)
#define CP_ACCENT_PINK  QColor(255, 0, 128)
#define CP_ACCENT_GREEN QColor(0, 255, 128)
#define CP_ACCENT_RED   QColor(255, 50, 50)

// ==================== NotificationWidget Implementation ====================

NotificationWidget::NotificationWidget(const Notification &notification, QWidget *parent)
    : QWidget(parent)
    , m_notification(notification)
    , m_opacity(0.0f)
    , m_slideOffset(100)
    , m_isHovered(false)
    , m_opacityAnimation(nullptr)
    , m_slideAnimation(nullptr)
    , m_autoDismissTimer(nullptr)
{
    setFixedSize(380, 80);
    setCursor(Qt::PointingHandCursor);
    setupUI();
}

void NotificationWidget::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(15, 10, 15, 10);
    mainLayout->setSpacing(12);

    // Icon
    QLabel *iconLabel = new QLabel(getTypeIcon());
    iconLabel->setStyleSheet(QString("font-size: 24px; color: %1;").arg(getTypeColor().name()));
    iconLabel->setFixedSize(40, 40);
    iconLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(iconLabel);

    // Content
    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->setSpacing(4);

    QLabel *titleLabel = new QLabel(m_notification.title);
    titleLabel->setStyleSheet("color: #FFFFFF; font-size: 14px; font-weight: bold;");
    contentLayout->addWidget(titleLabel);

    QLabel *messageLabel = new QLabel(m_notification.message);
    messageLabel->setStyleSheet("color: #AAAAAA; font-size: 12px;");
    messageLabel->setWordWrap(true);
    contentLayout->addWidget(messageLabel);

    mainLayout->addLayout(contentLayout, 1);

    // Close button
    QLabel *closeLabel = new QLabel("✕");
    closeLabel->setStyleSheet("color: #888888; font-size: 16px;");
    closeLabel->setCursor(Qt::PointingHandCursor);
    closeLabel->setFixedSize(20, 20);
    closeLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(closeLabel);

    // Shadow effect
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 100));
    shadow->setOffset(0, 5);
    setGraphicsEffect(shadow);

    // Auto-dismiss timer
    if (!m_notification.persistent && m_notification.duration > 0) {
        m_autoDismissTimer = new QTimer(this);
        m_autoDismissTimer->setSingleShot(true);
        connect(m_autoDismissTimer, &QTimer::timeout, this, &NotificationWidget::dismissAnimated);
        m_autoDismissTimer->start(m_notification.duration);
    }
}

QColor NotificationWidget::getTypeColor() const
{
    switch (m_notification.type) {
    case Notification_Success:
        return CP_ACCENT_GREEN;
    case Notification_Warning:
        return QColor(255, 200, 0);
    case Notification_Error:
        return CP_ACCENT_RED;
    case Notification_AI_Processing:
        return CP_ACCENT_PURPLE;
    default:
        return CP_ACCENT_CYAN;
    }
}

QString NotificationWidget::getTypeIcon() const
{
    switch (m_notification.type) {
    case Notification_Success:
        return "✓";
    case Notification_Warning:
        return "⚠";
    case Notification_Error:
        return "✕";
    case Notification_AI_Processing:
        return "🤖";
    default:
        return "ℹ";
    }
}

void NotificationWidget::setOpacity(float opacity)
{
    m_opacity = opacity;
    setWindowOpacity(opacity);
    update();
}

void NotificationWidget::setSlideOffset(int offset)
{
    m_slideOffset = offset;
    move(x() + offset, y());
}

void NotificationWidget::showAnimated()
{
    // Slide in animation
    m_slideAnimation = new QPropertyAnimation(this, "slideOffset");
    m_slideAnimation->setDuration(300);
    m_slideAnimation->setStartValue(100);
    m_slideAnimation->setEndValue(0);
    m_slideAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_slideAnimation->start();

    // Fade in animation
    m_opacityAnimation = new QPropertyAnimation(this, "opacity");
    m_opacityAnimation->setDuration(300);
    m_opacityAnimation->setStartValue(0.0f);
    m_opacityAnimation->setEndValue(1.0f);
    m_opacityAnimation->start();
}

void NotificationWidget::dismissAnimated()
{
    if (m_autoDismissTimer) {
        m_autoDismissTimer->stop();
    }

    // Slide out animation
    QPropertyAnimation *slideOut = new QPropertyAnimation(this, "slideOffset");
    slideOut->setDuration(250);
    slideOut->setStartValue(0);
    slideOut->setEndValue(100);
    slideOut->setEasingCurve(QEasingCurve::InCubic);

    // Fade out animation
    QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "opacity");
    fadeOut->setDuration(250);
    fadeOut->setStartValue(1.0f);
    fadeOut->setEndValue(0.0f);

    connect(fadeOut, &QPropertyAnimation::finished, this, [this]() {
        emit dismissed(m_notification.id);
    });

    slideOut->start();
    fadeOut->start();
}

void NotificationWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Glass background
    QColor bgColor(25, 25, 45, 240);
    painter.fillRect(rect(), bgColor);

    // Border with type color
    QColor borderColor = getTypeColor();
    borderColor.setAlpha(100);
    painter.setPen(QPen(borderColor, 1));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 10, 10);

    // Left accent bar
    painter.fillRect(0, 10, 4, height() - 20, getTypeColor());

    // Progress bar for auto-dismiss
    if (m_autoDismissTimer && m_autoDismissTimer->isActive() && !m_isHovered) {
        int elapsed = m_autoDismissTimer->interval() - m_autoDismissTimer->remainingTime();
        int progress = elapsed * width() / m_autoDismissTimer->interval();

        QColor progressColor = getTypeColor();
        progressColor.setAlpha(50);
        painter.fillRect(0, height() - 3, progress, 3, progressColor);
    }
}

void NotificationWidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    emit clicked(m_notification.id);
    dismissAnimated();
}

void NotificationWidget::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event)
    m_isHovered = true;
    update();

    // Pause auto-dismiss on hover
    if (m_autoDismissTimer) {
        m_autoDismissTimer->stop();
    }
}

void NotificationWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_isHovered = false;
    update();

    // Resume auto-dismiss
    if (m_autoDismissTimer && !m_notification.persistent) {
        m_autoDismissTimer->start(m_notification.duration / 2);
    }
}

// ==================== NotificationManager Implementation ====================

NotificationManager::NotificationManager(QWidget *parent)
    : QWidget(parent)
    , m_enabled(true)
    , m_maxNotifications(5)
    , m_defaultDuration(5000)
    , m_notificationSpacing(10)
    , m_notificationWidth(380)
{
    setupUI();
}

void NotificationManager::setupUI()
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(20, 20, 20, 20);
    m_layout->setSpacing(m_notificationSpacing);
    m_layout->setAlignment(Qt::AlignTop | Qt::AlignRight);
}

void NotificationManager::showNotification(const QString &title, const QString &message,
                                           NotificationType type, int durationMs)
{
    Notification notification;
    notification.id = QUuid::createUuid().toString();
    notification.title = title;
    notification.message = message;
    notification.type = type;
    notification.timestamp = QDateTime::currentDateTime();
    notification.duration = durationMs > 0 ? durationMs : m_defaultDuration;
    notification.persistent = (durationMs <= 0);

    showNotification(notification);
}

void NotificationManager::showNotification(const Notification &notification)
{
    if (!m_enabled) return;

    // Add to history
    m_notificationHistory.append(notification);

    // Check if we can show immediately
    if (m_activeNotifications.size() < m_maxNotifications) {
        NotificationWidget *widget = new NotificationWidget(notification, this);
        connect(widget, &NotificationWidget::dismissed, this, &NotificationManager::onNotificationDismissed);
        connect(widget, &NotificationWidget::clicked, this, &NotificationManager::onNotificationClicked);

        addNotificationWidget(widget);
        widget->showAnimated();

        emit notificationShown(notification.id);
    } else {
        // Add to queue
        m_notificationQueue.enqueue(notification);
    }
}

void NotificationManager::addNotificationWidget(NotificationWidget *widget)
{
    m_activeNotifications.append(widget);
    m_layout->insertWidget(0, widget);
}

void NotificationManager::removeNotificationWidget(const QString &id)
{
    NotificationWidget *widget = findWidget(id);
    if (widget) {
        m_activeNotifications.removeAll(widget);
        widget->deleteLater();
    }
}

NotificationWidget *NotificationManager::findWidget(const QString &id)
{
    for (NotificationWidget *widget : m_activeNotifications) {
        if (widget->id() == id) {
            return widget;
        }
    }
    return nullptr;
}

void NotificationManager::dismissNotification(const QString &id)
{
    NotificationWidget *widget = findWidget(id);
    if (widget) {
        widget->dismissAnimated();
    }
}

void NotificationManager::dismissAllNotifications()
{
    for (NotificationWidget *widget : m_activeNotifications) {
        widget->dismissAnimated();
    }
    m_notificationQueue.clear();
}

void NotificationManager::onNotificationDismissed(const QString &id)
{
    removeNotificationWidget(id);
    emit notificationDismissed(id);

    // Process queue
    QTimer::singleShot(100, this, &NotificationManager::processQueue);
}

void NotificationManager::onNotificationClicked(const QString &id)
{
    emit notificationClicked(id);
}

void NotificationManager::processQueue()
{
    while (m_activeNotifications.size() < m_maxNotifications && !m_notificationQueue.isEmpty()) {
        Notification notification = m_notificationQueue.dequeue();
        showNotification(notification);
    }
}

void NotificationManager::repositionNotifications()
{
    // Reposition notifications if needed
}

int NotificationManager::activeNotificationCount() const
{
    return m_activeNotifications.size();
}

bool NotificationManager::hasNotifications() const
{
    return !m_activeNotifications.isEmpty() || !m_notificationQueue.isEmpty();
}

void NotificationManager::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void NotificationManager::setMaxNotifications(int max)
{
    m_maxNotifications = max;
}

void NotificationManager::setDefaultDuration(int durationMs)
{
    m_defaultDuration = durationMs;
}

void NotificationManager::clearHistory()
{
    m_notificationHistory.clear();
}

// ==================== ToastNotification Implementation ====================

ToastNotification::ToastNotification(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , m_opacity(0.0f)
    , m_animation(nullptr)
    , m_timer(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, 60);
}

void ToastNotification::show(QWidget *parent, const QString &message,
                              NotificationType type, int durationMs)
{
    ToastNotification *toast = new ToastNotification(parent);
    toast->m_message = message;
    toast->m_type = type;

    // Position at center bottom of parent
    if (parent) {
        QPoint pos = parent->mapToGlobal(QPoint(
            (parent->width() - toast->width()) / 2,
            parent->height() - toast->height() - 50
        ));
        toast->move(pos);
    }

    toast->showAnimated();

    // Auto dismiss
    QTimer::singleShot(durationMs, toast, &ToastNotification::dismissAnimated);
}

void ToastNotification::setOpacity(float opacity)
{
    m_opacity = opacity;
    setWindowOpacity(opacity);
    update();
}

void ToastNotification::showAnimated()
{
    show();

    m_animation = new QPropertyAnimation(this, "opacity");
    m_animation->setDuration(200);
    m_animation->setStartValue(0.0f);
    m_animation->setEndValue(1.0f);
    m_animation->start();
}

void ToastNotification::dismissAnimated()
{
    m_animation = new QPropertyAnimation(this, "opacity");
    m_animation->setDuration(200);
    m_animation->setStartValue(1.0f);
    m_animation->setEndValue(0.0f);

    connect(m_animation, &QPropertyAnimation::finished, this, &QWidget::deleteLater);
    m_animation->start();
}

void ToastNotification::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background
    QColor bgColor(30, 30, 50, 240);
    painter.fillRect(rect(), bgColor);

    // Border
    QColor borderColor;
    switch (m_type) {
    case Notification_Success:
        borderColor = CP_ACCENT_GREEN;
        break;
    case Notification_Warning:
        borderColor = QColor(255, 200, 0);
        break;
    case Notification_Error:
        borderColor = CP_ACCENT_RED;
        break;
    default:
        borderColor = CP_ACCENT_CYAN;
    }

    painter.setPen(QPen(borderColor, 1));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 8, 8);

    // Message
    painter.setPen(Qt::white);
    QFont font;
    font.setPointSize(12);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, m_message);
}
