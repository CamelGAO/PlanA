#ifndef _PRINTER_H
#define _PRINTER_H

#include "stdafx.h"
#include "Base.h"

class classPrinter : public classBase
{
public:
	classPrinter(vpObserver *_observer, vsChannel::Subscriber *_subscriber = NULL);
	classPrinter(const char *_name, vsChannel::Subscriber *_subscriber = NULL);
	~classPrinter();
	void init();

protected:
	
private:

};

#endif
