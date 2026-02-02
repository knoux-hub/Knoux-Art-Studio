#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QString>
#include <QColor>

// Cyberpunk Color Palette
#define CP_BG_DARK      "#0A0A19"
#define CP_BG_PANEL     "#12122A"
#define CP_BG_CARD      "#1A1A3E"
#define CP_ACCENT_CYAN  "#00FFFF"
#define CP_ACCENT_PINK  "#FF0080"
#define CP_ACCENT_PURPLE "#B400FF"
#define CP_ACCENT_GOLD  "#FFD700"
#define CP_ACCENT_GREEN "#00FF88"
#define CP_ACCENT_RED   "#FF4444"
#define CP_TEXT_PRIMARY "#FFFFFF"
#define CP_TEXT_SECONDARY "#A0A0C0"
#define CP_BORDER       "#2A2A5A"

namespace Knoux {

/**
 * @brief Style Manager - Provides consistent styling across the application
 */
class StyleManager {
public:
    // Global styles
    static QString applicationStyle();
    static QString mainWindowStyle();
    static QString glassPanelStyle();
    static QString glassButtonStyle();
    static QString tabStyle();
    static QString sliderStyle();
    static QString scrollbarStyle();
    static QString menuStyle();
    static QString tooltipStyle();
    static QString dialogStyle();
    
    // Component styles
    static QString comboBoxStyle();
    static QString spinBoxStyle();
    static QString lineEditStyle();
    static QString textEditStyle();
    static QString listWidgetStyle();
    static QString treeWidgetStyle();
    static QString tableWidgetStyle();
    static QString progressBarStyle();
    
    // Animation helpers
    static QString animatedButtonStyle();
    static QString hoverEffectStyle();
    static QString glowEffectStyle(const QColor &color);
    
    // Color utilities
    static QColor blendColors(const QColor &c1, const QColor &c2, float ratio);
    static QString colorToRgba(const QColor &color, float alpha);
    static QString colorToHex(const QColor &color);
    
    // RTL support
    static void applyRtlStyle(QWidget *widget);
    static bool isRtl();
    
private:
    StyleManager() = delete;
};

} // namespace Knoux

#endif // STYLEMANAGER_H
