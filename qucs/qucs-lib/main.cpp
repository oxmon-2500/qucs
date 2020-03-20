/***************************************************************************
                                main.cpp
                               ----------
    begin                : Sat May 28 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
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

#include <QApplication>
#include <QString>
#include <QTextCodec>
#include <QTranslator>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDir>
#include <QFont>
#include <QSettings>
#include <QDebug>
#include <QList>
#include <QProcess>
#include "qucslib.h"
#include "qucslib_common.h"

tQucsSettings QucsSettings;
QDir UserLibDir;
QDir SysLibDir;
int checkComponentLibraries(void); //prototype

int parseLibraries (const QString LibDir, QList<ComponentLibrary> &libList, QString &errText){
  QStringList LibFiles;
  QStringList::iterator it;

  LibFiles = QDir(LibDir).entryList(QStringList("*.lib"), QDir::Files, QDir::Name);

  // create top level library items, base on the library names
  for(it = LibFiles.begin(); it != LibFiles.end(); it++){
      QString libPath(*it);
      libPath.chop(4); // remove extension

      ComponentLibrary parsedlibrary;

      switch(parseComponentLibrary (libPath , parsedlibrary)){
      case QUCS_COMP_LIB_IO_ERROR: //TODO
        break;
      case QUCS_COMP_LIB_CORRUPT:
        break;
      default:
        break;
      }
      libList.insert(libList.end(), parsedlibrary); //add element
  }
  return QUCS_COMP_LIB_OK;
}
// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs");
    // Qucs Library Tool specific settings
    settings.beginGroup("QucsLib");
    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    if(settings.contains("dx"))QucsSettings.dx=settings.value("dx").toInt();
    if(settings.contains("dy"))QucsSettings.dy=settings.value("dy").toInt();
    settings.endGroup();
    // Qucs general settings
    if(settings.contains("QucsHomeDir"))
      if(settings.value("QucsHomeDir").toString() != "")
         QucsSettings.QucsHomeDir.setPath(settings.value("QucsHomeDir").toString());
    if(settings.contains("font"))QucsSettings.font.fromString(settings.value("font").toString());
    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();

  return true;
}


// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings(QucsLib *qucs)
{
    QSettings settings ("qucs","qucs");
    settings.beginGroup("QucsLib");
    settings.setValue("x", qucs->x());
    settings.setValue("y", qucs->y());
    settings.setValue("dx", qucs->width());
    settings.setValue("dy", qucs->height());
    settings.endGroup();
  return true;

}
QString qucslibProgName;

// #########################################################################
// ##########                                                     ##########
// ##########                  Program Start                      ##########
// ##########                                                     ##########
// #########################################################################

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  // apply default settings
  QucsSettings.x = 100;
  QucsSettings.y = 50;
  QucsSettings.dx = 600;
  QucsSettings.dy = 350;
  QucsSettings.font = QFont("Helvetica", 12);
  QucsSettings.QucsHomeDir.setPath(QDir::homePath() + "/.qucs");

  // is application relocated?
  char * var = getenv ("QUCSDIR");
  QDir QucsDir;
  if (var != NULL) {
    QucsDir = QDir(QString(var));
    QucsSettings.LangDir =     QucsDir.canonicalPath() + "/share/qucs/lang/";
    QucsSettings.LibDir =      QucsDir.canonicalPath() + "/share/qucs/library/";
  } else {
    QString QucsApplicationPath = QCoreApplication::applicationDirPath();
#ifdef __APPLE__
    QucsDir = QDir(QucsApplicationPath.section("/bin",0,0));
#else
    QucsDir = QDir(QucsApplicationPath);
    QucsDir.cdUp();
#endif
    QucsSettings.LangDir = QucsDir.canonicalPath() + "/share/qucs/lang/";
    QucsSettings.LibDir  = QucsDir.canonicalPath() + "/share/qucs/library/";
  }

  if(char* qucslibdir=getenv("QUCS_LIBRARY")){
    qDebug() << "setting library" << qucslibdir;
    QucsSettings.LibDir = QDir(QString(qucslibdir)).canonicalPath();
  }else{
  }

  loadSettings();

  SysLibDir.setPath(QucsSettings.LibDir);
  UserLibDir.setPath(QucsSettings.QucsHomeDir.canonicalPath() + "/user_lib/");

  a.setFont(QucsSettings.font);

  if (argc>1 && strcmp(argv[1], "--check-libraries")==0){
    qucslibProgName = argv[0]; // ~/local/qucs/bin/qucslib
    return checkComponentLibraries();
  }
  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty())
    lang = QString(QLocale::system().name());
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  a.installTranslator( &tor );

  QucsLib *qucs = new QucsLib();
  qucs->raise();
  qucs->resize(QucsSettings.dx, QucsSettings.dy); // size and position ...
  qucs->move(QucsSettings.x, QucsSettings.y);     // ... before "show" !!!
  qucs->show();

  int result = a.exec();
  saveApplSettings(qucs);
  delete qucs;
  return result;
}
int exeProcess(const QString processPath, const QStringList arguments, QString &stdOut){
  QProcess simProcess;
  simProcess.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
  simProcess.setReadChannelMode(QProcess::MergedChannels);
  simProcess.start(processPath, arguments);
  simProcess.waitForFinished();
  stdOut = QString(simProcess.readAllStandardOutput().data());
  return simProcess.exitCode();
}
void doNetlist(const char * fi, const char *fo){
  //TODO
}
int checkComponentLibraries(void){
  const char * SCH_FILE="tmp.sch"; //TODO
  const char * NET_FILE="tmp.net";
  const char * LOG_FILE="tmp.log";
  QString retErrText;
  QList<ComponentLibrary> libList;
  if ( parseLibraries(QucsSettings.LibDir, libList, retErrText)!=0){
    fprintf(stderr, "err TODO", retErrText.toAscii().data());
    return -1;
  }

  bool ok=true;
  FILE *log = fopen(LOG_FILE, "w");
  for(ComponentLibrary parsedLibrary : libList){
    for (int i = 0; i < parsedLibrary.components.count (); i++){
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
      //doNetlist(SCH_FILE, NET_FILE);

      QString qucsProgName = qucslibProgName;
      qucsProgName.replace("/qucslib", "/qucs");
      arguments << "-n" << "-i" << SCH_FILE << "-o" << NET_FILE;
      if (exeProcess(qucsProgName, arguments, stdOut)!=0){
        //TODO
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
  //TODO rm NET_FILE, rm SCH_FILE
  return ok? 0 : -1;
}


