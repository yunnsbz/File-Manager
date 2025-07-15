#ifndef FILEOPERATIONMANAGER_H
#define FILEOPERATIONMANAGER_H

#include "IFileOperation.h"

#include <QObject>
#include <QString>
#include <QQueue>
#include <QFileInfo>
#include <QList>
#include <QVariantMap>
#include <QMap>
#include <QSet>

#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE

class MainWindow;

class FileOperationManager : public QObject
{
    Q_OBJECT

public:
    explicit FileOperationManager(QObject* parent = nullptr);

    // void undoLast();
    // geçmişten bir işlem seçip geri almak içindir. void undo(QVariantMap params);

    void deleteOperation(QList<QString> src);

    void moveOperation(QString dst);

    // copyalananları birden fazla yolun her birine yapıştırır.
    // void PasteOperationMultiple(QList<QString> destinations);

    // // tek bir dosyanın ismini değiştirmek için kullanılır.
    // void RenameOperationBasic(QString src,QString name);

    // // birden fazla bir dosyanın ismini farklı şekilde değiştirmek için kullanılır. (prefix, postfix vb.)
    // void RenameOperationAdvanced(QList<QString> src);

    // // hedef yol içinde boş klasör oluşturur.
    // void CreateNewFolder(QString dst);

    // seçilenleri kesme işlemi için kaydeder. sonrasında PasteOperation çağırılmalıdır
    void addToCut(const QString& src);

    // sadece geçici olarak kopyalanma adreslerini tutar. kopyalama işlemini bitirmez. sonrasında PasteOperation çağırılmalıdır.
    void addToCopy(const QString& src);


protected:


private:
    MainWindow* m_mainWindow_;

    QList<QVariantMap> m_operationHistory;

    // string key girdisinde snake case kullanılır
    QMap<QString, IFileOperation*> m_operationMap;
    QVariantMap m_currentOperation;

    // data for copied or cuted file paths: used for pasteOperation. it will be cleared after using
    QSet<QString> m_copiedPaths;

    // kesme işlemi için kullanılır. eğer true ise move işlemi copyaladığı dosyaları kaynaktan siler.
    bool m_isFilesSelectedToCut = false;

private slots:
    void onProgress(int percent);
    void onError(const QString &msg);
    void onFinished(const QString &info);
};


FM_END_NAMESPACE
#endif // FILEOPERATIONMANAGER_H
