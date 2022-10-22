#include "connection_dialog.h"

ConnectionDialog::ConnectionDialog(QWidget* parent, SocketAdapter* socket) :
	QDialog(parent),
	VBoxLayout(new QVBoxLayout(this)),
	FormLayout(new QFormLayout(this)),
  	StatusLabel(new QLabel(tr("PROTOS BUS Monitor is disconnected now"), this)),
	PortLineEdit(new QLineEdit(tr("3699"), this)),
	IpLineEdit(new QLineEdit(tr("127.0.0.1"), this)),
	ConnectButton(new QPushButton(tr("Connect"), this)),
	Socket(socket)
{
	setWindowTitle(tr("Connection Settings"));	
	StatusLabel->setWordWrap(true);
	StatusLabel->setMinimumWidth(210);
	StatusLabel->setMinimumHeight(25);
	VBoxLayout->addWidget(StatusLabel);
	VBoxLayout->addSpacing(10);
	VBoxLayout->addLayout(FormLayout);	
	FormLayout->addRow(tr("IP:"), IpLineEdit);
	FormLayout->addRow(tr("Port:"), PortLineEdit);
	VBoxLayout->addWidget(ConnectButton);
	
	ConnectionTimer.setSingleShot(true);	

	if (Socket->IsConnected())
	{
		QString SocketIp   = Socket->GetIp();
		QString SocketPort = QString::number(Socket->GetPort());
		QString LabelText  = tr("Sucsesfully connected to\n Host Server: IP: %1, Port: %2").arg(SocketIp).arg(SocketPort);
		StatusLabel->setText(LabelText);
		IpLineEdit->setText(SocketIp);			
		PortLineEdit->setText(SocketPort);
		IpLineEdit->setEnabled(false);
		PortLineEdit->setEnabled(false);
		ConnectButton->setText(tr("Disconnect"));		
		Result = 1;
	}
	else
	{		
		QString LabelText  = tr("Disconnected from Host Server now");
		StatusLabel->setText(LabelText);
		IpLineEdit->setEnabled(true); 
		PortLineEdit->setEnabled(true);
		ConnectButton->setText(tr("Connect"));		
		Result = 0;
	}

	connect(ConnectButton, SIGNAL(clicked(bool)), this, SLOT(on_ConnectButton_clicked(bool)));	
}

ConnectionDialog::~ConnectionDialog()
{
}

void ConnectionDialog::closeEvent(QCloseEvent* event)
{
	setResult(Result);
}

void ConnectionDialog::on_ConnectButton_clicked(bool clicked)
{
	if (ConnectButton->text() == tr("Connect"))
	{
		QString SocketIp   = IpLineEdit->text();
		QString SocketPort = PortLineEdit->text();
		StatusLabel->setText(tr("Connecting now to\nHost Server: IP: %1, Port: %2").arg(SocketIp).arg(SocketPort));		
		if (Socket->Connect(SocketIp, SocketPort.toInt(), 1000))
		{
			ConnectButton->setText(tr("Disconnect"));
			IpLineEdit->setEnabled(false);
			PortLineEdit->setEnabled(false);
			StatusLabel->setStyleSheet("color : black");
			StatusLabel->setText(tr("Succsesfully connected to\nHost Server: IP: %1, Port: %2").arg(SocketIp).arg(SocketPort));
			StatusLabel->setStyleSheet("color : black");
			Result = 1;
			close();
		}
		else
		{
			StatusLabel->setStyleSheet("color : 0x0000ff");
			StatusLabel->setText(tr("Unable to connect to\nHost Server: IP: %1, Port: %2").arg(SocketIp).arg(SocketPort));
			StatusLabel->setStyleSheet("color : red");
			Result = 0;
		}
	}
	else
	{
		Socket->Disconnect(1000);
		StatusLabel->setStyleSheet("color : black");
		StatusLabel->setText(tr("Disconnected from Host Server now"));		
		IpLineEdit->setEnabled(true);
		PortLineEdit->setEnabled(true);	
		ConnectButton->setText(tr("Connect"));
		Result = 0;
	}	
}

bool ConnectionDialog::isConnected() const {
    return Socket->IsConnected();
}

