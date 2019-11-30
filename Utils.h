#pragma once

//-----------------------------------------------------------------------------

void indent();
void dedent();

struct Indenter {
  Indenter()  { indent(); }
  ~Indenter() { dedent(); }
};

void dprintf(const char* format, ...);
void sprintf(string& out, const char* format, ...);

//-----------------------------------------------------------------------------

string toString(Decl* decl);
string toString(Stmt* stmt);

//-----------------------------------------------------------------------------

/*
FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
if (FullLocation.isValid())
  llvm::outs() << "Found declaration at "
                << FullLocation.getSpellingLineNumber() << ":"
                << FullLocation.getSpellingColumnNumber() << "\n";
*/
