#ifndef CRYPTOWORKBENCH_H
#define CRYPTOWORKBENCH_H

#include <QtWidgets/QMainWindow>
#include "ui_CryptoWorkbench.h"

class CryptoWorkbench : public QMainWindow
{
	Q_OBJECT

public:
	CryptoWorkbench(QWidget *parent = 0);
	~CryptoWorkbench();

private:
	Ui::CryptoWorkbenchClass ui;
};

#endif // CRYPTOWORKBENCH_H
