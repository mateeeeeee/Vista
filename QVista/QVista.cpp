#include "QVista.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include "Win32Util.h"

QVista::QVista(QWidget* parent) : QMainWindow(parent), processId(0)
{
	setWindowIcon(QIcon(":/icons/Data/Logos/Vista.ico"));
	setWindowTitle(tr("Vista"));
	setMinimumSize(600, 300);

	SetupUI();
	SetupConnections();
}

QVista::~QVista()
{
	if (processHandle)
	{
		CloseHandle(processHandle);
		processHandle = nullptr;
	}
}

void QVista::SetupUI()
{
	QWidget* centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);

	mainLayout = new QVBoxLayout(centralWidget);

	programGroupBox = new QGroupBox(tr("Program"), centralWidget);
	mainLayout->addWidget(programGroupBox);

	programLayout = new QFormLayout(programGroupBox);
	programGroupBox->setLayout(programLayout);

	executablePathLabel = new QLabel(tr("Executable Path:"), programGroupBox);
	executablePathEdit = new QLineEdit(programGroupBox);
	browseExeButton = new QPushButton(tr("..."), programGroupBox);
	browseExeButton->setFixedWidth(30);

	QHBoxLayout* exePathLayout = new QHBoxLayout();
	exePathLayout->addWidget(executablePathEdit);
	exePathLayout->addWidget(browseExeButton);
	exePathLayout->setContentsMargins(0, 0, 0, 0);

	programLayout->addRow(executablePathLabel, exePathLayout);

	workingDirectoryLabel = new QLabel(tr("Working Directory:"), programGroupBox);
	workingDirectoryEdit = new QLineEdit(programGroupBox);
	browseDirButton = new QPushButton(tr("..."), programGroupBox);
	browseDirButton->setFixedWidth(30);

	QHBoxLayout* workDirLayout = new QHBoxLayout();
	workDirLayout->addWidget(workingDirectoryEdit);
	workDirLayout->addWidget(browseDirButton);
	workDirLayout->setContentsMargins(0, 0, 0, 0);

	programLayout->addRow(workingDirectoryLabel, workDirLayout);

	commandArgsLabel = new QLabel(tr("Command-Line Arguments:"), programGroupBox);
	commandArgsEdit = new QLineEdit(programGroupBox);

	programLayout->addRow(commandArgsLabel, commandArgsEdit);

	mainLayout->addStretch();

	launchButton = new QPushButton(tr("Launch"), centralWidget);
	launchButton->setFixedWidth(100);
	terminateButton = new QPushButton(tr("Terminate"), centralWidget); 
	terminateButton->setFixedWidth(100);
	terminateButton->setEnabled(false); 

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(launchButton);
	buttonLayout->addWidget(terminateButton); 
	mainLayout->addLayout(buttonLayout);

	executablePathEdit->setText("C:\\Users\\Mate\\Desktop\\Projekti\\Adria\\build\\bin\\x64\\Profile\\Adria.exe");
	workingDirectoryEdit->setText("C:\\Users\\Mate\\Desktop\\Projekti\\Adria\\Adria\\");
}

void QVista::SetupConnections()
{
	connect(browseExeButton, &QPushButton::clicked, this, &QVista::on_browseExeButton_clicked);
	connect(browseDirButton, &QPushButton::clicked, this, &QVista::on_browseDirButton_clicked);
	connect(launchButton, &QPushButton::clicked, this, &QVista::on_launchButton_clicked);
	connect(terminateButton, &QPushButton::clicked, this, &QVista::on_terminateButton_clicked); 
}

void QVista::on_browseExeButton_clicked()
{
	QString filePath = QFileDialog::getOpenFileName(this,
		tr("Select Executable"),
		"",
		tr("Executables (*.exe);;All Files (*.*)"));

	if (!filePath.isEmpty())
	{
		executablePathEdit->setText(filePath);
		QDir fileDir = QFileInfo(filePath).dir();
		workingDirectoryEdit->setText(fileDir.path());
	}
}

void QVista::on_browseDirButton_clicked()
{
	QString dirPath = QFileDialog::getExistingDirectory(this,
		tr("Select Working Directory"),
		"");

	if (!dirPath.isEmpty())
	{
		workingDirectoryEdit->setText(dirPath);
	}
}

void QVista::on_launchButton_clicked()
{
	QString exePath = executablePathEdit->text();
	QString workingDir = workingDirectoryEdit->text();
	QString cmdArgs = commandArgsEdit->text();
	QString dllName = "Vista.dll";

	if (exePath.isEmpty())
	{
		QMessageBox::warning(this, tr("Input Required"), tr("Please specify the executable path."));
		return;
	}

	std::wstring exePathW = exePath.toStdWString();
	std::wstring workingDirW = workingDir.toStdWString();
	std::wstring argsW = cmdArgs.toStdWString();
	std::wstring errorMessage;

	PROCESS_INFORMATION processInfo = {};
	if (!win32util::CreateSuspendedProcess(exePathW, workingDirW, argsW, processInfo, errorMessage))
	{
		QMessageBox::critical(this, tr("Launch Failed"), QString::fromStdWString(errorMessage));
		return;
	}

	QString dllFullPath = QCoreApplication::applicationDirPath() + "/" + dllName;
	std::wstring dllFullPathW = dllFullPath.toStdWString();

	auto result = win32util::InjectDLL(processInfo.dwProcessId, dllFullPathW);
	if (!result.success)
	{
		QMessageBox::critical(this, tr("Injection Failed"), QString::fromStdWString(result.errorMessage));
		::TerminateProcess(processInfo.hProcess, 1);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		return;
	}

	processId = processInfo.dwProcessId;
	processHandle = processInfo.hProcess;
	threadHandle = processInfo.hThread;

	::ResumeThread(threadHandle);

	launchButton->setEnabled(false);
	terminateButton->setEnabled(true);

	StartProcessMonitoring();
}

void QVista::on_terminateButton_clicked()
{
	if (processId == 0)
	{
		QMessageBox::warning(this, tr("No Process"), tr("No Process is running."));
		return;
	}

	std::wstring errorMessage;
	if (!win32util::TerminateProcessById(processId, errorMessage))
	{
		QMessageBox::warning(this, tr("Termination Failed"), QString::fromStdWString(errorMessage));
		return;
	}

	CleanupProcess();
}

void QVista::StartProcessMonitoring()
{
	if (!processHandle)
		return;

	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [this, timer]() 
		{
		if (processHandle && WaitForSingleObject(processHandle, 0) == WAIT_OBJECT_0) 
		{
			CleanupProcess();
			timer->stop();
			timer->deleteLater();
		}
		});
	timer->start(500);
}

void QVista::CleanupProcess()
{
	if (processId != 0) {
		CloseHandle(processHandle);
		CloseHandle(threadHandle);
		processId = 0;
		processHandle = nullptr;
		threadHandle = nullptr;
	}

	terminateButton->setEnabled(false);
	launchButton->setEnabled(true);
}
