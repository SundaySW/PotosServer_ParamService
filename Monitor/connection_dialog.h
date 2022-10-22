#ifndef CONNECTION_DIALOG_H
#define CONNECTION_DIALOG_H

#include "socket_adapter.h"

#include <QDialog>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include <QTimer>

class ConnectionDialog : public QDialog
{
	Q_OBJECT

public:
	ConnectionDialog(QWidget* parent, SocketAdapter* socket);
	bool isConnected() const;
	~ConnectionDialog();

private:
	void closeEvent(QCloseEvent* event);
	
	QVBoxLayout* VBoxLayout;
	QFormLayout* FormLayout;
	QLabel *PortLabel, *IpLabel, *StatusLabel;
	QLineEdit *PortLineEdit, *IpLineEdit;
	QPushButton* ConnectButton;
	SocketAdapter* Socket;
	QTimer ConnectionTimer;
	int Result;

private slots:
	void on_ConnectButton_clicked(bool clicked);	
};

#endif //CONNECTION_DIALOG_H

