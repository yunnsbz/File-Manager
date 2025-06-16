#ifndef FILEOPERATIONMANAGER_H
#define FILEOPERATIONMANAGER_H

#include "IFileOperation.h"

#include <QString>
#include <QQueue>
#include <QFileInfo>
#include <QObject>

class MainWindow;

class FileOperationManager : public QObject
{
    Q_OBJECT

public:
    explicit FileOperationManager(QObject* parent = nullptr);

    // void undoLast();
    // geçmişten bir işlem seçip geri almak içindir. void undo(QVariantMap params);

    void DeleteOperation(QList<QString> src);
    // void MoveOperation(QList<QString> src, QString dst);

    void MoveOperation(QString dst);

    // copyalananları birden fazla yolun her birine yapıştırır.
    // void PasteOperationMultiple(QList<QString> destinations);

    // // tek bir dosyanın ismini değiştirmek için kullanılır.
    // void RenameOperationBasic(QString src,QString name);

    // // birden fazla bir dosyanın ismini farklı şekilde değiştirmek için kullanılır. (prefix, postfix vb.)
    // void RenameOperationAdvanced(QList<QString> src);

    // // hedef yol içinde boş klasör oluşturur.
    // void CreateNewFolder(QString dst);

    // seçilenleri kesme işlemi için kaydeder. sonrasında PasteOperation çağırılmalıdır
    void addToCut(QString src);

    // sadece geçici olarak kopyalanma adreslerini tutar. kopyalama işlemini bitirmez. sonrasında PasteOperation çağırılmalıdır.
    void addToCopy(QString src);


protected:


private:
    MainWindow* mainWindow_;

    QList<QVariantMap> operationHistory;

    // string key girdisinde snake case kullanılır
    QMap<QString, IFileOperation*> operationMap;
    QVariantMap currentOperation;

    // data for copied or cuted file paths: used for pasteOperation. it will be cleared after using
    QSet<QString> copiedPaths;

    // kesme işlemi için kullanılır. eğer true ise move işlemi copyaladığı dosyaları kaynaktan siler.
    bool isFilesSelectedToCut = false;

private slots:
    void onProgress(int percent);
    void onError(const QString &msg);
    void onFinished(const QString &info);
};

#endif // FILEOPERATIONMANAGER_H
