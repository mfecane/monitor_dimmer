#include "BackEnd.h"

#include <QSystemTrayIcon>
#include <QMenu>

extern ScreenList* g_screenList;

BackEnd* BackEnd::m_instance = nullptr;

QObject* BackEnd::qmlInstance(QQmlEngine* engine, QJSEngine* scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return (QObject*)getInstance();
}

BackEnd* BackEnd::getInstance() {
    if (!m_instance) {
        m_instance = new BackEnd();
    }
    return m_instance;
}

BackEnd::BackEnd(QObject *parent):
    m_settings("Gavitka software", "Dimmer")
{
    setOpacity(m_settings.value("opacity").toInt());
}

BackEnd::~BackEnd() {

}

int BackEnd::opacity() {
    return m_opacity;
}

void BackEnd::setOpacity(int value) {
    m_opacity = value;
    g_screenList->setOpacity(m_opacity);
    emit settingsChanged();
    m_settings.setValue("opacity", value);
}

bool BackEnd::enabled() {
    return m_enabled;
}

void BackEnd::setEnabled(bool value) {
    if(value == true) {
        g_screenList->start();
    }
    else {
        g_screenList->stop();
    }
    m_enabled = value;
    emit settingsChanged();
}
