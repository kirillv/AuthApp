#include "AuthApp.h"

#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkReply>


#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <QUrlQuery>
#include <QUuid>

/*

// Developer and user json to post

{
"$class": "org.pone.network.PoneDeveloper",
"participantId": "string", 
"email": "string",
"userName": "string"
}

{
"$class": "org.pone.network.PoneUser",
"participantId": "string",
"email": "string",
"userName": "string"
}

*/

AuthApp::AuthApp(QWidget *parent)
  : QMainWindow(parent), 
    _urlUser("http://192.168.101.110:3000/api/PoneUser"), 
    _urlQuery("http://192.168.101.110:3000/api/queries/selectDeveloperByCredentials")
{
  ui.setupUi(this);

  QObject::connect(ui.loginButton, &QPushButton::clicked, this, &AuthApp::loginUserClicked);
  QObject::connect(ui.addUserButton, &QPushButton::clicked, this, &AuthApp::addUserClicked);

  this->_manager = new QNetworkAccessManager(this);
}

void AuthApp::addUserClicked(bool checked)
{
  this->getInput();

  QNetworkRequest request(this->_urlUser);
  request.setRawHeader("Content-Type", "application/json");

  QJsonObject loginObject;
  loginObject.insert("$class", QJsonValue::fromVariant("org.pone.network.PoneUser"));
  loginObject.insert("email", QJsonValue::fromVariant(this->_email));
  loginObject.insert("userName", QJsonValue::fromVariant(this->_userName));
  loginObject.insert("participantId", QJsonValue::fromVariant(QUuid::createUuid().toString()));
  QJsonDocument doc(loginObject);

  QNetworkReply * repl = this->_manager->post(request, doc.toJson());
  connect(repl, &QNetworkReply::finished, this, &AuthApp::onAuthFinished);
}

void AuthApp::loginUserClicked(bool checked)
{
  this->getInput();

  QNetworkRequest request(this->_urlQuery);
  request.setRawHeader("Content-Type", "application/json");

  QUrlQuery params;
  params.addQueryItem("mail", this->_email);
  params.addQueryItem("mail", this->_email);

  QNetworkReply * repl = this->_manager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
  connect(repl, &QNetworkReply::finished, this, &AuthApp::onAuthFinished);
}

void AuthApp::getInput()
{
  this->_userName = ui.usernameEdit->text();
  this->_email = ui.emailEdit->text();
}

void AuthApp::onAuthFinished()
{
  QMessageBox msgBox;

  QNetworkReply *rep = qobject_cast<QNetworkReply*>(sender());

  if (!checkStatus(rep)) {
    msgBox.setText("Auth failed.");
    msgBox.exec();
    return;
  }

  QString reply = rep->readAll();
  qDebug() << reply;

  QJsonDocument doc = QJsonDocument::fromJson(reply.toUtf8());
  QJsonArray jsonArray = doc.array();

  int arrSize = jsonArray.size();

  if (!arrSize) {
    // Naive check
    // Just for testing

    msgBox.setText("No such credentials");
    msgBox.exec();
    return;
  } else if (arrSize != 1) {
    msgBox.setText("More than one credentials");
    msgBox.exec();
    return;
  }

  // Would be returned array of elements
  // Should be 1 or empty
  
  msgBox.setText("Auth complete.");
  msgBox.exec();
}

void AuthApp::onAddUserFinished()
{
  QMessageBox msgBox;

  QNetworkReply *rep = qobject_cast<QNetworkReply*>(sender());

  if (!checkStatus(rep)) {
    msgBox.setText("Add user failed.");
    msgBox.exec();
    return;
  }

  QString reply = rep->readAll();
  //QJsonDocument doc = QJsonDocument::fromJson(reply.toUtf8());

  qDebug() << reply;

  msgBox.setText("Add user complete.");
  msgBox.exec();
}

bool AuthApp::checkStatus(QNetworkReply * rep)
{
  if (!rep)
    return false;

  QVariant statusCode = rep->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  if (!statusCode.isValid())
    return false;

  int status = statusCode.toInt();

  if (statusCode.toInt() != 200) {
    return false;
  }

  return true;
}
