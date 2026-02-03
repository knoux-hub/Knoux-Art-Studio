#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H
#include <QString>
namespace Knoux {
class StyleManager {
public:
    static QString cyberpunkStyle() {
        return "QMainWindow { background: #0A0A19; color: #FFFFFF; }";
    }
};
}
#endif
