// Felix 2018, GPLv3+
// Part of QUCS

#include "qucsdoc.h"
#include "schematic_model.h"
#include "schematic_lang.h"
#include "globals.h"
#include "diagrams/diagram.h" // BUG
#include "diagrams/diagrams.h" // worse

class LegacySchematicLanguage : public SchematicLanguage {
public:
	LegacySchematicLanguage() : SchematicLanguage(){ untested();
		defaultSchematicLanguage = this;
	}
private: // stuff saved from schematic_file.cpp
	Diagram* loadDiagram(QString const& Line, DocumentStream& /*, DiagramList *List */) const;
private:
	void parse(DocumentStream& stream, SchematicModel* s) const {
		QString Line;

		// mode: a throwback to the legacy format:
		//       connect legacy "parsers".
		// this is not needed in a proper SchematicLanguage
		char mode='\0';
		while(!stream.atEnd()) {
			Line = stream.readLine();
			Line = Line.trimmed();
			if(Line.size()<2){
			}else if(Line.at(0) == '<'
			  && Line.at(1) == '/'){
				qDebug() << "endtag?" << Line;
			}else if(Line.isEmpty()){
			}else if(Line == "<Components>") {
				mode='C';
			}else if(Line == "<Symbol>") {
				mode='S';
			}else if(Line == "<Wires>") {
				mode='W';
			}else if(Line == "<Diagrams>") { untested();
				mode='D';
			}else if(Line == "<Properties>") { untested();
				mode='Q';
			}else if(Line == "<Paintings>") { untested();
				mode='P';
			}else{

				/// \todo enable user to load partial schematic, skip unknown components
				Element*c=nullptr;
				if(mode=='C'){
					c = getComponentFromName(Line, s /*connect ports?*/);
				}else if(mode=='S'){
//					// fix later.
					SchematicSymbol* s=new SchematicSymbol();
					try{
						qDebug() << "symbol Paintings";
						s->symbolPaintings().load(&stream);
						c = s;
					}catch(...){
						incomplete();
					}
				}else if(mode=='W'){
					// (Node*)4 =  move all ports (later on)
					Wire* w = new Wire(0,0,0,0, (Node*)4,(Node*)4);
					c = w->obsolete_load(Line);
					if(!c){
						qDebug() << "ERROR" << Line;
						delete(w);
					}else{
					}
				}else if(mode=='D'){
					qDebug() << "diagram parse?" << Line;

					Diagram* d=loadDiagram(Line, stream);
					if(d){
						c = d;
					}else{
						incomplete();
					}

				}else if(mode=='Q'){
				}else{
					qDebug() << "LSL::parse" <<  Line;
					incomplete();
				}

				if(c){
					s->pushBack(c);
				}else{
				}

			}
		}
	}
}defaultSchematicLanguage_;

Diagram* LegacySchematicLanguage::loadDiagram(QString const& line_in,
		DocumentStream& stream /*, DiagramList *List */)const
{ untested();
	Diagram *d;
	QString Line=line_in;
	QString cstr;
	if(!stream.atEnd()) { untested();
		qDebug() << "diagram?" << Line;
		if(Line.at(0) == '<') if(Line.at(1) == '/') return nullptr;
		Line = Line.trimmed();
		if(Line.isEmpty()){
			return nullptr;
		}else{
		}

		cstr = Line.section(' ',0,0);    // diagram type
		qDebug() << "diagram section" << cstr;
		if(cstr == "<Rect") d = new RectDiagram();
		else if(cstr == "<Polar") d = new PolarDiagram();
		else if(cstr == "<Tab") d = new TabDiagram();
		else if(cstr == "<Smith") d = new SmithDiagram();
		else if(cstr == "<ySmith") d = new SmithDiagram(0,0,false);
		else if(cstr == "<PS") d = new PSDiagram();
		else if(cstr == "<SP") d = new PSDiagram(0,0,false);
		else if(cstr == "<Rect3D") d = new Rect3DDiagram();
		else if(cstr == "<Curve") d = new CurveDiagram();
		else if(cstr == "<Time") d = new TimingDiagram();
		else if(cstr == "<Truth") d = new TruthDiagram();
		/*else if(cstr == "<Phasor") d = new PhasorDiagram();
		  else if(cstr == "<Waveac") d = new Waveac();*/
		else { untested();
			incomplete();
			// throw ...
			return nullptr;
		}

		if(!d->load(Line, stream)) { untested();
			incomplete();
			delete d;
			return nullptr;
		}else{ untested();
		}
		return d;
	}

	return nullptr;
}
