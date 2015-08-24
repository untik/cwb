#include "CryptoWorkbench.h"
#include "JavascriptInterface.h"

CryptoWorkbench::CryptoWorkbench(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	JavascriptInterface js;
}

CryptoWorkbench::~CryptoWorkbench()
{

}
