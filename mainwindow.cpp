#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDir>
#include <QFile>
#include <QCheckBox>
#include <QSettings>
#include <QCoreApplication>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QDesktopServices>
#include <QApplication>
#include <QRegularExpression>
#include <QInputDialog>
#include <QString>
#include <QPalette>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Set the main window title and dimensions
    this->setWindowIcon(QIcon(":/icon.ico"));
    this->setWindowTitle("AnimationResourceTool");
    this->resize(900, 700);

    // Make it dark
    qApp->setStyle("fusion");
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);

    // Initialize the QSettings object and settings file path
    settingsFilePath = QCoreApplication::applicationDirPath() + "/settings.ini";
    appSettings = new QSettings(settingsFilePath, QSettings::IniFormat, this);

    // Create a central widget to hold all other UI elements
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Output Mods Folder Section
    QGroupBox *outputModFolderGroup = new QGroupBox("Output Mod Folder", centralWidget);
    QVBoxLayout *outputModLayout = new QVBoxLayout(outputModFolderGroup);

    outputModsFolderLineEdit = new QLineEdit(outputModFolderGroup);
    outputModsFolderLineEdit->setPlaceholderText("Enter or browse the output mods folder path");

    // Load saved settings for "Output Mods Folder"
    QString outputModsFolder = appSettings->value("OutputModsFolder").toString();
    outputModsFolderLineEdit->setText(outputModsFolder);

    // Browse button to select the output mods folder
    browseButton = new QPushButton("Browse", outputModFolderGroup);
    connect(browseButton, &QPushButton::clicked, this, &MainWindow::browseOutputModsFolder);

    outputModLayout->addWidget(outputModsFolderLineEdit);
    outputModLayout->addWidget(browseButton);
    outputModFolderGroup->setLayout(outputModLayout);

    mainLayout->addWidget(outputModFolderGroup);

    // Mods List Section
    QGroupBox *modsListGroup = new QGroupBox("Animation Mods List", centralWidget);
    QVBoxLayout *modsListLayout = new QVBoxLayout(modsListGroup);

    // Create the "Reload Mods List" button
    QPushButton *reloadModsListButton = new QPushButton("Reload Mods List", centralWidget);
    connect(reloadModsListButton, &QPushButton::clicked, this, &MainWindow::reloadModsList);

    // Create the "Import AnimationParam.bgyml" button
    QPushButton *importBgymlButton = new QPushButton("Import AnimationParam.bgyml", centralWidget);
    connect(importBgymlButton, &QPushButton::clicked, this, &MainWindow::importBgyml);

    // Create the "Generate Patch" button
    QPushButton *generatePatchButton = new QPushButton("Generate Patch", centralWidget);
    connect(generatePatchButton, &QPushButton::clicked, this, &MainWindow::generatePatch);

    // Create move up and move down buttons as arrows
    moveUpButton = new QPushButton("↑", centralWidget);
    moveUpButton->setMaximumWidth(20);
    connect(moveUpButton, &QPushButton::clicked, this, &MainWindow::moveModUp);

    moveDownButton = new QPushButton("↓", centralWidget);
    moveDownButton->setMaximumWidth(20);
    connect(moveDownButton, &QPushButton::clicked, this, &MainWindow::moveModDown);

    QHBoxLayout *moveButtonsLayout = new QHBoxLayout;
    moveButtonsLayout->addWidget(moveUpButton);
    moveButtonsLayout->addWidget(moveDownButton);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(reloadModsListButton);
    buttonLayout->addWidget(importBgymlButton);
    buttonLayout->addWidget(generatePatchButton);

    // Mods List Table
    modsListTable = new QTableWidget(centralWidget);
    modsListTable->setColumnCount(5); // Update from 4 to 5
    modsListTable->setHorizontalHeaderLabels({"ID", "Mod name", "Description", "Enable", "Delete"});
    modsListTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // ID
    modsListTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // Description
    modsListTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents); // Enable
    modsListTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents); // Delete
    modsListTable->verticalHeader()->setVisible(false);

    modsListLayout->addWidget(modsListTable);
    modsListLayout->addLayout(moveButtonsLayout);
    modsListLayout->addLayout(buttonLayout);
    modsListGroup->setLayout(modsListLayout);

    mainLayout->addWidget(modsListGroup);

    centralWidget->setLayout(mainLayout);

    this->setCentralWidget(centralWidget);

    // Check for the existence of "byml-to-yaml.exe" in the /scripts folder
    QString bymlToYamlPath = QDir(QCoreApplication::applicationDirPath()).filePath("scripts/byml-to-yaml.exe");
    if (!QFile::exists(bymlToYamlPath)) {
        // Display a warning prompt with a link to download "byml-to-yaml.exe"
        QMessageBox prompt;
        prompt.setIcon(QMessageBox::Warning);
        prompt.setText("byml-to-yaml.exe is missing in the /scripts folder.");
        prompt.setInformativeText("Please download it from GitHub by clicking the download button.");
        QPushButton *downloadButton = prompt.addButton("Download", QMessageBox::ActionRole);
        QPushButton *closeButton = prompt.addButton("Close", QMessageBox::ActionRole);
        prompt.exec();

        if (prompt.clickedButton() == downloadButton) {
            // Open the download link in the user's browser
            QDesktopServices::openUrl(QUrl("https://github.com/ArchLeaders/byml_to_yaml/releases/latest"));
        } else if (prompt.clickedButton() == closeButton) {
            // Close the application
            QApplication::exit(0);
        }
    }

    // Load mods list from the settings
    populateModsList();
}

void MainWindow::deleteMod()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button)
    {
        QString iniFileName = button->property("modIniFile").toString();
        if (!iniFileName.isEmpty())
        {
            QString modsFolderPath = QDir(QCoreApplication::applicationDirPath()).filePath("Mods");
            QFile iniFile(modsFolderPath + "/" + iniFileName);
            if (iniFile.remove())
            {
                // Extracting the mod number from the file name
                QRegularExpression re("(\\d+)");
                QRegularExpressionMatch match = re.match(iniFileName);
                if (match.hasMatch()) {
                    QString modNumber = match.captured(1);

                    // Begin a group operation for "ModEnabled"
                    appSettings->beginGroup("ModEnabled");

                    // Remove the specific mod setting
                    appSettings->remove(modNumber);

                    // Here we need to reindex the following mods if you want to keep the sequence continuous.
                    QStringList keys = appSettings->childKeys();
                    QMap<int, QVariant> modSettings;
                    for (const QString &key : keys) {
                        int currentKey = key.toInt();
                        if (currentKey > modNumber.toInt()) {
                            modSettings[currentKey - 1] = appSettings->value(key);
                            appSettings->remove(key);
                        }
                    }
                    for (auto it = modSettings.constBegin(); it != modSettings.constEnd(); ++it) {
                        // Here's the change: we use QString::asprintf() to ensure the key is always 3 digits
                        appSettings->setValue(QString::asprintf("%03d", it.key()), it.value());
                    }

                    // End the group operation
                    appSettings->endGroup();

                    // Synchronize settings to write them to the disk
                    appSettings->sync();

                    reloadModsList();
                }
            }
            else
            {
                QMessageBox::critical(this, "Error", "Failed to delete the .ini file.");
            }
        }
    }
}

void MainWindow::onModCheckBoxChanged(int state)
{
    QCheckBox* senderCheckBox = qobject_cast<QCheckBox*>(sender());
    if (!senderCheckBox)
        return;

    QString modID = senderCheckBox->property("modID").toString();

    if (state == Qt::Checked) {
        // If checked and not in the list, add it
        if (!m_enabledModIDs.contains(modID)) {
            m_enabledModIDs.append(modID);
        }
    } else {
        // If unchecked and in the list, remove it
        m_enabledModIDs.removeAll(modID);
    }

    // Save the new state to the settings
    appSettings->setValue("ModEnabled/" + modID, (state == Qt::Checked));
}

void MainWindow::populateModsList()
{
    // Clear the existing table contents, if any
    modsListTable->setRowCount(0);
    modsListTable->clearContents();

    // Path to the Mods folder located next to the executable
    QString modsFolderPath = QDir(QCoreApplication::applicationDirPath()).filePath("Mods");

    // Get a list of .ini files in the Mods folder
    QStringList iniFiles = QDir(modsFolderPath).entryList(QStringList("*.ini"), QDir::Files);

    int maxNameWidth = 0; // To store the maximum width needed for "Mod name" column

    // Loop through the .ini files and populate the table
    for (const QString &iniFileName : iniFiles)
    {
        // Get the ID from the first 3 characters of the file name
        QString modID = iniFileName.left(3); // Assuming the ID is always the first 3 characters

        // Read the .ini file to get Name and Description
        QSettings settings(modsFolderPath + "/" + iniFileName, QSettings::IniFormat);
        QString modName = settings.value("Name").toString();
        QString modDescription = settings.value("Description").toString();

        // Calculate the width needed for "Mod name" based on its content
        QFontMetrics fm(modsListTable->font());
        int nameWidth = fm.horizontalAdvance(modName);
        maxNameWidth = qMax(maxNameWidth, nameWidth);

        // Create a custom widget for the "Enable" column with a centered checkbox
        QWidget* enableWidget = new QWidget();
        QHBoxLayout* layout = new QHBoxLayout(enableWidget);

        // Load saved settings for the checkbox state
        bool isChecked = appSettings->value("ModEnabled/" + modID, false).toBool();
        QCheckBox* enableCheckBox = new QCheckBox();
        enableCheckBox->setChecked(isChecked); // set the state here, before adding to layout
        enableCheckBox->setProperty("modID", modID); // Set the modID property for the checkbox

        // Connect the checkbox's stateChanged signal to the onModCheckBoxChanged slot
        connect(enableCheckBox, &QCheckBox::stateChanged, this, &MainWindow::onModCheckBoxChanged);

        layout->addWidget(enableCheckBox); // add the checkbox to the layout after setting its state
        layout->setAlignment(Qt::AlignCenter);
        layout->setContentsMargins(0, 0, 0, 0); // Remove any default margins
        enableWidget->setLayout(layout);

        // Add a new row to the table
        int row = modsListTable->rowCount();
        modsListTable->insertRow(row);

        // Set data in the table cells
        modsListTable->setItem(row, 0, new QTableWidgetItem(modID));
        modsListTable->setItem(row, 1, new QTableWidgetItem(modName));
        modsListTable->setItem(row, 2, new QTableWidgetItem(modDescription));
        modsListTable->setCellWidget(row, 3, enableWidget);

        // Add delete button in the "Delete" column
        QPushButton *deleteButton = new QPushButton("X");
        deleteButton->setProperty("modIniFile", iniFileName);
        connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteMod);
        modsListTable->setCellWidget(row, 4, deleteButton);
    }

    // Set the "Mod name" column width to accommodate the longest name
    modsListTable->setColumnWidth(1, maxNameWidth);

    // Adjust column widths and row heights as needed
    modsListTable->resizeColumnsToContents();
    modsListTable->resizeRowsToContents();

    // Stretch the "Description" column (index 2) to fill the remaining space
    modsListTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    // Select items by rows
    modsListTable->setSelectionBehavior(QAbstractItemView::SelectRows);
}


void MainWindow::browseOutputModsFolder()
{
    QString selectedFolder = QFileDialog::getExistingDirectory(this, "Select Output Mods Folder");

    if (!selectedFolder.isEmpty())
    {
        // Set the selected folder path in the text box
        outputModsFolderLineEdit->setText(selectedFolder);

        // Save the selected folder path in settings
        appSettings->setValue("OutputModsFolder", selectedFolder);
        appSettings->sync(); // Optional: Forces immediate save to file
    }
}

void MainWindow::reloadModsList()
{
    // Get the directory path where the .ini files are located
    QString modsFolderPath = QDir(QCoreApplication::applicationDirPath()).filePath("Mods");

    // Get a list of .ini files in the Mods folder
    QStringList iniFiles = QDir(modsFolderPath).entryList(QStringList("*.ini"), QDir::Files);

    int idCounter = 1; // Counter for renaming files
    for (const QString &iniFileName : iniFiles)
    {
        QString newModID = QString::number(idCounter).rightJustified(3, '0'); // Format ID as 3 digits
        QString newFileName = newModID + ".ini";

        if (iniFileName != newFileName)
        {
            QString oldFilePath = modsFolderPath + "/" + iniFileName;
            QString newFilePath = modsFolderPath + "/" + newFileName;

            // Rename the file
            QFile::rename(oldFilePath, newFilePath);
        }

        idCounter++;
    }

    // Clear the existing rows in the mods list
    modsListTable->setRowCount(0);

    // Repopulate the mods list
    populateModsList();

    // After repopulating the list, reselect the row that was selected (if any)
    if (currentSelectedRow >= 0 && currentSelectedRow < modsListTable->rowCount())
    {
        modsListTable->selectRow(currentSelectedRow);
    }
}

void MainWindow::importBgyml()
{
    // Prompt the user to select a .bgyml file
    QString bgymlFilePath = QFileDialog::getOpenFileName(this, "Select AnimationParam.bgyml File", QDir::currentPath(), "AnimationParam.bgyml Files (*.bgyml)");

    if (!bgymlFilePath.isEmpty())
    {
        // Prompt the user to enter Mod Name and Description
        QString modName = QInputDialog::getText(this, "Enter Mod Name", "Mod Name:");
        QString modDescription = QInputDialog::getText(this, "Enter Mod Description", "Mod Description:");

        // Execute the PowerShell script with the selected .bgyml file and user inputs
        QString scriptPath = QDir(QCoreApplication::applicationDirPath()).filePath("scripts/ImportBGYML.ps1");
        QString command = "powershell.exe -ExecutionPolicy Bypass -File \"" + scriptPath + "\"";
        command += " -userInputFilePath \"" + bgymlFilePath + "\"";
        command += " -modName \"" + modName + "\"";
        command += " -modDescription \"" + modDescription + "\"";

        QProcess::startDetached(command);

        // Inform the user that the import process has completed
        QMessageBox::information(this, "Import Completed", "Import process has completed.");
    }
    reloadModsList();
}

struct TableRowContent
{
    QTableWidgetItem *idItem;
    QTableWidgetItem *nameItem;
    QTableWidgetItem *descriptionItem;
    QWidget *enableWidget;
    QPushButton *deleteButton;
};

// Helper function to clone the checkbox widget with its state
QWidget* cloneCheckboxWidget(QWidget* original) {
    QCheckBox* originalCheckBox = original->findChild<QCheckBox*>();
    QWidget* newWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(newWidget);
    QCheckBox* newCheckBox = new QCheckBox();
    newCheckBox->setChecked(originalCheckBox->isChecked()); // preserve the checked state
    layout->addWidget(newCheckBox);
    layout->setAlignment(Qt::AlignCenter);
    newWidget->setLayout(layout);
    return newWidget;
}

TableRowContent cloneRowContent(QTableWidget *table, int row)
{
    TableRowContent content;
    content.idItem = new QTableWidgetItem(*table->item(row, 0));
    content.nameItem = new QTableWidgetItem(*table->item(row, 1));
    content.descriptionItem = new QTableWidgetItem(*table->item(row, 2));

    // Use the new function to clone the checkbox
    content.enableWidget = cloneCheckboxWidget(table->cellWidget(row, 3));

    // Clone the delete button
    content.deleteButton = new QPushButton("X");
    QString iniFileName = qobject_cast<QPushButton*>(table->cellWidget(row, 4))->property("modIniFile").toString();
    content.deleteButton->setProperty("modIniFile", iniFileName);

    return content;
}

void MainWindow::moveMod(bool moveUp)
{
    int selectedRow = modsListTable->currentRow();
    int rowCount = modsListTable->rowCount();

    if (selectedRow >= 0 && rowCount > 1)
    {
        int targetRow = moveUp ? selectedRow - 1 : selectedRow + 1;
        if (targetRow >= 0 && targetRow < rowCount)
        {
            userInitiatedMoveAction = true; // Set the flag to true

            // Clone the rows
            TableRowContent currentRowContent = cloneRowContent(modsListTable, selectedRow);
            TableRowContent targetRowContent = cloneRowContent(modsListTable, targetRow);

            // Swap the row contents
            modsListTable->setItem(selectedRow, 0, targetRowContent.idItem->clone());
            modsListTable->setItem(selectedRow, 1, targetRowContent.nameItem->clone());
            modsListTable->setItem(selectedRow, 2, targetRowContent.descriptionItem->clone());
            modsListTable->setCellWidget(selectedRow, 3, targetRowContent.enableWidget);
            modsListTable->setCellWidget(selectedRow, 4, targetRowContent.deleteButton);

            modsListTable->setItem(targetRow, 0, currentRowContent.idItem->clone());
            modsListTable->setItem(targetRow, 1, currentRowContent.nameItem->clone());
            modsListTable->setItem(targetRow, 2, currentRowContent.descriptionItem->clone());
            modsListTable->setCellWidget(targetRow, 3, currentRowContent.enableWidget);
            modsListTable->setCellWidget(targetRow, 4, currentRowContent.deleteButton);

            // Reconnect the delete buttons to the deleteMod slot after they have been moved
            connect(currentRowContent.deleteButton, &QPushButton::clicked, this, &MainWindow::deleteMod);
            connect(targetRowContent.deleteButton, &QPushButton::clicked, this, &MainWindow::deleteMod);

            // Construct the keys for the settings based on your mod's ID format
            QString currentModKey = "ModEnabled/" + currentRowContent.idItem->text();
            QString targetModKey = "ModEnabled/" + targetRowContent.idItem->text();

            // Get the current 'enabled' state from the settings file
            bool currentModState = appSettings->value(currentModKey, false).toBool();
            bool targetModState = appSettings->value(targetModKey, false).toBool();

            // Swap the 'enabled' states in the settings file
            appSettings->setValue(currentModKey, targetModState);
            appSettings->setValue(targetModKey, currentModState);

            // Sync the settings to ensure they're written to the file
            appSettings->sync();

            // Rename the corresponding .ini files
            renameIniFiles(selectedRow, targetRow);

            modsListTable->setCurrentCell(targetRow, 0); // Set the new current cell
            userInitiatedMoveAction = false; // Reset the flag after the move is done

            // Update the current selected row and possibly other UI elements here
            currentSelectedRow = modsListTable->currentRow();
            modsListTable->setCurrentCell(targetRow, 0, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
            modsListTable->setFocus();
        }
    }

    // Reload the mods list after all operations are done
    reloadModsList();
}

void MainWindow::moveModUp()
{
    moveMod(true);
}

void MainWindow::moveModDown()
{
    moveMod(false);
}

// Add this helper function to clone a checkbox with its state
QWidget* MainWindow::cloneCheckboxWidget(QWidget* original)
{
    QCheckBox* originalCheckBox = original->findChild<QCheckBox*>();
    QWidget* newWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(newWidget);
    QCheckBox* newCheckBox = new QCheckBox();
    newCheckBox->setChecked(originalCheckBox->isChecked()); // Preserve the checked state
    layout->addWidget(newCheckBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    newWidget->setLayout(layout);
    return newWidget;
}

void MainWindow::renameIniFiles(int fromRow, int toRow)
{
    QPushButton *fromDeleteButton = qobject_cast<QPushButton*>(modsListTable->cellWidget(fromRow, 4)); // Assuming 4 is the "Delete" column
    QPushButton *toDeleteButton = qobject_cast<QPushButton*>(modsListTable->cellWidget(toRow, 4));

    if (!fromDeleteButton || !toDeleteButton)
    {
        qDebug() << "Invalid delete buttons.";
        return;
    }

    QString fromFileName = fromDeleteButton->property("modIniFile").toString();
    QString toFileName = toDeleteButton->property("modIniFile").toString();

    if (fromFileName.isEmpty() || toFileName.isEmpty())
    {
        qDebug() << "File names are empty.";
        return;
    }

    QString modsFolderPath = QDir(QCoreApplication::applicationDirPath()).filePath("Mods");

    QString fromFilePath = QDir(modsFolderPath).filePath(fromFileName);
    QString toFilePath = QDir(modsFolderPath).filePath(toFileName);

    // Temporary renaming to avoid file name conflicts
    QString tempFileName = "TEMP_" + fromFileName;
    QString tempFilePath = QDir(modsFolderPath).filePath(tempFileName);

    if (QFile::rename(fromFilePath, tempFilePath))
    {
        if (QFile::rename(toFilePath, fromFilePath))
        {
            if (!QFile::rename(tempFilePath, toFilePath))
            {
                qDebug() << "Failed to rename temporary to:" << toFilePath;
            }
        }
        else
        {
            qDebug() << "Failed to rename:" << toFilePath << "to" << fromFilePath;
        }
    }
    else
    {
        qDebug() << "Failed to rename:" << fromFilePath << "to temporary";
    }
}


void MainWindow::generatePatch()
{
    // Check if the "Output Mods Folder" is empty
    QString outputModsFolderPath = outputModsFolderLineEdit->text().trimmed();
    if (outputModsFolderPath.isEmpty())
    {
        QMessageBox::critical(this, "Error", "Output Mods Folder path is empty. Please select a folder before generating the patch.");
        return;
    }

    // Get the selected mod IDs where "Enable" is checked
    QStringList selectedModIDs;
    for (int row = 0; row < modsListTable->rowCount(); ++row)
    {
        // Retrieve the custom widget from the "Enable" column
        QWidget *enableWidget = modsListTable->cellWidget(row, 3); // Assuming 3 is the "Enable" column
        if (enableWidget)
        {
            // Retrieve the checkbox from the custom widget
            QCheckBox *checkBox = enableWidget->findChild<QCheckBox*>();
            if (checkBox && checkBox->isChecked())
            {
                // Retrieve the mod ID, ensuring to remove leading zeros
                QString modID = modsListTable->item(row, 0)->text();
                int modIDAsInt = modID.toInt(); // Convert to int to remove any leading '0's
                QString modIDWithoutLeadingZeros = QString::number(modIDAsInt);

                selectedModIDs << modIDWithoutLeadingZeros;

                // Save the state of the checkbox in settings
                appSettings->setValue("ModEnabled/" + modID, true);
            }
            else if (checkBox && !checkBox->isChecked()) // If checkbox is unchecked
            {
                QString modID = modsListTable->item(row, 0)->text();

                // Save the state of the checkbox in settings
                appSettings->setValue("ModEnabled/" + modID, false);
            }
        }
    }

    // Save settings to file
    appSettings->sync();

    if (selectedModIDs.isEmpty())
    {
        QMessageBox::warning(this, "No Mods Selected", "No mods are enabled. Please enable at least one mod before generating the patch.");
        return;
    }

    // Path to the PowerShell script
    QString scriptPath = QDir(QCoreApplication::applicationDirPath()).filePath("scripts/GeneratePatch.ps1");

    // Prepare the command to run PowerShell script
    QString command = "powershell.exe -ExecutionPolicy Bypass -File \"" + scriptPath + "\"";

    // Add the modIDs and modPath as arguments
    command += " -modIDs \"" + selectedModIDs.join(",") + "\"";
    command += " -modPath \"" + outputModsFolderPath + "\"";

    // Execute the PowerShell script and capture the output
    QProcess process;
    process.start(command);
    process.waitForFinished(-1); // Wait indefinitely until the process finishes

    // Get the output from the process
    QString output = process.readAllStandardOutput();
    QString errorOutput = process.readAllStandardError();

    // Check if there was an error in the script execution
    if (!errorOutput.isEmpty())
    {
        QMessageBox::critical(this, "Error", "Failed to generate patch. Error: " + errorOutput);
        return;
    }

    // Check if the output contains "Command executed successfully"
    if (output.contains("Command executed successfully"))
    {
        QMessageBox::information(this, "Success", "Patch generated successfully in your output mods folder.");
    }
    else
    {
        // Display an error message if the success message is not found
        QMessageBox::critical(this, "Error", "Failed to generate patch. Unexpected output: " + output);
    }
}

MainWindow::~MainWindow()
{
    // Destructor - anything to clean up can be added here
    delete appSettings;
}
