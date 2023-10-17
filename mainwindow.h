// mainwindow.h

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void moveModUp();
    void moveModDown();
    void moveMod(bool moveUp);
    void importBgyml();
    void onModCheckBoxChanged(int state);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browseOutputModsFolder();
    void generatePatch();
    void populateModsList();
    void reloadModsList();
    void renameIniFiles(int fromRow, int toRow);
    void deleteMod();

private:
    QTableWidget *modsListTable;
    QLineEdit *outputModsFolderLineEdit;
    QPushButton *browseButton;
    QPushButton *moveUpButton;
    QPushButton *moveDownButton;
    bool userInitiatedMoveAction = false;
    QSettings *appSettings;
    QString settingsFilePath;
    QStringList m_enabledModIDs;
    int currentSelectedRow;
    QWidget* cloneCheckboxWidget(QWidget* original);
};

#endif // MAINWINDOW_H
