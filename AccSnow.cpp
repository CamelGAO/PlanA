#include "stdafx.h"
#include "Base.h"
#include "AccSnow.h"

bool classAccSnow::isFirstInit = true;

classAccSnow::classAccSnow(vpObserver *_observer, vsChannel::Subscriber *_subscriber):classBase(_observer,_subscriber,ENABLE_KEY),density(0)
{
	init();
}
classAccSnow::classAccSnow(const char *_name, vsChannel::Subscriber *_subscriber):classBase(_name,_subscriber,ENABLE_KEY),density(0)
{
	init();
}

classAccSnow::~classAccSnow()
{
	pChannel->setRepresentationIndex(0);
	pChannel->removeRepresentation(representation);
	pShaderDeployer->removeRepresentation("r_accsnow");

	isFirstInit = true;
}

void classAccSnow::init(void)
{
	if( (pVertexShader = pShaderDeployer->getShader("v_accsnow")) == NULL )
		std::cout << " vShader \"v_accsnow\" doesn't exist! " << std::endl;

	if( (pFragmentShader = pShaderDeployer->getShader("f_accsnow")) == NULL )
		std::cout << " fShader \"f_accsnow\" doesn't exist! " << std::endl;

	acc_density = vuParameter::obtain< vuParameterTyped<float> >("density");

	pShaderDeployer->addRepresentation("r_accsnow");
	representation = pShaderDeployer->getRepresentation("r_accsnow");

	if(isFirstInit == true)
	{
		vsTraversalUser<int, vsTraversalLookUpNodeId> trav;
		trav.addPreVisit(vsGeometry::getStaticNodeId(), this, travFunc);			
		trav.visit(pObject);

		isFirstInit = false;
	}
}

void classAccSnow::handleKey(vrWindow::Key _key, int _mod)
{
	switch (_key)
	{
	case vrWindow::KEY_F5:
		pChannel->setRepresentationIndex( pChannel->getRepresentationIndex() == representation ? 0 : representation );
		break;
		
	case vrWindow::KEY_UP:
		acc_density->set(density >= 10000.0f ? density = 10000.0f : density += 0.1); 
		break;

	case vrWindow::KEY_DOWN:
		acc_density->set(density < 0.1f ? density = 0.1f : density -= 0.1); 
		break;
	}
}

void classAccSnow::setDensity(float _density)
{
	if(_density != 0)
		pChannel->setRepresentationIndex(representation);
	else
		pChannel->setRepresentationIndex(0);
	acc_density->set(_density);
}

vsTraversal::Result classAccSnow::travFunc(vsNode *node, int)
{
	vsGeometry *geometry = static_cast<vsGeometry *>(node);
	geometry->setGeometry(geometry->getGeometry(), representation);
	vrState *state = new vrState();
	vsgu::copy(state, geometry->getState());

	if(pVertexShader != NULL)
	{
		vrShader::ElementVertex vertexShaderElement;
		vertexShaderElement.m_shader = pVertexShader;
		state->setElement(vrShader::ElementVertex::Id, &vertexShaderElement);
	}

	if(pFragmentShader != NULL)
	{
		vrShader::ElementFragment fragmentShaderElement;
		fragmentShaderElement.m_shader = pFragmentShader;
		state->setElement(vrShader::ElementFragment::Id, &fragmentShaderElement);	
	}

	geometry->setState(state, representation);
	return vsTraversal::RESULT_CONTINUE;
}