#define WIN32_LEAN_AND_MEAN

#include "MetaProcessor.h"

#include <iostream>
using namespace std;
	/*class used to process the input metadata and create a metadata file in a desired format*/
	void MetaProcessor::stringTokenize(const string& str, vector<string>& tokens, const string& delimiters = " ")
	{
		 // Skip delimiters at beginning.
		string::size_type lastPos = str.find_first_not_of(delimiters, 0);
		// Find first "non-delimiter".
		string::size_type pos     = str.find_first_of(delimiters, lastPos);

		while (std::string::npos != pos || std::string::npos != lastPos)
		{
			// Found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);
			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}
	}

	xercesc::DOMDocument* MetaProcessor::process(char* name, string** resultPath)
	{
		try
		{
			 XMLPlatformUtils::Initialize();
		 }
		catch(const XMLException& toCatch)
		{
			exit(1);
		}
		try
		{
			//dom implementation
			DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));
			 //create a parser
			DOMBuilder*  parser= 
			((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
			//enable datatype normalization - default is off
			parser->setFeature(XMLUni::fgDOMDatatypeNormalization, true);

			//load the xml document in a DOMDocument
			xercesc::DOMDocument* doc = parser->parseURI(X(name));
			DOMElement* root = doc->getDocumentElement();

			//retrieve the name of the avi
			DOMNodeList* nodeList = root->getElementsByTagName(X("Description"));
			DOMElement* el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("MultimediaContent"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("AudioVisual"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("MediaInformation"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("MediaProfile"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("MediaInstance"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("MediaLocator"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("MediaUri"));
			el = (DOMElement*) nodeList->item(0);
			string aviName(StrX(el->getFirstChild()->getTextContent()));

			//retrieve the media start time
			nodeList = root->getElementsByTagName(X("Description"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("MultimediaContent"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("AudioVisual"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("MediaSourceDecomposition"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("VideoSegment"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("TemporalDecomposition"));
			el = (DOMElement*) nodeList->item(0);
			nodeList = el->getElementsByTagName(X("VideoSegment"));
			el = (DOMElement*) nodeList->item(0);
			
			nodeList = el->getElementsByTagName(X("MediaTime"));
			DOMElement* el3 = (DOMElement*) nodeList->item(0);

			nodeList = el3->getElementsByTagName(X("MediaTimePoint"));
			DOMElement* el2 = (DOMElement*) nodeList->item(0);
			string str(StrX(el2->getFirstChild()->getTextContent()));
			vector<string> tokens;
			stringTokenize((const string&)str, tokens, "T");
			str = tokens.at(1);
			tokens.clear();
			stringTokenize((const string&)str, tokens, ":");
			int startHour =  atoi(tokens.at(0).c_str());
			int startMin = atoi(tokens.at(1).c_str());
			int startSec =  atoi(tokens.at(2).c_str());
			int startFrame = atoi(tokens.at(3).c_str());
			tokens.clear();

			//retrieve the media duration 
			nodeList = el3->getElementsByTagName(X("MediaDuration"));
			el2 = (DOMElement*) nodeList->item(0);
			str = *(new string(StrX(el2->getFirstChild()->getTextContent())));
			string temp = "";
			for(unsigned int i=str.find("T",0)+1; i< str.find("H",0);i++)
			{
				temp +=  str[i];
			}
			int durH = atoi(temp.c_str());
			temp="";
			for(unsigned int i=str.find("H",0)+1; i< str.find("M",0);i++)
			{
				temp +=  str[i];
			}
			int durM = atoi(temp.c_str());
			temp="";	
			for(unsigned int i=str.find("M",0)+1; i< str.find("S",0);i++)
			{
				temp +=  str[i];
			}
			int durS = atoi(temp.c_str());
			temp="";
			for(unsigned int i=str.find("S",0)+1; i< str.find("N",0);i++)
			{
				temp +=  str[i];
			}
			int durF = atoi(temp.c_str());

			//compute the frame rate
			int frameRate = 25;

			//count the frames 
			int frameNumber = (durH*3600 + durM*60 + durS)* frameRate + durF;
			
			//initialize the output document
			xercesc::DOMDocument* newDoc= impl->createDocument(
                           0,                    // root element namespace URI.
                           X("stinput"),         // root element name
                           0);                   // document type object (DTD).

			//store the name of the avi
			newDoc->getDocumentElement()->setAttribute(X("aviName"), X(aviName.c_str()));

			DOMElement* newRoot = newDoc->getDocumentElement();
			for(int i=0; i<frameNumber; i++)
			{
				char name[100];
				sprintf(name,"frame%d",i);
				newRoot->appendChild(newDoc->createElement(X(name)));
			}

			/*process each moving region element and populate the output document*/
			//retrieve the moving regions list
			nodeList = el->getElementsByTagName(X("SpatioTemporalDecomposition"));
			el = (DOMElement*) nodeList->item(0);
			bool skip = false;
			try
			{
				nodeList = el->getElementsByTagName(X("MovingRegion"));
			}
			catch(...)
			{
				//no regions, skip processing
				skip=true;
			}
			
			if(!skip)
			{
			  //process each node (region). the id of the region is i.
			  for(unsigned int i=0; i< nodeList->getLength(); i++)
			  {
				DOMElement* region = (DOMElement*) nodeList->item(i);

				DOMNodeList* tempList = region->getElementsByTagName(X("SemanticRef"));
				DOMElement* nameElem = (DOMElement*) tempList->item(0);
				string humanid(StrX(nameElem->getAttribute(X("idref"))));
			
				//get the parameter trajectories summing up the coordinate data of the various parameter trajectories
				DOMNodeList* regionTrajectoryList = ((DOMElement*) region->getElementsByTagName(
					X("SpatioTemporalLocator"))->item(0))->getElementsByTagName(
					X("ParameterTrajectory"));
				
				//each trajectory element gives the coordinates of the region's 
				//bounding rectrangle for a period of time. we analyze this data 
				//each one individually
				for(unsigned int j=0; j< regionTrajectoryList->getLength(); j++)
				{
					//process the next element
					DOMElement* trajectoryElement = (DOMElement*) regionTrajectoryList->item(j);

					//retrieve the traj info start time
					DOMElement* trajTimeElement = (DOMElement*) trajectoryElement->getElementsByTagName(
						X("MediaTime"))->item(0);
					DOMElement* trajStartTimeElement = (DOMElement*) trajTimeElement->getElementsByTagName(
						X("MediaTimePoint"))->item(0);
					trajStartTimeElement = (DOMElement*) trajStartTimeElement->getFirstChild();
					string trajStartTime = *(new string(StrX(trajStartTimeElement->getTextContent())));

					tokens.clear();
					stringTokenize((const string&)trajStartTime, tokens, ":");
					int trajStartHour = atoi(tokens[0].c_str());
					int trajStartMin = atoi(tokens[1].c_str());
					int trajStartSec = atoi(tokens[2].c_str());
					char* trajStartRest = strstr(tokens[3].c_str(),"F");
					int trajStartLength = trajStartRest - tokens[3].c_str();
					trajStartLength++;
					char* trajStartMil = new char[trajStartLength];
					memcpy(trajStartMil,tokens[3].c_str(),trajStartLength);
					trajStartMil[trajStartLength-1]='\0';
					int trajStartM = atoi(trajStartMil);
					tokens.clear();
					
					//find the starting frame id
					int startFrameId = (trajStartHour*3600 + trajStartMin*60 
						+ trajStartSec)*frameRate + trajStartM;
				
					//retrieve the traj info duration
					DOMElement* trajDurationElement = (DOMElement*) trajTimeElement->getElementsByTagName(
						X("MediaDuration"))->item(0);
					trajDurationElement = (DOMElement*) trajDurationElement->getFirstChild();
					str = *(new string(StrX(trajDurationElement->getTextContent())));

					string temp = "";
					for(unsigned int k=str.find("T",0)+1; k< str.find("H",0);k++)
					{
						temp +=  str[k];
					}
					int durH = atoi(temp.c_str());
					temp="";
					for(unsigned int k=str.find("H",0)+1; k< str.find("M",0);k++)
					{
						temp +=  str[k];
					}
					int durM = atoi(temp.c_str());
					temp="";	
					for(unsigned int k=str.find("M",0)+1; k< str.find("S",0);k++)
					{
						temp +=  str[k];
					}
					int durS = atoi(temp.c_str());
					temp="";
					for(unsigned int k=str.find("S",0)+1; k< str.find("N",0);k++)
					{
						temp +=  str[k];
					}
					int durF = atoi(temp.c_str());
				
					//find the end frame id
					int endFrameId = (durH*3600 + durM*60 + durS)*frameRate + durF + startFrameId;

					//get the coordinates of the region
					DOMElement* coordsElem = (DOMElement*) trajectoryElement->getElementsByTagName(
						X("InitialRegion"))->item(0);
					coordsElem = (DOMElement*) coordsElem->getElementsByTagName(
						X("Polygon"))->item(0);
					coordsElem = (DOMElement*) coordsElem->getElementsByTagName(
						X("Coords"))->item(0);
					coordsElem = (DOMElement*) coordsElem->getFirstChild();
					string coords = *(new string(StrX(coordsElem->getTextContent())));

					tokens.clear();
					stringTokenize((const string&)coords, tokens, " ");
					int x1 = atoi(tokens[0].c_str());
					int x2 = x1 + atoi(tokens[1].c_str());
					int y1 = atoi(tokens[4].c_str());
					int y2 = y1 + atoi(tokens[6].c_str());
				
					//create the new element the new document shall be populated with
					char ss[100], ns[100];
					sprintf(ns, "%u", i);
					DOMElement* newElem = (DOMElement*) newDoc->createElement(X("region"));
					newElem->setAttribute(X("id"),X(ns));
					newElem->setAttribute(X("human"), X(humanid.c_str()));
					newElem->setAttribute(X("x1"),X(tokens[0].c_str()));
					newElem->setAttribute(X("x2"),X(itoa(x2,ss,10)));
					newElem->setAttribute(X("y1"),X(tokens[4].c_str()));
					newElem->setAttribute(X("y2"),X(itoa(y2,ss,10)));
				
					tokens.clear();
					//populate the metadata xml file with the generated info
					for(int newIndex=startFrameId; newIndex<endFrameId;newIndex++)
					{
						char name[100];
						sprintf(name,"frame%d",newIndex);
						
						//append the new element
						DOMNodeList* list2AddTo =  newRoot->getElementsByTagName(X(name));
						DOMElement* element2AddTo = (DOMElement*) list2AddTo->item(0);
					
						DOMElement* clonos = (DOMElement*) newElem->cloneNode(false);
						element2AddTo->appendChild(clonos);
					}
				 }
			   }
			  }
			//store the new metadata file
			int pos = aviName.rfind(".avi",aviName.length());
			string out = aviName.substr(0,pos) + "_mod.xml";

			DOMWriter* writer = ((DOMImplementationLS*)impl)->createDOMWriter();
			XMLFormatTarget* myFormTarget = new LocalFileFormatTarget(out.c_str());
			writer->writeNode(myFormTarget, *newDoc);
			myFormTarget->flush();
			delete myFormTarget;
			writer->release();
			string* inputPath2Return = new string(out);
			*resultPath = inputPath2Return;
			delete writer;
		}
		catch(exception& e)
		{
			cout << e.what();
			return NULL;
		}
		catch( ... )
		{
			printf("UNKNOWN ERROR \n") ;
		}
		
		//shut down xerces
	    XMLPlatformUtils::Terminate();
	} 

