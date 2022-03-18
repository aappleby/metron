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

typedef set<FunctionDecl*> FunctionSet;
typedef map<FunctionDecl*, FunctionSet> CallMap;
typedef vector<FieldDecl*> FieldPath;
typedef vector<FieldPath>  FieldPaths;
typedef set<FieldDecl*>    FieldSet;

typedef map<FieldDecl*, FieldSet>    FieldMap;
typedef map<RecordDecl*, FieldPaths> RecordToFieldPaths;

typedef set<NamedDecl*> DeclSet;
typedef vector<DeclSet> DeclStack;


//-----------------------------------------------------------------------------

/*
FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getBeginLoc());
if (FullLocation.isValid())
  llvm::outs() << "Found declaration at "
                << FullLocation.getSpellingLineNumber() << ":"
                << FullLocation.getSpellingColumnNumber() << "\n";
*/
