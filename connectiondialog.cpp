#include "connectiondialog.h"

ConnectionDialog::ConnectionDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("数据库连接");
    setMinimumSize(400, 300);
    
    setupUI();
    setupConnections();
}

ConnectionDialog::~ConnectionDialog()
{
}

QString ConnectionDialog::driver() const
{
    return driverCombo->currentText();
}

QString ConnectionDialog::host() const
{
    return hostEdit->text();
}

QString ConnectionDialog::port() const
{
    return portEdit->text();
}

QString ConnectionDialog::databaseName() const
{
    return dbNameEdit->text();
}

QString ConnectionDialog::username() const
{
    return usernameEdit->text();
}

QString ConnectionDialog::password() const
{
    return passwordEdit->text();
}

void ConnectionDialog::onDriverChanged(int index)
{
    QString driver = driverCombo->itemText(index);
    
    // 根据不同的数据库驱动，调整UI
    if (driver == "QSQLITE") {
        hostEdit->setEnabled(false);
        portEdit->setEnabled(false);
        usernameEdit->setEnabled(false);
        passwordEdit->setEnabled(false);
    } else {
        hostEdit->setEnabled(true);
        portEdit->setEnabled(true);
        usernameEdit->setEnabled(true);
        passwordEdit->setEnabled(true);
    }
}

void ConnectionDialog::onTestConnectionClicked()
{
    QString driverText = driver();
    
    // 移除默认连接
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
    }
    
    QSqlDatabase db = QSqlDatabase::addDatabase(driverText);
    
    if (driverText != "QSQLITE") {
        db.setHostName(host());
        if (!port().isEmpty())
            db.setPort(port().toInt());
        db.setUserName(username());
        db.setPassword(password());
    }
    
    db.setDatabaseName(databaseName());
    
    if (db.open()) {
        QMessageBox::information(this, "连接成功", "已成功连接到数据库");
        db.close();
    } else {
        QMessageBox::critical(this, "连接失败", "无法连接到数据库:\n" + db.error().text());
    }
}

void ConnectionDialog::setupUI()
{
    // 创建控件
    QLabel *driverLabel = new QLabel("数据库驱动:", this);
    QLabel *hostLabel = new QLabel("主机:", this);
    QLabel *portLabel = new QLabel("端口:", this);
    QLabel *dbNameLabel = new QLabel("数据库名:", this);
    QLabel *usernameLabel = new QLabel("用户名:", this);
    QLabel *passwordLabel = new QLabel("密码:", this);
    
    driverCombo = new QComboBox(this);
    driverCombo->addItems({"QSQLITE", "QMYSQL", "QPSQL", "QODBC"});
    
    hostEdit = new QLineEdit("localhost", this);
    portEdit = new QLineEdit(this);
    dbNameEdit = new QLineEdit(this);
    usernameEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    
    testButton = new QPushButton("测试连接", this);
    okButton = new QPushButton("确定", this);
    cancelButton = new QPushButton("取消", this);
    
    // 创建布局
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(driverLabel, driverCombo);
    formLayout->addRow(hostLabel, hostEdit);
    formLayout->addRow(portLabel, portEdit);
    formLayout->addRow(dbNameLabel, dbNameEdit);
    formLayout->addRow(usernameLabel, usernameEdit);
    formLayout->addRow(passwordLabel, passwordEdit);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(testButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    
    // 初始设置
    onDriverChanged(0);
}

void ConnectionDialog::setupConnections()
{
    connect(driverCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConnectionDialog::onDriverChanged);
    connect(testButton, &QPushButton::clicked, this, &ConnectionDialog::onTestConnectionClicked);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}    
