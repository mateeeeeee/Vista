#pragma once
#include <QtWidgets/QMainWindow>

class QLabel;
class QLineEdit;
class QPushButton;
class QGroupBox;
class QVBoxLayout;
class QFormLayout;
class QPlainTextEdit;
class QLogPipeWorker;

class QVista : public QMainWindow
{
	Q_OBJECT

public:
	explicit QVista(QWidget* parent = nullptr);
	~QVista();

private:
	QGroupBox* programGroupBox;

	QLabel* executablePathLabel;
	QLineEdit* executablePathEdit;
	QPushButton* browseExeButton;

	QLabel* workingDirectoryLabel;
	QLineEdit* workingDirectoryEdit;
	QPushButton* browseDirButton;

	QLabel* commandArgsLabel;
	QLineEdit* commandArgsEdit;

	QPushButton* launchButton;
	QPushButton* terminateButton;

	QVBoxLayout* mainLayout;
	QFormLayout* programLayout;

	uint32_t processId;
	void*	 processHandle;
	void*	 threadHandle;

private slots:
	void on_browseExeButton_clicked();
	void on_browseDirButton_clicked();
	void on_launchButton_clicked();
	void on_terminateButton_clicked();

private:
	void SetupUI();
	void SetupConnections();
	void StartProcessMonitoring();
	void CleanupProcess();
};
