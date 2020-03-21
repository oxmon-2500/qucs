/***************************************************************************
                                checklibraries.cpp
                               ----------
    begin                : Sat Mar 21 2020
    copyright            : (C) 2020 by qucs
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>

#include <QProcess>
#include "qucslib.h"
#include "checklibraries.h"

int CheckComponentLibraries::parseLibraries (const QString LibDir, QList<ComponentLibrary> &libList){
  QStringList LibFiles;
  QStringList::iterator it;

  LibFiles = QDir(LibDir).entryList(QStringList("*.lib"), QDir::Files, QDir::Name);

  // create top level library items, base on the library names
  for(it = LibFiles.begin(); it != LibFiles.end(); it++){
      QString libPath(*it);
      libPath.chop(4); // remove extension

      ComponentLibrary parsedlibrary;

      int ret = parseComponentLibrary (libPath , parsedlibrary);
      if (ret!=QUCS_COMP_LIB_OK){
        return ret;
      }
      libList.insert(libList.end(), parsedlibrary); //add element
  }
  return QUCS_COMP_LIB_OK;
}

int CheckComponentLibraries::exeProcess(const QString processPath, const QStringList arguments, QString &stdOut){
  QProcess simProcess;
  simProcess.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
  simProcess.setReadChannelMode(QProcess::MergedChannels);
  simProcess.start(processPath, arguments);
  simProcess.waitForFinished();
  stdOut = QString(simProcess.readAllStandardOutput().data());
  return simProcess.exitCode();
}
int CheckComponentLibraries::checkComponentLibraries(const char *argv0, const QString componentName, QStringList &ignoList){
  qucslibProgName = argv0;
  const char * SCH_FILE="tmpQucsLib.sch";
  const char * NET_FILE="tmpQucsLib.net";
  const char * LOG_FILE="qucsLib.log";
  QList<ComponentLibrary> libList;
  int ret = parseLibraries(QucsSettings.LibDir, libList);
  if (ret != QUCS_COMP_LIB_OK){
    fprintf(stderr, "Error %d in parseLibraries\n", ret);
    return -1;
  }

  QString libComp;
  bool ok=true;
  FILE *log = fopen(LOG_FILE, "w");
  for(ComponentLibrary parsedLibrary : libList){
    if(ignoList.contains(parsedLibrary.name+".lib")){
      continue;
    }
    for (int i = 0; i < parsedLibrary.components.count (); i++){
      if (!componentName.isEmpty() && parsedLibrary.components[i].name != componentName){
        continue;
      }
      libComp = parsedLibrary.name + "_" + parsedLibrary.components[i].name;
      //<Components>
      //<Lib PVR100AZ_B12V_1 1 90 120 20 -66 0 0 "Regulators" 0 "PVR100AZ-B12V" 0>
      //<.DC DC1 1 100 210 0 42 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
      //</Components>
      FILE * f = fopen (SCH_FILE, "w");
      fprintf(f, "<Qucs Schematic 0.0.20>\n");
      fprintf(f, "<Properties>\n");
      fprintf(f, "  <View=0,0,800,800,1,0,0>\n");
      fprintf(f, "  <Grid=10,10,1>\n");
      fprintf(f, "  <DataSet=XXX.dat>\n");
      fprintf(f, "  <DataDisplay=XXX.dpl>\n");
      fprintf(f, "  <OpenDisplay=1>\n");
      fprintf(f, "  <Script=XXX.m>\n");
      fprintf(f, "  <RunScript=0>\n");
      fprintf(f, "  <showFrame=0>\n");
      fprintf(f, "  <FrameText0=Title>\n");
      fprintf(f, "  <FrameText1=Drawn By:>\n");
      fprintf(f, "  <FrameText2=Date:>\n");
      fprintf(f, "  <FrameText3=Revision:>\n");
      fprintf(f, "</Properties>\n");
      fprintf(f, "<Symbol>\n");
      fprintf(f, "</Symbol>\n");
      fprintf(f, "<Components>\n");
      fprintf(f, "%s\n", parsedLibrary.components[i].modelString.toAscii().data()); //<Lib XXXX,... or
      fprintf(f, "<.DC DC1 1 100 210 0 42 0 0 \"26.85\" 0 \"0.001\" 0 \"1 pA\" 0 \"1 uV\" 0 \"no\" 0 \"150\" 0 \"no\" 0 \"none\" 0 \"CroutLU\" 0>\n");
      fprintf(f, "</Components>\n");
      fprintf(f, "<Wires>\n");
      fprintf(f, "</Wires>\n");
      fprintf(f, "<Diagrams>\n");
      fprintf(f, "</Diagrams>\n");
      fprintf(f, "<Paintings>\n");
      fprintf(f, "</Paintings>\n");
      fclose(f);
      QStringList arguments;
      QString stdOut;

      QString qucsProgName = qucslibProgName;
      qucsProgName.replace("/qucslib", "/qucs");
      arguments << "-n" << "-i" << SCH_FILE << "-o" << NET_FILE;
      ret = exeProcess(qucsProgName, arguments, stdOut);
      if (ret!=0){
        fprintf(stderr, "%s: Error %d in %s\n",
            libComp.toAscii().data(),
            ret, qucsProgName.toAscii().data());
        // cat SCH_FILE TODO
      }
      QString qucsatorProgName = qucslibProgName;
      qucsatorProgName.replace("/qucslib", "/qucsator");
      arguments << "--check" << "-i" << NET_FILE; // qucsator --check -i tmp.net
      if (exeProcess(qucsatorProgName, arguments, stdOut)!=0){
        fprintf(log, "\n");
        fprintf(log, "--------------------------%s--------------------------\n", parsedLibrary.name.toAscii().data());
        fprintf(log, "name      : %s\n", parsedLibrary.components[i].name.toAscii().data());
        fprintf(log, "definition: %s\n", parsedLibrary.components[i].definition.toAscii().data());
        fprintf(log, "model     : %s\n", parsedLibrary.components[i].modelString.toAscii().data());
        fprintf(log, "%s\n", stdOut.toAscii().data());
        ok=false;
      }
    }
  }
  fclose(log);
  //
  if (componentName.isEmpty()){
    QFile f1(NET_FILE); f1.remove();
    QFile f2(SCH_FILE); f2.remove();
  }
  return ok? 0 : -1;
}


