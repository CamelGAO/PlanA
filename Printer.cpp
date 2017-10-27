#include "stdafx.h"
#include "Printer.h"

classPrinter::classPrinter(const char *_name, vsChannel::Subscriber *_subscriber)
: classBase(_name, _subscriber, ENABLE_DRAWEVENT, false, TEXT("myObject"))
{
	init();
}

classPrinter::classPrinter(vpObserver * _observer, vsChannel::Subscriber *_subscriber)
: classBase(_observer, _subscriber, ENABLE_DRAWEVENT, false, TEXT("myObject"))
{
	init();
}

classPrinter::~classPrinter()
{
	removeEvent(vsChannel::EVENT_POST_DRAW);
}

void classPrinter::init()
{
	addEvent(vsChannel::EVENT_POST_DRAW);

	vsTraversalUser<int, vsTraversalLookUpNodeId> trav;
	trav.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc);			
	trav.visit(pBuilding);
}