#include "StyleManager.h"
#include <QWidget>
#include <QApplication>

namespace Knoux {

QString StyleManager::applicationStyle() {
    return QString(R"(
        QMainWindow {
            background-color: )" CP_BG_DARK R"(;
            border: none;
        }
        
        QWidget {
            background-color: )" CP_BG_DARK R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
            font-size: 13px;
        }
        
        QLabel {
            background-color: transparent;
            color: )" CP_TEXT_PRIMARY R"(;
        }
        
        QToolTip {
            background-color: )" CP_BG_PANEL R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_ACCENT_CYAN R"(;
            border-radius: 4px;
            padding: 8px;
            font-size: 12px;
        }
    )");
}

QString StyleManager::mainWindowStyle() {
    return QString(R"(
        QMainWindow {
            background-color: )" CP_BG_DARK R"(;
            border: none;
        }
        
        QMainWindow::separator {
            background-color: )" CP_BORDER R"(;
            width: 2px;
            height: 2px;
        }
        
        QMainWindow::separator:hover {
            background-color: )" CP_ACCENT_CYAN R"(;
        }
    )");
}

QString StyleManager::glassPanelStyle() {
    return QString(R"(
        QWidget[glass="true"] {
            background-color: rgba(18, 18, 42, 180);
            border: 1px solid rgba(0, 255, 255, 50);
            border-radius: 12px;
        }
        
        QWidget[glass="true"]:hover {
            border: 1px solid rgba(0, 255, 255, 100);
            background-color: rgba(26, 26, 62, 200);
        }
    )");
}

QString StyleManager::glassButtonStyle() {
    return QString(R"(
        QPushButton {
            background-color: rgba(0, 255, 255, 30);
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_ACCENT_CYAN R"(;
            border-radius: 8px;
            padding: 10px 20px;
            font-weight: 500;
        }
        
        QPushButton:hover {
            background-color: rgba(0, 255, 255, 50);
            border: 1px solid )" CP_ACCENT_CYAN R"(;
        }
        
        QPushButton:pressed {
            background-color: rgba(0, 255, 255, 70);
        }
        
        QPushButton:disabled {
            background-color: rgba(100, 100, 100, 30);
            border: 1px solid rgba(100, 100, 100, 50);
            color: )" CP_TEXT_SECONDARY R"(;
        }
        
        QPushButton:checked {
            background-color: rgba(0, 255, 255, 60);
            border: 2px solid )" CP_ACCENT_CYAN R"(;
        }
    )");
}

QString StyleManager::tabStyle() {
    return QString(R"(
        QTabWidget::pane {
            background-color: rgba(18, 18, 42, 180);
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 8px;
            top: -1px;
        }
        
        QTabBar::tab {
            background-color: rgba(26, 26, 62, 150);
            color: )" CP_TEXT_SECONDARY R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-bottom: none;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            padding: 10px 20px;
            margin-right: 4px;
        }
        
        QTabBar::tab:selected {
            background-color: rgba(0, 255, 255, 30);
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_ACCENT_CYAN R"(;
            border-bottom: none;
        }
        
        QTabBar::tab:hover:!selected {
            background-color: rgba(0, 255, 255, 15);
            color: )" CP_TEXT_PRIMARY R"(;
        }
    )");
}

QString StyleManager::sliderStyle() {
    return QString(R"(
        QSlider::groove:horizontal {
            background-color: )" CP_BG_CARD R"(;
            height: 6px;
            border-radius: 3px;
        }
        
        QSlider::sub-page:horizontal {
            background-color: )" CP_ACCENT_CYAN R"(;
            height: 6px;
            border-radius: 3px;
        }
        
        QSlider::handle:horizontal {
            background-color: )" CP_ACCENT_CYAN R"(;
            width: 16px;
            height: 16px;
            margin: -5px 0;
            border-radius: 8px;
            border: 2px solid )" CP_TEXT_PRIMARY R"(;
        }
        
        QSlider::handle:horizontal:hover {
            background-color: )" CP_ACCENT_PINK R"(;
            width: 18px;
            height: 18px;
            margin: -6px 0;
        }
    )");
}

QString StyleManager::scrollbarStyle() {
    return QString(R"(
        QScrollBar:vertical {
            background-color: )" CP_BG_PANEL R"(;
            width: 12px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:vertical {
            background-color: )" CP_BORDER R"(;
            min-height: 30px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: )" CP_ACCENT_CYAN R"(;
        }
        
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height: 0px;
        }
        
        QScrollBar:horizontal {
            background-color: )" CP_BG_PANEL R"(;
            height: 12px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:horizontal {
            background-color: )" CP_BORDER R"(;
            min-width: 30px;
            border-radius: 6px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background-color: )" CP_ACCENT_CYAN R"(;
        }
        
        QScrollBar::add-line:horizontal,
        QScrollBar::sub-line:horizontal {
            width: 0px;
        }
    )");
}

QString StyleManager::menuStyle() {
    return QString(R"(
        QMenu {
            background-color: )" CP_BG_PANEL R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 8px;
            padding: 8px;
        }
        
        QMenu::item {
            background-color: transparent;
            color: )" CP_TEXT_PRIMARY R"(;
            padding: 8px 24px;
            border-radius: 4px;
        }
        
        QMenu::item:selected {
            background-color: rgba(0, 255, 255, 30);
        }
        
        QMenu::separator {
            height: 1px;
            background-color: )" CP_BORDER R"(;
            margin: 6px 12px;
        }
    )");
}

QString StyleManager::tooltipStyle() {
    return QString(R"(
        QToolTip {
            background-color: )" CP_BG_PANEL R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_ACCENT_CYAN R"(;
            border-radius: 6px;
            padding: 8px 12px;
            font-size: 12px;
        }
    )");
}

QString StyleManager::dialogStyle() {
    return QString(R"(
        QDialog {
            background-color: )" CP_BG_DARK R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 12px;
        }
        
        QDialogButtonBox {
            button-layout: 1;
        }
    )");
}

QString StyleManager::comboBoxStyle() {
    return QString(R"(
        QComboBox {
            background-color: )" CP_BG_CARD R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 6px;
            padding: 8px 12px;
            min-width: 100px;
        }
        
        QComboBox:hover {
            border: 1px solid )" CP_ACCENT_CYAN R"(;
        }
        
        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 24px;
            border-left: 1px solid )" CP_BORDER R"(;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid )" CP_ACCENT_CYAN R"(;
            width: 0;
            height: 0;
        }
        
        QComboBox QAbstractItemView {
            background-color: )" CP_BG_PANEL R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_BORDER R"(;
            selection-background-color: rgba(0, 255, 255, 30);
        }
    )");
}

QString StyleManager::spinBoxStyle() {
    return QString(R"(
        QSpinBox, QDoubleSpinBox {
            background-color: )" CP_BG_CARD R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 6px;
            padding: 8px;
        }
        
        QSpinBox:hover, QDoubleSpinBox:hover {
            border: 1px solid )" CP_ACCENT_CYAN R"(;
        }
        
        QSpinBox::up-button, QDoubleSpinBox::up-button {
            subcontrol-origin: border;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid )" CP_BORDER R"(;
            border-top-right-radius: 6px;
        }
        
        QSpinBox::down-button, QDoubleSpinBox::down-button {
            subcontrol-origin: border;
            subcontrol-position: bottom right;
            width: 20px;
            border-left: 1px solid )" CP_BORDER R"(;
            border-bottom-right-radius: 6px;
        }
    )");
}

QString StyleManager::lineEditStyle() {
    return QString(R"(
        QLineEdit {
            background-color: )" CP_BG_CARD R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 6px;
            padding: 8px 12px;
        }
        
        QLineEdit:hover {
            border: 1px solid )" CP_ACCENT_CYAN R"(;
        }
        
        QLineEdit:focus {
            border: 2px solid )" CP_ACCENT_CYAN R"(;
        }
        
        QLineEdit:disabled {
            background-color: rgba(100, 100, 100, 30);
            color: )" CP_TEXT_SECONDARY R"(;
        }
    )");
}

QString StyleManager::textEditStyle() {
    return QString(R"(
        QTextEdit {
            background-color: )" CP_BG_CARD R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 6px;
            padding: 8px;
        }
        
        QTextEdit:hover {
            border: 1px solid )" CP_ACCENT_CYAN R"(;
        }
        
        QTextEdit:focus {
            border: 2px solid )" CP_ACCENT_CYAN R"(;
        }
    )");
}

QString StyleManager::listWidgetStyle() {
    return QString(R"(
        QListWidget {
            background-color: )" CP_BG_CARD R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 6px;
            outline: none;
        }
        
        QListWidget::item {
            padding: 8px 12px;
            border-radius: 4px;
        }
        
        QListWidget::item:selected {
            background-color: rgba(0, 255, 255, 30);
        }
        
        QListWidget::item:hover {
            background-color: rgba(0, 255, 255, 15);
        }
    )");
}

QString StyleManager::treeWidgetStyle() {
    return QString(R"(
        QTreeWidget {
            background-color: )" CP_BG_CARD R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 6px;
            outline: none;
        }
        
        QTreeWidget::item {
            padding: 6px 8px;
        }
        
        QTreeWidget::item:selected {
            background-color: rgba(0, 255, 255, 30);
        }
        
        QTreeWidget::item:hover {
            background-color: rgba(0, 255, 255, 15);
        }
        
        QTreeWidget::branch {
            background-color: transparent;
        }
        
        QTreeWidget::branch:has-children:!has-siblings:closed,
        QTreeWidget::branch:closed:has-children:has-siblings {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid )" CP_ACCENT_CYAN R"(;
        }
        
        QTreeWidget::branch:open:has-children:!has-siblings,
        QTreeWidget::branch:open:has-children:has-siblings {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-bottom: 6px solid )" CP_ACCENT_CYAN R"(;
        }
    )");
}

QString StyleManager::tableWidgetStyle() {
    return QString(R"(
        QTableWidget {
            background-color: )" CP_BG_CARD R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 6px;
            gridline-color: )" CP_BORDER R"(;
            outline: none;
        }
        
        QTableWidget::item {
            padding: 8px;
        }
        
        QTableWidget::item:selected {
            background-color: rgba(0, 255, 255, 30);
        }
        
        QHeaderView::section {
            background-color: )" CP_BG_PANEL R"(;
            color: )" CP_TEXT_PRIMARY R"(;
            padding: 8px;
            border: none;
            border-right: 1px solid )" CP_BORDER R"(;
            font-weight: bold;
        }
        
        QHeaderView::section:hover {
            background-color: rgba(0, 255, 255, 15);
        }
    )");
}

QString StyleManager::progressBarStyle() {
    return QString(R"(
        QProgressBar {
            background-color: )" CP_BG_CARD R"(;
            border: 1px solid )" CP_BORDER R"(;
            border-radius: 6px;
            text-align: center;
            color: )" CP_TEXT_PRIMARY R"(;
        }
        
        QProgressBar::chunk {
            background-color: qlineargradient(
                x1: 0, y1: 0, x2: 1, y2: 0,
                stop: 0 )" CP_ACCENT_CYAN R"(,
                stop: 1 )" CP_ACCENT_PINK R"(
            );
            border-radius: 5px;
        }
    )");
}

QString StyleManager::animatedButtonStyle() {
    return QString(R"(
        QPushButton {
            background-color: rgba(0, 255, 255, 30);
            color: )" CP_TEXT_PRIMARY R"(;
            border: 1px solid )" CP_ACCENT_CYAN R"(;
            border-radius: 8px;
            padding: 12px 24px;
            font-weight: 500;
        }
        
        QPushButton:hover {
            background-color: rgba(0, 255, 255, 50);
            border: 1px solid )" CP_ACCENT_CYAN R"(;
        }
        
        QPushButton:pressed {
            background-color: rgba(0, 255, 255, 70);
        }
    )");
}

QString StyleManager::hoverEffectStyle() {
    return QString(R"(
        QWidget:hover {
            background-color: rgba(0, 255, 255, 10);
        }
    )");
}

QString StyleManager::glowEffectStyle(const QColor &color) {
    return QString(R"(
        QWidget {
            border: 1px solid %1;
        }
        
        QWidget:hover {
            border: 2px solid %1;
        }
    )").arg(color.name());
}

QColor StyleManager::blendColors(const QColor &c1, const QColor &c2, float ratio) {
    int r = static_cast<int>(c1.red() * (1 - ratio) + c2.red() * ratio);
    int g = static_cast<int>(c1.green() * (1 - ratio) + c2.green() * ratio);
    int b = static_cast<int>(c1.blue() * (1 - ratio) + c2.blue() * ratio);
    int a = static_cast<int>(c1.alpha() * (1 - ratio) + c2.alpha() * ratio);
    return QColor(r, g, b, a);
}

QString StyleManager::colorToRgba(const QColor &color, float alpha) {
    return QString("rgba(%1, %2, %3, %4)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(alpha);
}

QString StyleManager::colorToHex(const QColor &color) {
    return color.name();
}

void StyleManager::applyRtlStyle(QWidget *widget) {
    if (!widget) return;
    
    widget->setLayoutDirection(Qt::RightToLeft);
    
    // Apply to all children
    for (QObject *child : widget->children()) {
        if (QWidget *childWidget = qobject_cast<QWidget*>(child)) {
            applyRtlStyle(childWidget);
        }
    }
}

bool StyleManager::isRtl() {
    return QApplication::layoutDirection() == Qt::RightToLeft;
}

} // namespace Knoux
