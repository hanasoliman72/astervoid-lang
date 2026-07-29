#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTreeWidget>
#include <QPushButton>
#include <QSplitter>
#include <QTimer>
#include <QProcess>
#include <QLabel>
#include <QStatusBar>
#include <QPainter>
#include <cmath>

class StarfieldWidget;
class CodeEditor;

// Animated Logo Widget
class LogoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LogoWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QTimer *animationTimer;
    float animationPhase;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onRunClicked();
    void onSaveClicked();
    void onOpenClicked();
    void onNewFileClicked();
    void updateLineNumbers();
    void updateCursorPosition();
    void onFileItemClicked(QTreeWidgetItem *item, int column);

private:
    void setupUI();
    void setupStyles();
    void setupMenuBar();
    void setupToolbar();
    void setupSidebar();
    void setupEditor();
    void setupTerminal();
    void setupStatusBar();
    void createWelcomeScreen();
    void enhanceButtons();
    void connectSignals();
    void loadExampleCode();
    void printToTerminal(const QString &text, const QString &type = "info");

    // UI Components
    QWidget *centralWidget;
    QSplitter *mainSplitter;
    QSplitter *verticalSplitter;

    // Sidebar
    QWidget *sidebarWidget;
    QTreeWidget *fileExplorer;

    // Toolbar
    QWidget *toolbarWidget;
    QPushButton *btnRun;
    QPushButton *btnSave;
    QPushButton *btnOpen;
    QPushButton *btnNew;

    // Editor
    CodeEditor *codeEditor;
    QTextEdit *lineNumbers;

    // Terminal
    QTextEdit *terminal;

    // Status bar
    QLabel *statusLabel;
    QLabel *lineColLabel;
    QLabel *fileTypeLabel;

    // Welcome screen
    QWidget *welcomeScreen;
    bool showingWelcome;

    // Starfield
    QTimer *starTimer;
    QList<QPointF> stars;
    QList<float> starOpacity;

    // Compiler
    QProcess *compilerProcess;
    QString currentFilePath;

    // Styling
    QString sidebarStyle;
    QString buttonStyle;
    QString editorStyle;
    QString terminalStyle;
};

#endif // MAINWINDOW_H
