#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QEvent>
#include <QKeyEvent>
#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE

class EventHandler : public QObject {
    Q_OBJECT

public:
    explicit EventHandler(QObject* parent = nullptr, QWidget* tabLeft = nullptr, QWidget* tabRight = nullptr)
        : QObject(parent), tabWidgetLeft(tabLeft), tabWidgetRight(tabRight)
    {
        qApp->installEventFilter(this);
    }

signals:
    void tabLeftClicked();
    void tabRightClicked();
    void keyEvent_C();

protected:
    auto eventFilter(QObject* obj, QEvent* event) -> bool override{
        if (event->type() == QEvent::MouseButtonPress) {
            QWidget* w = qobject_cast<QWidget*>(obj);
            if (w == nullptr)
            {
                return false;
            }

            if (tabWidgetLeft != nullptr && tabWidgetLeft->isAncestorOf(w)) {
                emit tabLeftClicked();
            } else if (tabWidgetRight != nullptr && tabWidgetRight->isAncestorOf(w)) {
                emit tabRightClicked();
            }

            // reset ignores: (her hangi bir mouse tıklama eventi tuş ile fokus ignore'larını resetlemeli)
            m_shouldIgnoreNextC = true;
        }
        if (event->type() == QEvent::KeyPress) {
            auto keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_C) {
                if (m_shouldIgnoreNextC) {
                    emit keyEvent_C();
                    m_shouldIgnoreNextC = false;
                    return true;
                }
            }
        }
        return QObject::eventFilter(obj, event);
    }

private:
    QWidget* tabWidgetLeft = nullptr;
    QWidget* tabWidgetRight = nullptr;

    bool m_shouldIgnoreNextC = true;
};

FM_END_NAMESPACE
#endif // EVENTHANDLER_H
