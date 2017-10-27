int classGeometry::creatXML(const char* _path)   //生成测试路径
{
	char buffer[1024];
	const char* d ="xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"";
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLDeclaration* declaration=doc.NewDeclaration(d);
	doc.InsertFirstChild(declaration);
	tinyxml2::XMLElement* root=doc.NewElement("root");

	for(int i = 0; i < 1000; i++)
	{
		PathNode temp;

		temp._hpr[0] = atan(cos(i*3.1415926/180))*180/3.1415926-90;
		temp._hpr[1] = 0;
		temp._hpr[2] = 0;

		temp._pos[0] = 1000*i*3.1415926/180;
		temp._pos[1] = 1000*sin(i*3.1415926/180);
		temp._pos[2] = 300;

		tinyxml2::XMLElement* node = doc.NewElement("node");
		node->SetAttribute("index",i);
		root->InsertEndChild(node);

		tinyxml2::XMLElement* x = doc.NewElement("x");
		tinyxml2::XMLUtil::ToStr(temp._pos[0], buffer, 1024);
		x->InsertEndChild(doc.NewText(buffer));
		node->InsertEndChild(x);

		tinyxml2::XMLElement* y = doc.NewElement("y");
		tinyxml2::XMLUtil::ToStr(temp._pos[1], buffer, 1024);
		y->InsertEndChild(doc.NewText(buffer));
		node->InsertEndChild(y);

		tinyxml2::XMLElement* z = doc.NewElement("z");
		tinyxml2::XMLUtil::ToStr(temp._pos[2], buffer, 1024);
		z->InsertEndChild(doc.NewText(buffer));
		node->InsertEndChild(z);

		tinyxml2::XMLElement* h = doc.NewElement("h");
		tinyxml2::XMLUtil::ToStr(temp._hpr[0], buffer, 1024);
		h->InsertEndChild(doc.NewText(buffer));
		node->InsertEndChild(h);

		tinyxml2::XMLElement* p = doc.NewElement("p");
		tinyxml2::XMLUtil::ToStr(temp._hpr[1], buffer, 1024);
		p->InsertEndChild(doc.NewText(buffer));
		node->InsertEndChild(p);

		tinyxml2::XMLElement* r = doc.NewElement("r");
		tinyxml2::XMLUtil::ToStr(temp._hpr[2], buffer, 1024);
		r->InsertEndChild(doc.NewText(buffer));
		node->InsertEndChild(r);
	}

	doc.InsertEndChild(root);

	return doc.SaveFile(_path);
}

int classGeometry::creatXML(const char* _path)   //生成landingarea蓝图
{
	char* buffer;
	const char* d ="xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"";
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLDeclaration* declaration=doc.NewDeclaration(d);
	doc.InsertFirstChild(declaration);
	tinyxml2::XMLElement* root=doc.NewElement("root");

	for(int i = 0; i < 104; i++)
	{
		static int a = 0;
		if (i < 2)
		{
			buffer = "Altimeter";
			static int t = 0;
			a = t++;
		}
		else if (i < 4)
		{
			buffer = "Cross";
			static int t = 0;
			a = t++;
		}
		else if (i < 6)
		{
			buffer = "Indicator";
			static int t = 0;
			a = t++;
		}
		else if (i < 22)
		{
			buffer = "Central area";
			static int t = 0;
			a = t++;
		}
		else if (i < 52)
		{
			buffer = "Big pillar";
			static int t = 0;
			a = t++;
		}
		else if (i < 88)
		{
			buffer = "Small pillar";
			static int t = 0;
			a = t++;
		}
		else if (i < 104)
		{
			buffer = "Surface";
			static int t = 0;
			a = t++;
		}
		tinyxml2::XMLElement* node = doc.NewElement("line");
		node->SetAttribute("x1",0);
		node->SetAttribute("y1",1);
		node->SetAttribute("z1",2);
		node->SetAttribute("x2",3);
		node->SetAttribute("y2",4);
		node->SetAttribute("z2",5);
		node->SetAttribute("category",buffer);
		node->SetAttribute("index",a);
		root->InsertEndChild(node);
	}

	doc.InsertEndChild(root);

	return doc.SaveFile(_path);
}