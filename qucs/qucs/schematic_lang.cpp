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
		char mode='\0';
		while(!stream.atEnd()) {
			Line = stream.readLine();
			if(Line.at(0) == '<'
			  && Line.at(1) == '/'){
				incomplete();
				continue;
			}
			Line = Line.trimmed();
			if(Line.isEmpty()){
				continue;
			}else if(Line == "<Components>") {
				mode='C';
			}else if(Line == "<Wires>") {
				mode='W';
			}else if(Line == "<Diagrams>") { untested();
			}else if(Line == "<Paintings>") { untested();
			}

			/// \todo enable user to load partial schematic, skip unknown components
			Element*c=NULL;
			if(mode=='C'){
				c = getComponentFromName(Line, NULL /*???*/);
			}else if(mode=='W'){
				// (Node*)4 =  move all ports (later on)
				Wire* w = new Wire(0,0,0,0, (Node*)4,(Node*)4);
				c = w->obsolete_load(Line);
				if(!c){
					delete(w);
				}else{
				}
			}else{
				incomplete();
			}

			if(c){
				s->pushBack(c);
			}else{
			}

		}
	}
}defaultSchematicLanguage_;
