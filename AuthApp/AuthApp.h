#pragma once

#include <QtWidgets/QMainWindow>

#include <QString>
#include <QNetworkAccessManager>
#include <QUrl>

#include "ui_AuthApp.h"

class AuthApp : public QMainWindow
{
  Q_OBJECT

public:
  AuthApp(QWidget *parent = Q_NULLPTR);

  private slots:
  void addUserClicked(bool checked = false);
  void loginUserClicked(bool checked = false);

private:
  void getInput();

  void onAuthFinished();
  void onAddUserFinished();

  bool checkStatus(QNetworkReply * rep);

  Ui::AuthAppClass ui;

  QString _userName;
  QString _email;

  QUrl _urlUser;
  QUrl _urlQuery;

  QNetworkAccessManager *_manager;
};
