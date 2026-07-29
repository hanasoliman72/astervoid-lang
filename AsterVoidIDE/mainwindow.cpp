#include "mainwindow.h"
#include "codeeditor.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QPainter>
#include <QRandomGenerator>
#include <QMenuBar>
#include <QToolBar>
#include <QMessageBox>     // ← Make sure this is here!
#include <QScrollBar>
#include <QFileInfo>       // ← And this!
#include <QDir>            // ← And this!
#include <cmath>


// LogoWidget Implementation
LogoWidget::LogoWidget(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(120);
    setMaximumHeight(120);

    animationTimer = new QTimer(this);
    animationPhase = 0.0f;
    connect(animationTimer, &QTimer::timeout, this, [this]() {
        animationPhase += 0.05f;
        if (animationPhase > 6.28f) animationPhase = 0.0f;
        update();
    });
    animationTimer->start(50);
}

void LogoWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int centerX = width() / 2;
    int centerY = height() / 2 - 10;

    // Draw orbiting particles
    for (int i = 0; i < 8; i++) {
        float angle = (i * 6.28f / 8.0f) + animationPhase;
        float radius = 35.0f;
        int x = centerX + cos(angle) * radius;
        int y = centerY + sin(angle) * radius;

        QColor particleColor;
        particleColor.setHsv((i * 45 + (int)(animationPhase * 50)) % 360, 200, 255);

        painter.setPen(Qt::NoPen);
        painter.setBrush(particleColor);
        painter.drawEllipse(QPointF(x, y), 4, 4);

        QRadialGradient glow(x, y, 8);
        glow.setColorAt(0, QColor(particleColor.red(), particleColor.green(), particleColor.blue(), 100));
        glow.setColorAt(1, QColor(particleColor.red(), particleColor.green(), particleColor.blue(), 0));
        painter.setBrush(glow);
        painter.drawEllipse(QPointF(x, y), 8, 8);
    }

    // Central star with glow
    QRadialGradient starGlow(centerX, centerY, 25);
    starGlow.setColorAt(0, QColor(0, 212, 255, 255));
    starGlow.setColorAt(0.5, QColor(107, 79, 181, 200));
    starGlow.setColorAt(1, QColor(107, 79, 181, 0));
    painter.setBrush(starGlow);
    painter.drawEllipse(QPointF(centerX, centerY), 25, 25);

    painter.setBrush(QColor(255, 255, 255));
    painter.drawEllipse(QPointF(centerX, centerY), 8, 8);

    // Star rays
    painter.setPen(QPen(QColor(255, 255, 255, 200), 2));
    for (int i = 0; i < 6; i++) {
        float angle = i * 6.28f / 6.0f + animationPhase * 0.5f;
        int x1 = centerX + cos(angle) * 8;
        int y1 = centerY + sin(angle) * 8;
        int x2 = centerX + cos(angle) * 18;
        int y2 = centerY + sin(angle) * 18;
        painter.drawLine(x1, y1, x2, y2);
    }

    // Text
    QFont font("Arial", 18, QFont::Bold);
    painter.setFont(font);
    painter.setPen(QColor(0, 10, 200));
    painter.drawText(rect().adjusted(0, 70, 0, 0), Qt::AlignCenter, "AsterVoid");

    QFont subtitleFont("Courier New", 8, QFont::Bold);
    painter.setFont(subtitleFont);
    painter.setPen(QColor(139, 92, 246, 180));
    painter.drawText(rect().adjusted(0, 105, 0, 0), Qt::AlignCenter, "\"Born from the void, made for the stars\"");
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , compilerProcess(nullptr)
    , showingWelcome(false)      // Move this BEFORE currentFilePath
    , currentFilePath("")
{
    qDebug() << "1. Constructor started";

    // Initialize stars
    for (int i = 0; i < 150; i++) {
        float x = QRandomGenerator::global()->bounded(1000) / 1000.0f;
        float y = QRandomGenerator::global()->bounded(1000) / 1000.0f;
        stars.append(QPointF(x, y));
        starOpacity.append(QRandomGenerator::global()->bounded(30, 100) / 100.0f);
    }

    setupUI();
    setupStyles();
    setupMenuBar();
    setupStatusBar();        // NEW!
    enhanceButtons();        // NEW!
    connectSignals();

    compilerProcess = new QProcess(this);

    // Star animation
    starTimer = new QTimer(this);
    connect(starTimer, &QTimer::timeout, this, [this]() {
        for (int i = 0; i < starOpacity.size(); i++) {
            starOpacity[i] += (QRandomGenerator::global()->bounded(20) - 10) / 100.0f;
            if (starOpacity[i] > 1.0f) starOpacity[i] = 1.0f;
            if (starOpacity[i] < 0.2f) starOpacity[i] = 0.2f;
        }
        update();
    });
    starTimer->start(100);

    resize(1400, 900);
    setWindowTitle("⭐ AsterVoid IDE");
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateLineNumbers()
{
    if (!codeEditor || !lineNumbers) return;

    int lineCount = codeEditor->document()->blockCount();
    QString lineNumberText;
    for (int i = 1; i <= lineCount; ++i) {
        lineNumberText += QString::number(i) + "\n";
    }
    lineNumbers->setPlainText(lineNumberText);
}

void MainWindow::printToTerminal(const QString &text, const QString &type)
{
    if (!terminal) return;

    QString coloredText;
    if (type == "success") {
        coloredText = "✅ " + text;
    } else if (type == "error") {
        coloredText = "❌ " + text;
    } else if (type == "warning") {
        coloredText = "⚠️ " + text;
    } else if (type == "info") {
        coloredText = "ℹ️ " + text;
    } else {
        coloredText = text;
    }

    terminal->append(coloredText);
}

void MainWindow::createWelcomeScreen()
{
    // Empty for now - optional feature
}


void MainWindow::setupUI()
{
    qDebug() << "setupUI: Starting...";

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    qDebug() << "setupUI: Central widget created";

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    qDebug() << "setupUI: Main layout created";

    // Main splitter
    mainSplitter = new QSplitter(Qt::Horizontal, this);

    qDebug() << "setupUI: Creating sidebar...";
    setupSidebar();

    qDebug() << "setupUI: Creating toolbar...";
    setupToolbar();

    qDebug() << "setupUI: Creating editor...";
    setupEditor();

    qDebug() << "setupUI: Creating terminal...";
    setupTerminal();

    qDebug() << "setupUI: Assembling layout...";

    // Right side with editor and terminal
    QWidget *rightWidget = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    rightLayout->addWidget(toolbarWidget);

    // Vertical splitter for editor and terminal
    verticalSplitter = new QSplitter(Qt::Vertical);

    // Editor container with line numbers
    QWidget *editorContainer = new QWidget();
    QHBoxLayout *editorLayout = new QHBoxLayout(editorContainer);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    editorLayout->setSpacing(0);

    // Create line numbers widget
    lineNumbers = new QTextEdit();
    lineNumbers->setReadOnly(true);
    lineNumbers->setMaximumWidth(50);
    lineNumbers->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    lineNumbers->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    qDebug() << "setupUI: Line numbers created";

    editorLayout->addWidget(lineNumbers);
    editorLayout->addWidget(codeEditor);

    verticalSplitter->addWidget(editorContainer);
    verticalSplitter->addWidget(terminal);
    verticalSplitter->setStretchFactor(0, 3);
    verticalSplitter->setStretchFactor(1, 1);

    rightLayout->addWidget(verticalSplitter);

    mainSplitter->addWidget(sidebarWidget);
    mainSplitter->addWidget(rightWidget);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 4);

    mainLayout->addWidget(mainSplitter);

    qDebug() << "setupUI: Complete!";
}

void MainWindow::setupSidebar()
{
    sidebarWidget = new QWidget();
    sidebarWidget->setMinimumWidth(250);
    sidebarWidget->setMaximumWidth(350);

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebarWidget);

    // Animated logo
    LogoWidget *logo = new LogoWidget(sidebarWidget);

    // File Explorer
    fileExplorer = new QTreeWidget();
    fileExplorer->setHeaderLabel("📁 Project Files");
    fileExplorer->setIndentation(15);

    // Sample files with .astv extension
    QTreeWidgetItem *file1 = new QTreeWidgetItem(fileExplorer);
    file1->setText(0, "🌍 milkyway.astv");
    file1->setData(0, Qt::UserRole, "milkyway.astv");

    sidebarLayout->addWidget(logo);
    sidebarLayout->addWidget(fileExplorer);
}

void MainWindow::setupToolbar()
{
    toolbarWidget = new QWidget();
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbarWidget);
    toolbarLayout->setContentsMargins(10, 10, 10, 10);

    btnNew = new QPushButton("📄 New");
    btnOpen = new QPushButton("📂 Open");
    btnSave = new QPushButton("💾 Save");
    btnRun = new QPushButton("🚀 Launch");

    toolbarLayout->addWidget(btnNew);
    toolbarLayout->addWidget(btnOpen);
    toolbarLayout->addWidget(btnSave);
    toolbarLayout->addWidget(btnRun);
    toolbarLayout->addStretch();
}

void MainWindow::setupEditor()
{
    qDebug() << "setupEditor: Creating code editor...";

    codeEditor = new CodeEditor();

    qDebug() << "setupEditor: Setting default code...";

    // Set default code
    QString defaultCode =
        "** Welcome to AsterVoid IDE!\n"
        "#launch <bigbang>\n"
        "\n"
        "vacuum milkyway() {\n"
        "    shine(\"Hello, Galaxy!\\n\");\n"
        "    \n"
        "    mass count = 0;\n"
        "    orbit (count < 5) {\n"
        "        shine(\"Star \", count, \"\\n\");\n"
        "        count++;\n"
        "    }\n"
        "    \n"
        "    blackHole 0;\n"
        "}";

    codeEditor->setPlainText(defaultCode);

    qDebug() << "setupEditor: Complete!";
}

void MainWindow::setupTerminal()
{
    qDebug() << "setupTerminal: Creating terminal...";

    terminal = new QTextEdit();
    terminal->setReadOnly(true);
    terminal->setText("⚡ OUTPUT TERMINAL\n\nReady to launch...");

    qDebug() << "setupTerminal: Complete!";
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *fileMenu = menuBar->addMenu("File");
    fileMenu->addAction("New", this, &MainWindow::onNewFileClicked);
    fileMenu->addAction("Open", this, &MainWindow::onOpenClicked);
    fileMenu->addAction("Save", this, &MainWindow::onSaveClicked);
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, &QWidget::close);

    QMenu *runMenu = menuBar->addMenu("Run");
    runMenu->addAction("Launch Program", this, &MainWindow::onRunClicked);
}

void MainWindow::setupStyles()
{
    // Main window background
    setStyleSheet(
        "QMainWindow {"
        "  background-color: #0a0e27;"
        "}"
        );

    // Sidebar style
    sidebarWidget->setStyleSheet(
        "QWidget {"
        "  background-color: rgba(26, 29, 46, 230);"
        "  border-right: 1px solid rgba(107, 79, 181, 0.3);"
        "}"
        "QLabel {"
        "  color: #00d4ff;"
        "  margin: 20px;"
        "}"
        "QTreeWidget {"
        "  background-color: rgba(26, 29, 46, 200);"
        "  color: #e0e0e0;"
        "  border: none;"
        "  font-family: 'Courier New';"
        "  font-size: 12px;"
        "}"
        "QTreeWidget::item {"
        "  padding: 8px;"
        "  border-left: 3px solid transparent;"
        "}"
        "QTreeWidget::item:hover {"
        "  background-color: rgba(107, 79, 181, 0.3);"
        "  border-left: 3px solid #00d4ff;"
        "}"
        "QTreeWidget::item:selected {"
        "  background-color: rgba(107, 79, 181, 0.5);"
        "}"
        );

    // Toolbar style
    toolbarWidget->setStyleSheet(
        "QWidget {"
        "  background-color: rgba(26, 29, 46, 230);"
        "  border-bottom: 1px solid rgba(107, 79, 181, 0.3);"
        "}"
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "              stop:0 #6b4fb5, stop:1 #8b5cf6);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 5px;"
        "  padding: 8px 20px;"
        "  font-family: 'Courier New';"
        "  font-size: 11px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "              stop:0 #7b5fc5, stop:1 #9b6cff);"
        "}"
        "QPushButton:pressed {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "              stop:0 #5b4fa5, stop:1 #7b4ce6);"
        "}"
        );

    // Special style for Launch button
    btnRun->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "              stop:0 #00d4ff, stop:1 #4facfe);"
        "  color: white;"
        "  border: none;"
        "  border-radius: 5px;"
        "  padding: 8px 20px;"
        "  font-family: 'Courier New';"
        "  font-size: 11px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "              stop:0 #10e4ff, stop:1 #5fbcff);"
        "}"
        );

    // Code editor style
    codeEditor->setStyleSheet(
        "QTextEdit {"
        "  background-color: rgba(10, 14, 39, 240);"
        "  color: #e0e0e0;"
        "  border: none;"
        "  font-family: 'Courier New';"
        "  font-size: 13px;"
        "  selection-background-color: rgba(107, 79, 181, 0.5);"
        "}"
        );

    // Line numbers style
    lineNumbers->setStyleSheet(
        "QTextEdit {"
        "  background-color: rgba(10, 14, 39, 240);"
        "  color: rgba(224, 224, 224, 0.3);"
        "  border: none;"
        "  border-right: 1px solid rgba(107, 79, 181, 0.2);"
        "  font-family: 'Courier New';"
        "  font-size: 13px;"
        "  padding-right: 5px;"
        "}"
        );

    // Terminal style
    terminal->setStyleSheet(
        "QTextEdit {"
        "  background-color: rgba(10, 14, 39, 240);"
        "  color: #00ff88;"
        "  border: none;"
        "  border-top: 1px solid rgba(0, 255, 136, 0.3);"
        "  font-family: 'Courier New';"
        "  font-size: 12px;"
        "}"
        );
}

void MainWindow::connectSignals()
{
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRunClicked);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::onOpenClicked);
    connect(btnNew, &QPushButton::clicked, this, &MainWindow::onNewFileClicked);
    connect(codeEditor, &QTextEdit::textChanged, this, &MainWindow::updateLineNumbers);
    connect(codeEditor, &QTextEdit::cursorPositionChanged, this, &MainWindow::updateCursorPosition);  // NEW!
    connect(fileExplorer, &QTreeWidget::itemClicked, this, &MainWindow::onFileItemClicked);

    connect(codeEditor->verticalScrollBar(), &QScrollBar::valueChanged,
            lineNumbers->verticalScrollBar(), &QScrollBar::setValue);

    updateLineNumbers();
    updateCursorPosition();  // NEW!
}

void MainWindow::onRunClicked()
{
    if (!terminal) {
        QMessageBox::warning(this, "Error", "Terminal not initialized!");
        return;
    }
    terminal->clear();
    terminal->append("🚀 Launching AsterVoid program...\n");
    if (!codeEditor) {
        terminal->append("❌ Error: Code editor not found!");
        return;
    }

    // Create temp directory if it doesn't exist
    QDir tempDir("C:/temp");
    if (!tempDir.exists()) {
        tempDir.mkpath(".");
    }

    // Save to a fixed location
    QString tempFile = "C:/temp/temp_code.astv";
    QFile file(tempFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        terminal->append("❌ Error: Cannot create temp file!");
        return;
    }
    QTextStream out(&file);
    out << codeEditor->toPlainText();
    file.close();
    terminal->append("📝 Code saved to: " + tempFile + "\n");

    // Compiler path
    QString compilerPath = "D:/ClionProjects/Compiler/cmake-build-debug/Compiler.exe";

    // Check if compiler exists
    QFileInfo compilerInfo(compilerPath);
    if (!compilerInfo.exists()) {
        terminal->append("❌ Error: Compiler not found at:\n" + compilerPath);
        return;
    }
    terminal->append("🔧 Using compiler: " + compilerPath + "\n");
    terminal->append("📄 Input file: " + tempFile + "\n");

    // Set working directory to where the file is
    QString workDir = "C:/temp";
    terminal->append("📂 Working directory: " + workDir + "\n");
    terminal->append("⏳ Running...\n\n");

    if (!compilerProcess) {
        compilerProcess = new QProcess(this);
    }
    compilerProcess->disconnect();

    // Set working directory
    compilerProcess->setWorkingDirectory(workDir);

    // Connect outputs
    connect(compilerProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        QString output = compilerProcess->readAllStandardOutput();
        if (terminal) terminal->append(output);
    });
    connect(compilerProcess, &QProcess::readyReadStandardError, this, [this]() {
        QString error = compilerProcess->readAllStandardError();
        if (terminal) terminal->append(error);  // Show debug messages
    });
    connect(compilerProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
                if (!terminal) return;
                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    terminal->append("\n✅ Program completed successfully!");
                } else if (exitStatus == QProcess::CrashExit) {
                    terminal->append("\n❌ Compiler crashed!");
                } else {
                    terminal->append("\n❌ Exit code: " + QString::number(exitCode));
                }
            });

    // Create arguments list
    QStringList arguments;
    arguments << tempFile;  // Pass the full path to temp file

    // terminal->append("DEBUG: Starting process...\n");
    // terminal->append("DEBUG: Program: " + compilerPath + "\n");
    // terminal->append("DEBUG: Arg[0]: " + tempFile + "\n\n");

    // Start the compiler with arguments
    compilerProcess->start(compilerPath, arguments);

    if (!compilerProcess->waitForStarted(3000)) {
        terminal->append("❌ Failed to start compiler!\n");
        terminal->append("Error: " + compilerProcess->errorString());
    }

    if (statusLabel) {
        statusLabel->setText("  Compiling...");
    }
}

void MainWindow::onSaveClicked()
{
    QString fileName = currentFilePath;
    if (fileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "Save File", "", "AsterVoid Files (*.astv);;All Files (*)");
    }

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << codeEditor->toPlainText();
            file.close();
            currentFilePath = fileName;
            setWindowTitle("⭐ AsterVoid IDE - " + QFileInfo(fileName).fileName());
            terminal->append("💾 File saved: " + fileName);
        }
    }
}

void MainWindow::onOpenClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "AsterVoid Files (*.astv);;All Files (*)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            codeEditor->setPlainText(in.readAll());
            file.close();
            currentFilePath = fileName;
            setWindowTitle("⭐ AsterVoid IDE - " + QFileInfo(fileName).fileName());
            terminal->append("📂 File opened: " + fileName);
        }
    }
}

void MainWindow::onNewFileClicked()
{
    codeEditor->clear();
    currentFilePath.clear();
    setWindowTitle("⭐ AsterVoid IDE - New File");
    terminal->append("📄 New file created");
}

void MainWindow::onFileItemClicked(QTreeWidgetItem *item , int)
{
    QString fileName = item->data(0, Qt::UserRole).toString();
    terminal->append("Opening: " + fileName);
    // Here you would load the file
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw starfield
    for (int i = 0; i < stars.size(); i++) {
        QPointF pos = stars[i];
        int x = pos.x() * width();
        int y = pos.y() * height();

        painter.setPen(Qt::NoPen);
        QColor starColor(255, 255, 255, starOpacity[i] * 255);
        painter.setBrush(starColor);

        int size = QRandomGenerator::global()->bounded(1, 3);
        painter.drawEllipse(QPointF(x, y), size, size);
    }
}

void MainWindow::setupStatusBar()
{
    QStatusBar *status = statusBar();

    statusLabel = new QLabel("  Ready to launch");
    statusLabel->setStyleSheet("color: #00ff88; font-family: 'Courier New';");

    lineColLabel = new QLabel("Line: 1, Col: 1  ");
    lineColLabel->setStyleSheet("color: #00d4ff; font-family: 'Courier New';");

    fileTypeLabel = new QLabel("AsterVoid (.astv)  ");
    fileTypeLabel->setStyleSheet("color: #8b5cf6; font-family: 'Courier New';");

    status->addWidget(statusLabel, 1);
    status->addPermanentWidget(fileTypeLabel);
    status->addPermanentWidget(lineColLabel);

    status->setStyleSheet(
        "QStatusBar {"
        "  background-color: rgba(26, 29, 46, 230);"
        "  border-top: 1px solid rgba(107, 79, 181, 0.3);"
        "}"
        );
}

void MainWindow::updateCursorPosition()
{
    if (!codeEditor || !lineColLabel) return;

    QTextCursor cursor = codeEditor->textCursor();
    int line = cursor.blockNumber() + 1;
    int col = cursor.columnNumber() + 1;

    lineColLabel->setText(QString("Line: %1, Col: %2  ").arg(line).arg(col));
}

void MainWindow::enhanceButtons()
{
    btnNew->setToolTip("Create new file (Ctrl+N)");
    btnOpen->setToolTip("Open file (Ctrl+O)");
    btnSave->setToolTip("Save file (Ctrl+S)");
    btnRun->setToolTip("Compile and run (F5)");

    btnNew->setShortcut(QKeySequence::New);
    btnOpen->setShortcut(QKeySequence::Open);
    btnSave->setShortcut(QKeySequence::Save);
    btnRun->setShortcut(Qt::Key_F5);
}

void MainWindow::loadExampleCode()
{
    QString example =
        "** AsterVoid Example: Prime Checker\n"
        "#launch <bigbang>\n"
        "\n"
        "truth isPrime(mass n) {\n"
        "    phase (n <= 1) blackHole voidness;\n"
        "    mass i = 2;\n"
        "    orbit (i * i <= n) {\n"
        "        phase (n % i == 0) blackHole voidness;\n"
        "        i++;\n"
        "    }\n"
        "    blackHole starlight;\n"
        "}\n"
        "\n"
        "vacuum milkyway() {\n"
        "    shine(\"Enter number: \");\n"
        "    mass num;\n"
        "    moon(num);\n"
        "    phase (isPrime(num)) {\n"
        "        shine(num, \" is prime!\\n\");\n"
        "    } eclipse {\n"
        "        shine(num, \" is not prime.\\n\");\n"
        "    }\n"
        "    blackHole 0;\n"
        "}";

    codeEditor->setPlainText(example);
    if (statusLabel) statusLabel->setText("  Example loaded");
}
