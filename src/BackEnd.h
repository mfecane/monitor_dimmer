#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QSettings>
#include "ScreenList.h"

class BackEnd : public QObject {

    Q_OBJECT

    Q_PROPERTY(int opacity READ opacity WRITE setOpacity NOTIFY settingsChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY settingsChanged)

public:

    BackEnd(QObject* parent = nullptr);
    ~BackEnd();

    static QObject* qmlInstance(QQmlEngine* engine, QJSEngine* scriptEngine);
    static BackEnd* getInstance();

    int opacity();
    void setOpacity(int value);

    bool enabled();
    void setEnabled(bool value);
    void setScreenManage();

signals:

    void settingsChanged();

private:

    QSettings m_settings;
    float m_opacity = 0;
    static BackEnd* m_instance;
    bool m_enabled = false;
    ScreenList* m_screenmanager = nullptr;
};
