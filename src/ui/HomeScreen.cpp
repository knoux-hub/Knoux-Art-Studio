#include "HomeScreen.h"
#include "GlassButton.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <QDebug>

HomeScreen::HomeScreen(QWidget *parent)
    : QWidget(parent)
    , m_welcomeLabel(nullptr)
    , m_dateLabel(nullptr)
    , m_photoCard(nullptr)
    , m_videoCard(nullptr)
    , m_aiCard(nullptr)
    , m_bodyCard(nullptr)
    , m_faceCard(nullptr)
    , m_makeupCard(nullptr)
    , m_batchCard(nullptr)
    , m_recentProjectsContainer(nullptr)
    , m_templatesContainer(nullptr)
    , m_entryAnimation(nullptr)
{
    setupUI();
    loadRecentProjects();

    QTimer::singleShot(100, this, &HomeScreen::animateEntry);
}

void HomeScreen::setupUI()
{
    setObjectName("homeScreen");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 30, 40, 30);
    mainLayout->setSpacing(25);

    // Header
    createHeader();
    mainLayout->addLayout(qobject_cast<QHBoxLayout*>(m_welcomeLabel->parentWidget()->layout()));

    // Quick actions
    createQuickActions();
    mainLayout->addWidget(m_photoCard->parentWidget());

    // Recent projects
    createRecentProjects();
    mainLayout->addWidget(m_recentProjectsContainer);

    // Templates
    createTemplates();
    mainLayout->addWidget(m_templatesContainer, 1);
}

void HomeScreen::createHeader()
{
    QWidget *headerWidget = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    // Welcome message
    QVBoxLayout *welcomeLayout = new QVBoxLayout();
    welcomeLayout->setSpacing(5);

    m_welcomeLabel = new QLabel(tr("مرحباً بك في Knoux"), this);
    m_welcomeLabel->setStyleSheet(
        "font-size: 28px; font-weight: bold; color: #FFFFFF;"
        "font-family: 'Dubai', sans-serif;"
    );
    welcomeLayout->addWidget(m_welcomeLabel);

    QString arabicDate = QDateTime::currentDateTime().toString("dddd، d MMMM yyyy");
    m_dateLabel = new QLabel(arabicDate, this);
    m_dateLabel->setStyleSheet(
        "font-size: 14px; color: #00FFFF;"
        "font-family: 'Dubai', sans-serif;"
    );
    m_dateLabel->setLayoutDirection(Qt::RightToLeft);
    welcomeLayout->addWidget(m_dateLabel);

    headerLayout->addLayout(welcomeLayout);
    headerLayout->addStretch();

    // Status indicators
    QLabel *offlineLabel = new QLabel("🔒 " + tr("وضع آمن - بدون إنترنت"), this);
    offlineLabel->setStyleSheet(
        "font-size: 12px; color: #00FF88; padding: 8px 15px;"
        "background: rgba(0, 255, 136, 0.1); border-radius: 20px;"
        "border: 1px solid rgba(0, 255, 136, 0.3);"
    );
    headerLayout->addWidget(offlineLabel);
}

void HomeScreen::createQuickActions()
{
    QWidget *container = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(20);

    // Photo editor card
    m_photoCard = new QuickActionCard(
        "📷", tr("محرر الصور"), 
        tr("تحرير احترافي مع AI"),
        QColor(0, 255, 255), this);
    connect(m_photoCard, &QuickActionCard::clicked, this, &HomeScreen::onNewPhotoProject);
    layout->addWidget(m_photoCard);

    // Video editor card
    m_videoCard = new QuickActionCard(
        "🎬", tr("محرر الفيديو"),
        tr("Timeline متعدد المسارات"),
        QColor(255, 0, 128), this);
    connect(m_videoCard, &QuickActionCard::clicked, this, &HomeScreen::onNewVideoProject);
    layout->addWidget(m_videoCard);

    // AI Studio card
    m_aiCard = new QuickActionCard(
        "🤖", tr("استوديو AI"),
        tr("توليد وتحسين بالذكاء الاصطناعي"),
        QColor(180, 0, 255), this);
    connect(m_aiCard, &QuickActionCard::clicked, this, &HomeScreen::onOpenAIStudio);
    layout->addWidget(m_aiCard);

    // Body Editor card
    m_bodyCard = new QuickActionCard(
        "👤", tr("تعديل الجسم"),
        tr("تعديلات الجسم بالذكاء الاصطناعي"),
        QColor(0, 200, 255), this);
    connect(m_bodyCard, &QuickActionCard::clicked, this, &HomeScreen::openBodyEditor);
    layout->addWidget(m_bodyCard);

    // Face Retouch card
    m_faceCard = new QuickActionCard(
        "😊", tr("ريتوش الوجه"),
        tr("تعديلات الوجه المتقدمة"),
        QColor(255, 100, 150), this);
    connect(m_faceCard, &QuickActionCard::clicked, this, &HomeScreen::openFaceRetouch);
    layout->addWidget(m_faceCard);

    // Makeup Studio card
    m_makeupCard = new QuickActionCard(
        "💄", tr("استوديو المكياج"),
        tr("مكياج افتراضي احترافي"),
        QColor(255, 105, 180), this);
    connect(m_makeupCard, &QuickActionCard::clicked, this, &HomeScreen::openMakeupStudio);
    layout->addWidget(m_makeupCard);

    // Batch processing card
    m_batchCard = new QuickActionCard(
        "⚡", tr("معالجة دفعات"),
        tr("تطبيق تعديلات على عدة ملفات"),
        QColor(255, 200, 0), this);
    layout->addWidget(m_batchCard);
}

void HomeScreen::createRecentProjects()
{
    m_recentProjectsContainer = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_recentProjectsContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(15);

    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel(tr("المشاريع الحديثة"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    GlassButton *viewAllBtn = new GlassButton(tr("عرض الكل →"), this);
    viewAllBtn->setFixedWidth(120);
    headerLayout->addWidget(viewAllBtn);

    layout->addLayout(headerLayout);

    // Projects grid
    QHBoxLayout *projectsLayout = new QHBoxLayout();
    projectsLayout->setSpacing(15);

    // Placeholder cards - will be populated by loadRecentProjects
    for (int i = 0; i < 4; ++i) {
        QWidget *placeholder = new QWidget(this);
        placeholder->setFixedSize(200, 140);
        placeholder->setStyleSheet(
            "background: rgba(255, 255, 255, 0.05);"
            "border-radius: 12px; border: 1px dashed rgba(255, 255, 255, 0.2);"
        );
        projectsLayout->addWidget(placeholder);
    }

    projectsLayout->addStretch();
    layout->addLayout(projectsLayout);
}

void HomeScreen::createTemplates()
{
    m_templatesContainer = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(m_templatesContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(15);

    QLabel *titleLabel = new QLabel(tr("القوالب السريعة"), this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #FFFFFF;");
    layout->addWidget(titleLabel);

    QHBoxLayout *templatesLayout = new QHBoxLayout();
    templatesLayout->setSpacing(15);

    QStringList templates = {
        tr("يوتيوب 4K"), tr("إنستجرام"), tr("تيك توك"),
        tr("بورتريه"), tr("منظر طبيعي"), tr("منتج")
    };

    for (int i = 0; i < templates.size(); ++i) {
        GlassButton *btn = new GlassButton(templates[i], this);
        btn->setFixedSize(120, 80);
        connect(btn, &QPushButton::clicked, this, [this, i]() {
            onTemplateClicked(i);
        });
        templatesLayout->addWidget(btn);
    }

    templatesLayout->addStretch();
    layout->addLayout(templatesLayout);
    layout->addStretch();
}

void HomeScreen::loadRecentProjects()
{
    QSettings settings;
    QStringList recent = settings.value("recent_projects").toStringList();

    // TODO: Populate recent project cards
}

void HomeScreen::animateEntry()
{
    // Animate cards appearing
    QList<QuickActionCard*> cards = findChildren<QuickActionCard*>();
    for (int i = 0; i < cards.size(); ++i) {
        cards[i]->setWindowOpacity(0);
        QPropertyAnimation *anim = new QPropertyAnimation(cards[i], "windowOpacity", this);
        anim->setDuration(400);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
        anim->setDelay(i * 100);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void HomeScreen::onNewPhotoProject() { emit openPhotoEditor(); }
void HomeScreen::onNewVideoProject() { emit openVideoEditor(); }
void HomeScreen::onOpenExistingProject() { /* TODO */ }
void HomeScreen::onOpenAIStudio() { emit openAIStudio(); }
void HomeScreen::onRecentProjectClicked(const QString &path) { emit openProject(path); }
void HomeScreen::onTemplateClicked(int templateId) { /* TODO */ }

// QuickActionCard implementation
QuickActionCard::QuickActionCard(const QString &icon, const QString &title,
                                  const QString &subtitle, const QColor &accentColor,
                                  QWidget *parent)
    : QWidget(parent)
    , m_icon(icon)
    , m_title(title)
    , m_subtitle(subtitle)
    , m_accentColor(accentColor)
    , m_isHovered(false)
    , m_glowIntensity(0.0f)
{
    setFixedSize(200, 160);
    setCursor(Qt::PointingHandCursor);
}

void QuickActionCard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect = this->rect().adjusted(2, 2, -2, -2);

    // Background with glass effect
    QLinearGradient bgGrad(rect.topLeft(), rect.bottomRight());
    bgGrad.setColorAt(0, QColor(255, 255, 255, m_isHovered ? 25 : 15));
    bgGrad.setColorAt(1, QColor(255, 255, 255, m_isHovered ? 15 : 8));

    painter.setBrush(bgGrad);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, 16, 16);

    // Glow effect
    if (m_isHovered || m_glowIntensity > 0) {
        QColor glow = m_accentColor;
        glow.setAlpha(int((m_isHovered ? 60 : 30) + m_glowIntensity));
        painter.setPen(QPen(glow, 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect.adjusted(-3, -3, 3, 3), 18, 18);
    }

    // Border
    painter.setPen(QPen(QColor(m_accentColor.red(), m_accentColor.green(), 
                               m_accentColor.blue(), m_isHovered ? 100 : 50), 1));
    painter.drawRoundedRect(rect, 16, 16);

    // Icon
    QFont iconFont;
    iconFont.setPointSize(32);
    painter.setFont(iconFont);
    painter.setPen(Qt::white);
    painter.drawText(rect.adjusted(0, 20, 0, 0), Qt::AlignHCenter | Qt::AlignTop, m_icon);

    // Title
    QFont titleFont("Dubai", 14, QFont::Bold);
    painter.setFont(titleFont);
    painter.setPen(Qt::white);
    painter.drawText(rect.adjusted(0, 75, 0, 0), Qt::AlignHCenter | Qt::AlignTop, m_title);

    // Subtitle
    QFont subFont("Dubai", 10);
    painter.setFont(subFont);
    painter.setPen(QColor(180, 180, 200));
    painter.drawText(rect.adjusted(0, 100, 0, 0), Qt::AlignHCenter | Qt::AlignTop, m_subtitle);
}

void QuickActionCard::enterEvent(QEnterEvent *event)
{
    m_isHovered = true;
    update();
}

void QuickActionCard::leaveEvent(QEvent *event)
{
    m_isHovered = false;
    update();
}

void QuickActionCard::mousePressEvent(QMouseEvent *event)
{
    emit clicked();
}
