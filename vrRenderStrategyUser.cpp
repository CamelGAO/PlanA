#include "stdafx.h"
#include "vrRenderStrategyUser.h"

classRenderStrategySetUniform::classRenderStrategySetUniform(vsNode *_channel)
{
	pChannel = _channel;
	mVertexMin_test = vuParameter::obtain< vuParameterTyped<vuVec3f> >("vertex_min_test");
	mVertexMax_test = vuParameter::obtain< vuParameterTyped<vuVec3f> >("vertex_max_test");
}

classRenderStrategySetUniform::~classRenderStrategySetUniform(){}

void classRenderStrategySetUniform::draw(vrDrawContext *_context, const vrDrawFunc::Data &_data)
{
	//mVertexMax_test->set(_data.m_boundsVertex.m_max);
	//mVertexMin_test->set(_data.m_boundsVertex.m_min);

	vrDrawFunc::Data *data = const_cast<vrDrawFunc::Data *>(&_data);
	vector<vuVec3f> temp;




// 	cout << (_data.m_boundsVertex.m_max)[2] << "  " << (_data.m_boundsVertex.m_min)[2] << endl;
// 
// 	cout << "m_format:			" << _data.m_format << endl;
// 	cout << "m_numColors:			" << _data.m_numColors << endl;
// 	cout << "m_numIndices:			" << _data.m_numIndices << endl;
// 	cout << "m_numNormals:			" << _data.m_numNormals << endl;
// 	cout << "m_maxActiveStage:		" << _data.m_maxActiveStage << endl;
// 	cout << "m_numTextureCoords0:		" << _data.m_numTextureCoords[0] << endl;
// 	cout << "m_numTextureCoords1:		" << _data.m_numTextureCoords[1] << endl;
// 	cout << "m_numVertices:			" << _data.m_numVertices << endl;
// 	cout << "m_primitive:			" << _data.m_primitive << endl;
// 	cout << "m_numPrimitives:		" << _data.m_numPrimitives << endl;
// 	for(size_t i = 0; i < _data.m_numPrimitives; i++)
// 		cout << "m_primitiveLengths" << i << ":		" << (_data.m_primitiveLengths.get())[i] << endl; 
	
	for(size_t i = 0; i < _data.m_numVertices; i++)
	{
		temp.push_back( (_data.m_vertex.get())[i] );
		//cout << "m_Vertices" << i << ":			" << (_data.m_vertex.get())[i][0] << "	" << (_data.m_vertex.get())[i][1]<< "	" << (_data.m_vertex.get())[i][2] << endl;
		//(data->m_normal.get())[i][0] = 0;(data->m_normal.get())[i][1] = 0;(data->m_normal.get())[i][2] = 1;
		(data->m_vertex.get())[i][0] += ((float)rand()/RAND_MAX-0.5)*8; 
		(data->m_vertex.get())[i][1] += ((float)rand()/RAND_MAX-0.5)*8; 
		(data->m_vertex.get())[i][2] += ((float)rand()/RAND_MAX-0.5)*8;
	}
	//cout << endl;

	vrDrawFunc::call(_context, _data);



	for(size_t i = 0; i < _data.m_numVertices; i++)
	{
		(data->m_vertex.get())[i] = temp[i];
	}

}