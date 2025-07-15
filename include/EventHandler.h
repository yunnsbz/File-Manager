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
        : QObject(parent), m_tabWidgetLeft(tabLeft), m_tabWidgetRight(tabRight)
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

            if (m_tabWidgetLeft != nullptr && m_tabWidgetLeft->isAncestorOf(w)) {
                emit tabLeftClicked();
            } else if (m_tabWidgetRight != nullptr && m_tabWidgetRight->isAncestorOf(w)) {
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
    QWidget* m_tabWidgetLeft = nullptr;
    QWidget* m_tabWidgetRight = nullptr;

    // şu anda c harfine basıldığında adres line edit'ine odaklanılıyor ama ilk basışta c harfini yazdırmamak içn bir ignore mekanizması kullanılır
    bool m_shouldIgnoreNextC = true;
};

FM_END_NAMESPACE
#endif // EVENTHANDLER_H
