// Felix 2018, GPLv3+
// Part of QUCS

#include "qucsdoc.h"
#include "schematic_model.h"
#include "schematic_lang.h"
#include "globals.h"

class LegacySchematicLanguage : public SchematicLanguage {
public:
	LegacySchematicLanguage() : SchematicLanguage(){ untested();
		defaultSchematicLanguage = this;
	}
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
					incomplete();
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
