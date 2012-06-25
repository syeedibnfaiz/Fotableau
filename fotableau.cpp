
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <queue>
using namespace std;
/*
==============================================================
						Term
==============================================================
*/
class TermList;

class Term
{
private:
	int type;
	char *name;
	bool free;

	TermList *termList;
	static int varCount;
	static int funcCount;
public:
	const static int CONS = 1;
	const static int VAR = 2;
	const static int FUNC = 4;

	Term(int type, char *name);
	Term(char *name, TermList *termList);
	Term(Term &term);
	~Term();
	
	int getType();
	char* getName();
	TermList* getTermList();
	void print();

	void setName(char *name);
	void setType(int type);
	void setTermList(TermList *list);

	static int nextVarCount();
	static int nextFuncCount();

	bool isFree();
	bool operator==(Term &term);
	bool operator!=(Term &term);
};

class TermList
{
private:
	vector<Term*> *list;
public:
	TermList();
	TermList(TermList &tList);
	
	vector<Term*> *getList();
	void addTerm(Term *term);
	void replace(Term *oldV, Term *newV);
	void getFreeVarList(TermList *list);
	void print();

	bool operator==(TermList &list);
	bool operator!=(TermList &list);
};

/*
* Constructors for constants and variables
*/
Term::Term(int type, char *name)
{
	this->type = type;
	if (name != NULL) {
		this->free = false;
		this->name = new char[strlen(name) + 1];
		strcpy(this->name, name);
	}
	else {
		this->free = true;
		char tmp[20];
		sprintf(tmp, "v%d", this->nextVarCount());	
		this->name = new char[strlen(tmp) + 1];
		strcpy(this->name, tmp);
	}
}

/*
* Constructor for functions
*/
Term::Term(char *name, TermList *termList)
{
	this->type = this->FUNC;
	if (name != NULL) {
		this->name = new char[strlen(name) + 1];
		strcpy(this->name, name);
	}
	else {
		char tmp[20];
		sprintf(tmp, "f%d", this->nextFuncCount());		
		this->name = new char[strlen(tmp) + 1];
		strcpy(this->name, tmp);
	}
	this->termList = termList;
}
/*
* Copy Constructor
*/
Term::Term(Term &term)
{	
	this->free = term.free;
	this->type = term.type;
	this->name = new char[strlen(term.name) + 1];
	strcpy(this->name, term.getName());

	if (this->type == FUNC) {
		this->termList = new TermList();
		for (int i = 0; i < term.getTermList()->getList()->size(); i++) {
			Term *t = new Term(*(term.getTermList()->getList()->at(i)));
			this->termList->addTerm(t);
		}
	}
}
Term::~Term() 
{
	//printf("destructor called for ");
	//this->print();
	//printf("\n");
	delete [] name;	
}
int Term::getType()
{
	return type;
}

char* Term::getName()
{
	return name;
}

TermList* Term::getTermList()
{
	return termList;
}
void Term::setName(char *name)
{
	delete [] this->name;
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
}
void Term::setType(int type)
{
	this->type = type;
}
void Term::setTermList(TermList *list)
{
	this->termList = list;
}
bool Term::isFree()
{
	return this->free;
}
void Term::print()
{
	printf("%s", name);
	if (this->type == this->FUNC) {
		this->termList->print();
	}
}

TermList::TermList()
{
	this->list = new vector<Term*>();
}
TermList::TermList(TermList &tList)
{
	this->list = new vector<Term*>();
	for (int i = 0; i < tList.getList()->size(); i++) {
		Term *t = new Term(*(tList.getList()->at(i)));
		this->list->push_back(t);
	}
}
void TermList::addTerm(Term *term)
{
	this->list->push_back(term);
}
vector<Term*> *TermList::getList()
{
	return this->list;
}
void TermList::replace(Term *oldV, Term *newV)
{
	int sz = this->list->size();
	for (int i = 0; i < sz; i++) {
		Term *t = this->list->at(i);
		if (*t == *oldV) {
			this->list->at(i) = newV;
			//delete t; //TO DO
		}
		else if (t->getType() == Term::FUNC) {
			t->getTermList()->replace(oldV, newV);
		}
	}
}
void TermList::getFreeVarList(TermList *list)
{
	int sz = this->list->size();
	for (int i = 0; i < sz; i++) {
		Term *t = this->list->at(i);
		if (t->getType() == Term::VAR && t->isFree()) {
			list->addTerm(t);
		}
		else if (t->getType() == Term::FUNC) {
			t->getTermList()->getFreeVarList(list);
		}
	}
}
void TermList::print()
{
	printf("(");
	for (int i = 0; i < this->list->size(); i++) {
		if(i) printf(", ");
		(this->list->at(i))->print();
	}
	printf(")");
}
bool TermList::operator==(TermList &list)
{
	if (this->list->size() != list.getList()->size()) return false;
	for (int i = 0; i < this->list->size(); i++) {
		if (*(this->list->at(i)) != *(list.getList()->at(i))) return false;
	}
	return true;
}
bool TermList::operator != (TermList &list)
{
	return !(*(this) == list);
}
int Term::nextVarCount()
{
	static int count = 999;
	count++;
	return count;
}
int Term::nextFuncCount()
{
	static int count = 2222;
	count++;
	return count;
}
bool Term::operator ==(Term &term)
{
	if (this->type != term.type || strcmp(this->name, term.getName())) return false;
	if (this->type == FUNC) {
		return *(this->termList) == *(term.getTermList());
	}
	else {
		return true;
	}
}
bool Term::operator != (Term &term)
{
	return !(*(this) == term);
}
/*
===================================================
				End of Term
===================================================
*/
/*
===================================================
				Predicate
===================================================
*/
class Predicate
{
private:
	char *name;
	TermList *termList;
public:
	Predicate(char *name, TermList *termList);
	char* getName();
	TermList* getTermList();
	void print();
};
Predicate::Predicate(char *name, TermList *termList)
{
	this->name = new char[strlen(name) + 1];
	strcpy(this->name,name);
	this->termList = termList;
}
char* Predicate::getName()
{
	return name;
}
TermList* Predicate::getTermList()
{
	return this->termList;
}
void Predicate::print()
{
	printf("%s", name);
	this->termList->print();
}
/*
===================================================
				End of Predicate
===================================================
*/
/*
===================================================
				Formula
===================================================
*/
class Formula
{
private:
public:
	const static int ATOMIC = 1;
	const static int NEGATED = 2;
	const static int COMPOUND = 4;
	const static int QUANTIFIED = 8;
	const static int UNIV = 24;
	const static int EXIST = 40;

	char cntv;
	int type;
	Term *qVar;
	Predicate *predicate;
	Formula *formula1;
	Formula *formula2;
	Formula *nextFormula;

	Formula(Predicate *predicate);
	Formula(Formula *nextFormula, bool negated=true);
	Formula(Formula *left, Formula *right, char cntv);
	Formula(Term *qVar, Formula *nextFormula, int type);
	Formula(Formula &f);

	char getCntv();
	Term* getQVar();
	int getType();
	Predicate* getPredicate();
	Formula* getLeft();
	Formula* getRight();
	Formula* getNext();
	void getFreeVarList(TermList *list);

	bool isAtomic();
	bool isNegated();
	bool isCompound();
	bool isQuantified();
	bool isUniversal();
	bool isExistential();

	bool isAlpha();
	bool isBeta();
	bool isGamma();
	bool isDelta();

	Formula* getAlpha1();
	Formula* getAlpha2();
	Formula* getBeta1();
	Formula* getBeta2();
	Formula* getGamma(Term *newV);
	Formula* getDelta(Term *newF = NULL);

	void replace(Term *oldV, Term *newV);
	void copy(TermList *frm, TermList *to);
	void print();

};
/*
* Constructors
*/
Formula::Formula(Predicate *predicate)
{
	this->predicate = predicate;
	this->type = ATOMIC;
}
Formula::Formula(Formula *next, bool negated)
{
	this->nextFormula = next;
	this->type = NEGATED;
}
Formula::Formula(Formula *left, Formula *right, char cntv)
{
	this->formula1 = left;
	this->formula2 = right;
	this->cntv = cntv;
	this->type = COMPOUND;	
}
void Formula::copy(TermList *frm, TermList *to)
{
	int sz = frm->getList()->size();
	for (int i = 0; i < sz; i++) {
		Term *t = frm->getList()->at(i);
		if (t->getType() == Term::CONS) {
			to->addTerm(t);
		}
		else if (t->getType() == Term::VAR) {
			if (t->isFree()) {
				to->addTerm(t);
			}
			else {
				Term *x = new Term(*t);
				to->addTerm(x);
			}
		}
		else {
			TermList *list = new TermList();
			this->copy(t->getTermList(), list);
			Term *f = new Term(t->getName(), list);
			to->addTerm(f);
		}
	}
}
/*
* Special Copy Constructor
* Only pointers to free vars are retained
* Used for copying gamma formulas
*/
Formula::Formula(Formula &f)
{
	this->type = f.getType();
	this->cntv = f.getCntv();
	if (f.isAtomic()) {		
		TermList *list = new TermList();
		this->copy(f.getPredicate()->getTermList(), list);
		this->predicate = new Predicate(f.getPredicate()->getName(), list);
	}
	else if (f.isNegated()) {		
		this->nextFormula = new Formula(*f.getNext());
	}
	else if (f.isQuantified()) {
		this->qVar = new Term(*f.getQVar());		
		this->nextFormula = new Formula(*f.getNext());
	}
	else if (f.isCompound()) {
		this->formula1 = new Formula(*f.getLeft());
		this->formula2 = new Formula(*f.getRight());
	}
}
Formula::Formula(Term *qVar, Formula *next, int type)
{
	this->qVar = qVar;
	this->nextFormula = next;
	this->type = type;
}
char Formula::getCntv()
{
	return this->cntv;
}
Term* Formula::getQVar()
{
	return this->qVar;
}
int Formula::getType()
{
	return this->type;
}
Predicate* Formula::getPredicate()
{
	return this->predicate;
}
Formula* Formula::getLeft()
{
	return this->formula1;
}
Formula* Formula::getRight()
{
	return this->formula2;
}
Formula* Formula::getNext()
{
	return this->nextFormula;
}
void Formula::getFreeVarList(TermList *list)
{	
	if (this->isAtomic()) {
		this->getPredicate()->getTermList()->getFreeVarList(list);
	}
	else if (this->isNegated() || this->isQuantified()) {
		this->nextFormula->getFreeVarList(list);
	}
	else if (this->isCompound()) {
		this->formula1->getFreeVarList(list);
		this->formula2->getFreeVarList(list);
	}
}
bool Formula::isAtomic()
{
	if (this->type & ATOMIC) return true;
	else return false;
}
bool Formula::isNegated()
{
	if (this->type & NEGATED) return true;
	else return false;
}
bool Formula::isCompound()
{
	if (this->type & COMPOUND) return true;
	else return false;
}
bool Formula::isQuantified()
{
	if (this->type & QUANTIFIED) return true;
	else return false;
}
bool Formula::isUniversal()
{
	if (this->type == UNIV) return true;
	else return false;
}
bool Formula::isExistential()
{
	if (this->type == EXIST) return true;
	else return false;
}
bool Formula::isAlpha()
{
	if (this->isCompound() && this->cntv == '&') {
		return true;
	}
	else if (this->isNegated()) {
		Formula *next = this->getNext();
		if (next->isCompound() && (next->getCntv()=='|' || next->getCntv()=='>')) {
			return true;
		}
		else return false;
	}
	else return false;
}
bool Formula::isBeta()
{
	if (this->isCompound() && (this->cntv == '|' || this->cntv=='>')) {
		return true;
	}
	else if (this->isNegated()) {
		Formula *next = this->getNext();
		if (next->isCompound() && next->getCntv()=='&') {
			return true;
		}
		else return false;
	}
	else return false;
}
bool Formula::isGamma()
{
	if ((this->type == UNIV) || (this->isNegated() && this->nextFormula->isExistential())) return true;
	else return false;
}
bool Formula::isDelta()
{
	if ((this->type & EXIST) || (this->isNegated() && this->nextFormula->isUniversal())) return true;
	else return false;
}

Formula* Formula::getAlpha1()
{
	if (this->isCompound()) {
		return this->formula1;
	}
	else if (this->nextFormula->getCntv() == '|') {
		Formula *next = this->nextFormula;
		return new Formula(next->getLeft());
	}
	else {
		Formula *next = this->nextFormula;
		return next->getLeft();
	}
}
Formula* Formula::getAlpha2()
{
	if (this->isCompound()) {
		return this->formula2;
	}
	else {
		Formula *next = this->nextFormula;
		return new Formula(next->getRight());
	}
}
Formula* Formula::getBeta1()
{
	if (this->isCompound()) {
		if (this->getCntv() == '|') {
			return this->formula1;
		}
		else{
			return new Formula(this->formula1);
		}
		
	}
	else {
		Formula *next = this->nextFormula;
		return new Formula(next->getLeft());
	}
}
Formula* Formula::getBeta2()
{
	if (this->isCompound()) {
		return this->formula2;
	}
	else if (this->nextFormula->getCntv() == '&') {
		Formula *next = this->nextFormula;
		return new Formula(next->getRight());
	}
	else {
		Formula *next = this->nextFormula;
		return next->getRight();
	}
}
Formula* Formula::getGamma(Term *newV)
{
	Formula *f;
	Term *v;
	if (this->type == UNIV) {
		f = this->nextFormula;
		v = this->qVar;
	}
	else {
		f = new Formula(this->nextFormula->getNext());
		v = this->nextFormula->getQVar();
	}

	f->replace(v, newV);
	return f;
}
Formula* Formula::getDelta(Term *newFunc)
{
	Formula *f;
	Term *v;
	if (this->type == EXIST) {
		f = this->nextFormula;
		v = this->qVar;
	}
	else {
		f = new Formula(this->nextFormula->getNext());
		v = this->nextFormula->getQVar();
	}

	if (newFunc == NULL) {

		TermList *list = new TermList();
		f->getFreeVarList(list);

		newFunc = new Term(NULL, list);

	}

	f->replace(v, newFunc);
	return f;


}
void Formula::replace(Term *oldV, Term *newV)
{
	if (this->isAtomic()) {
		this->getPredicate()->getTermList()->replace(oldV, newV);
	}
	else if (this->isNegated() || this->isQuantified()) {
		this->getNext()->replace(oldV, newV);
	}
	else if (this->isCompound()) {
		this->formula1->replace(oldV, newV);
		this->formula2->replace(oldV, newV);
	}
	else {
		puts("\nError..wht fmla is this?\n");
	}
}
void Formula::print()
{
	if (this->isAtomic()) {
		this->predicate->print();
	}
	else if (this->isNegated()) {
		printf("~");
		this->nextFormula->print();
	}
	else if(this->isCompound()) {
		printf("(");
		this->formula1->print();
		printf(" %c ", this->cntv);
		this->formula2->print();
		printf(")");
	}
	else if (this->isQuantified()) {
		if (this->isUniversal()) {
			printf("all(");
		}
		else {
			printf("exists(");
		}
		this->qVar->print();
		printf(", ");
		this->nextFormula->print();
		printf(")");
	}
}
/*
===================================================
				End of Formula
===================================================
*/
/*
===================================================
					Parser
		A recursive decent parser for FOL
===================================================
*/

class Parser
{
private:
	char *p;
public:
	Parser(char *p);
	Formula* parse();
	Formula* parseF1(char cntv);
	Formula* parseF2();
	Formula* parsePredicate();
	void parseName(char *store);
	TermList* parseTermList();
	Term* parseTerm();
	Formula* parseForSome();
	Formula* parseForAll();
};
Parser::Parser(char *p)
{
	this->p = p;
}
Formula* Parser::parse()
{
	return this->parseF1('>');
}
Formula* Parser::parseF1(char cntv)
{
	char *startPtr = p;
	char *nextPtr = NULL;
	int bCount = 0;

	if (!*p) throw "incomplete formula";

	while (*p) {
		if (*p == '(') bCount++;
		else if (*p == ')') bCount--;
		else if (bCount == 0 && *p == cntv) {
			*p = 0;
			nextPtr = p + 1;
			break;
		}
		p++;
	}
	if (nextPtr == NULL) {
		p = startPtr;
		if (cntv == '>') return this->parseF1('|');
		else if (cntv == '|') return this->parseF1('&');
		else return this->parseF2();
	}
	else {
		Formula *left;
		p = startPtr;
		if (cntv == '>') left = this->parseF1('|');
		else if (cntv == '|') left = this->parseF1('&');
		else left =  this->parseF2();

		p = nextPtr;
		Formula *right = this->parseF1(cntv);

		Formula *newFormula = new Formula(left, right, cntv);

		return newFormula;
	}	
}
Formula* Parser::parseF2()
{
	while (*p) {
		if (*p == '(') {
			p++;
			Formula *f = this->parse();
			while(*p && *p != ')') p++;
			return f;
		}
		else if (*p == '~') {
			p++;
			Formula *next = this->parseF2();
			Formula *newFormula = new Formula(next);
			return newFormula;
		}
		else if (isalpha(*p) && isupper(*p)) {
			return this->parsePredicate();
		}
		else if (isalpha(*p) && islower(*p)) {
			char temp[10];
			this->parseName(temp);
			if (!strcmp(temp, "all")) {
				return this->parseForAll();
			}
			else if (!strcmp(temp, "exists")) {
				return this->parseForSome();
			}
			else {
				throw "Predicate expected..";
				return NULL;
			}
		}
		else if (!isspace(*p)){
			throw "unexpected character..";
			return NULL;
			//printf("extra: %c\n", *p);
		}
		p++;
	}
}
Formula* Parser::parsePredicate()
{
	char temp[10];
	this->parseName(temp);	

	TermList *list = this->parseTermList();
	Predicate *prd = new Predicate(temp, list);
	Formula *f = new Formula(prd);
	return f;
}
void Parser::parseName(char *store)
{
	while(isalnum(*p)) {
		*store = *p;
		p++;
		store++;
	}
	*store = 0;
}
TermList* Parser::parseTermList()
{
	TermList *list = NULL;
	while(isspace(*p)) p++;

	if (*p == '(') {
		list = new TermList();
		while (*p && *p != ')') {
			if (isalpha(*p)) {
				Term *t = this->parseTerm();
				list->addTerm(t);
				p--;
			}
			p++;
		}
		if (*p == ')') p++;
	}
	return list;
}
Term* Parser::parseTerm()
{
	char temp[10];
	this->parseName(temp);
	
	if (isupper(temp[0])) {
		Term *t = new Term(Term::CONS, temp);
		return t;
	}
	
	while (isspace(*p)) p++;

	if(*p == '(') {
		TermList *list = this->parseTermList();
		Term *t = new Term(temp, list);
		return t;
	}
	else {
		Term *t = new Term(Term::VAR, temp);
		return t;
	}
}
Formula* Parser::parseForAll()
{
	while(*p && *p != '(') p++;
	p++;
	Term *t = this->parseTerm();

	while(*p && *p != ',') p++;
	p++;
	Formula *f = this->parse();

	Formula *newFormula = new Formula(t, f, Formula::UNIV);
	
	while(*p && *p != ')') p++;
	p++;

	return newFormula;
}
Formula* Parser::parseForSome()
{
	while(*p && *p != '(') p++;
	p++;
	Term *t = this->parseTerm();

	while(*p && *p != ',') p++;
	p++;
	Formula *f = this->parse();

	Formula *newFormula = new Formula(t, f, Formula::EXIST);
	
	while(*p && *p != ')') p++;
	p++;
	return newFormula;
}
/*
===================================================
				End of Parser
===================================================
*/
/*
===================================================
					Prover
===================================================
*/
class Prover
{
private:
	int qDepth;
public:
	Prover(int qDepth = 1);
	bool prove(Formula *formula, int qDepth, int tabCount = 12);
	bool prove(char *p);
	bool expand(queue<Formula*> &q, int qDepth, int tabCount);
	bool isClosed(queue<Formula*> &q, int tabCount);
	bool isUnifiable(TermList *l1, TermList *l2);
	bool isUnifiable(Predicate *p1, Predicate *p2, int tabCount);
	bool unify(TermList *l1, TermList *l2);
	bool unify(Term *l1, Term *l2);
	bool occursIn(Term *t1, TermList *list);
	void addVars(TermList *l1,vector<Term*> &varList );
	void print(queue<Formula*> &q, int tabCount);
	void printTab(int tabCount);
};
Prover::Prover(int qDepth)
{
	this->qDepth = qDepth;
}
bool Prover::prove(char *p)
{
	Parser parser(p);
	Formula *formula;
	bool b;
	char tmp[1000];
	strcpy(tmp, p);

	try 
	{
		formula = parser.parse();
		b = this->prove(formula, this->qDepth);
		if (b) {
			printf("****************************\n");
			printf("%s is valid.\n", tmp);
			printf("****************************\n");
		}
		else {
			printf("****************************\n");
			printf("%s is not valid.\n", tmp);
			printf("****************************\n");
		}
		return b;
	}
	catch(const char *cause)
	{
		printf("Error occured: %s\n", cause);
		return false;		
	}
	catch(...)
	{
		printf("Error occured");
		return false;
	}
}
bool Prover::prove(Formula *formula, int depth, int tabCount)
{
	Formula *newFormula = new Formula(formula);

	//printf("Expanding...");
	//newFormula->print();
	//puts("");

	queue<Formula*> q;
	q.push(newFormula);
	return expand(q, depth, tabCount);
}
bool Prover::expand(queue<Formula*> &q, int depth, int tabCount)
{
	int sz = q.size();
	//printf("\n\n============Expanding==============\n\n");
	//this->print(q);

	this->print(q, tabCount);

	for (int i = 0; i < sz; ++i) {
		Formula *f = q.front();
		q.pop();

		if (f->isAtomic()) {
			q.push(f);
			/*if (this->isClosed(q)) {
				//printf("caught closeness early on!!");
				this->print(q, tabCount);
				return true;
			}*/
		}
		else if (f->isAlpha()) {
			//printf("Applying alpha rule for..");
			//f->print();
			//puts("");
			this->printTab(tabCount + 2);
			printf("|alpha (%d)\n", i + 1);

			Formula *f1 = f->getAlpha1();
			Formula *f2 = f->getAlpha2();
			q.push(f1);
			q.push(f2);
			return expand(q, depth, tabCount);
		}
		else if (f->isBeta()) {
			//printf("Applying beta rule for..");
			//f->print();
			//puts("");
			this->printTab(tabCount+2);
			printf("|beta (%d)\n", i + 1);

			Formula *f1 = f->getBeta1();
			Formula *f2 = f->getBeta2();

			queue<Formula*> q2(q);
			q.push(f1);
			q2.push(f2);

			return expand(q, depth, tabCount-4) && expand(q2, depth, tabCount+4);
		}
		else if (f->isGamma()) {			

			//printf("Applying gamma rule for..");
			//f->print();
			//puts("");

			this->printTab(tabCount+2);
			printf("|gamma (%d)\n", i + 1);

			depth--;
			Formula *cpy = NULL;
			if (depth > 0) {
				cpy = new Formula(*f);
				//printf("\n\ncopy formula1======\n");
				//cpy->print();
			}

			Term *newVar = new Term(Term::VAR, NULL);
			Formula *newFormula;
			newFormula = f->getGamma(newVar);

			//printf("got this..");
			//newFormula->print();
			//puts("");

			q.push(newFormula);
			if (cpy != NULL) {
				q.push(cpy);
				//printf("\n\ncopy formula2======\n");
				//cpy->print();
			}
			return expand(q, depth, tabCount);
		}
		else if (f->isDelta()) {			

			//printf("Applying delta rule for..");
			//f->print();
			//puts("");
			
			this->printTab(tabCount + 2);
			printf("delta (%d)\n", i + 1);

			Formula *newFormula;
			newFormula = f->getDelta();

			//printf("got this..");
			//newFormula->print();
			//puts("");

			q.push(newFormula);
			return expand(q, depth, tabCount);
		}
		else if (f->isNegated()) {
			if (f->getNext()->isAtomic()) {
				q.push(f);
			}
			else if(f->getNext()->isNegated()) {
				//printf("Applying alpha rule for..");
				//f->print();
				//puts("");

				this->printTab(tabCount + 2);
				printf("alpha (%d)\n", i + 1);

				q.push(f->getNext()->getNext());
				return expand(q, depth, tabCount);
			}
		}
		
	}
	bool b = this->isClosed(q, tabCount);
	//if (b) printf("A closed branch..\n");
	//else printf("An open branch..\n");

	//if (b) this->print(q, tabCount);
	return b;
}
bool Prover::isClosed(queue<Formula*> &q, int tabCount)
{
	vector<Formula*> aList;
	vector<Formula*> ntAList;
	int sz = q.size();

	for (int i = 0; i < sz; ++i) {
		Formula *f = q.front();
		q.pop();
		q.push(f);
		if (f->isAtomic()) {
			aList.push_back(f);
		}
		else if (f->isNegated() && f->nextFormula->isAtomic()) {
			ntAList.push_back(f->nextFormula);
		}
	}

	for (int i = 0; i < aList.size(); i++) {
		Formula *f1 = aList.at(i);
		for (int j = 0; j < ntAList.size(); j++) {
			Formula *f2 = ntAList.at(j);
			if (!strcmp(f1->getPredicate()->getName(), f2->getPredicate()->getName())) {
				//if (isUnifiable(f1->getPredicate()->getTermList(), f2->getPredicate()->getTermList())) {
				if (isUnifiable(f1->getPredicate(), f2->getPredicate(), tabCount)) {
					return true;
				}
			}
		}
	}
	return false;
}
bool Prover::isUnifiable(Predicate *p1, Predicate *p2, int tabCount)
{
	bool b = isUnifiable(p1->getTermList(), p2->getTermList());
	if (b) {
		this->printTab(tabCount);
		p1->print();
		printf(" and ");
		p2->print();
		printf(" are unified\n");
		
		b = this->unify(p1->getTermList(), p2->getTermList());
	}
	return b;
}
bool Prover::isUnifiable(TermList *l1, TermList *l2)
{
	TermList *tmp1, *tmp2;
	tmp1 = new TermList(*(l1));
	tmp2 = new TermList(*(l2));

	vector<Term*> varList;
	addVars(tmp1, varList);
	addVars(tmp2, varList);

	bool b =  unify(tmp1, tmp2);
	/*if (b == true) {
		//varList = vector<Term*>();
		//addVars(l1, varList);
		//addVars(l2, varList);
		b = unify(l1, l2);
	}*/
	
	delete tmp1;
	delete tmp2;

	return b;
}
bool Prover::unify(TermList *l1, TermList *l2)
{
	if (l1->getList()->size() != l2->getList()->size()) return false;

	for (int i = 0; i < l1->getList()->size(); i++) {
		Term *t1 = l1->getList()->at(i);
		Term *t2 = l2->getList()->at(i);
		if (!unify(t1, t2)) return false;
	}
	return true;
}
void Prover::addVars(TermList *l1,vector<Term*> &varList ) 
{
	for (int i = 0; i < l1->getList()->size(); i++) {
		Term *t1 = l1->getList()->at(i);
		if (t1->getType() == Term::VAR) {
			bool found = false;
			for (int j = 0; j < varList.size(); j++) {
				if (*(t1) == *(varList.at(j))) {
					delete t1;
					l1->getList()->at(i) = varList.at(j);
					found = true;
					break;
				}
			}
			if (!found) {
				varList.push_back(t1);
			}
		}
		else if (t1->getType() == Term::FUNC) {
			this->addVars(t1->getTermList(), varList);
		}		
	}
}
bool Prover::occursIn(Term *t, TermList *list)
{
	for (int i = 0; i < list->getList()->size(); i++) {
		Term *p = list->getList()->at(i);
		if (*t == *p) {
			return true;
		}
		else if ((p->getType() == Term::FUNC) && this->occursIn(t, p->getTermList())) {
			return true;
		}
	}
	return false;
}
bool Prover::unify(Term *t1, Term *t2)
{
	if (*(t1) == *(t2)) return true;
	else if (t1->getType() == t2->getType()) {
		if (t1->getType() == Term::VAR) {
			t1->setName(t2->getName());
			return true;
		}
		else if ((t1->getType() == Term::FUNC) && !strcmp(t1->getName(), t2->getName())) {
			return this->unify(t1->getTermList(), t2->getTermList());
		}
		else {
			return false;
		}
	}
	else if (t1->getType() == Term::VAR) {		
		if ((t2->getType() == Term::FUNC) && this->occursIn(t1, t2->getTermList())) {
			return false;
		}
		t1->setType(t2->getType());
		t1->setName(t2->getName());
		t1->setTermList(t2->getTermList());
		return true;
	}
	else if (t2->getType() == Term::VAR) {		
		if ((t1->getType() == Term::FUNC) && this->occursIn(t2, t1->getTermList())) {
			return false;
		}
		t2->setType(t1->getType());
		t2->setName(t1->getName());
		t2->setTermList(t1->getTermList());
		return true;
	}
	else {
		return false;
	}
}
void Prover::printTab(int tabCount)
{
	for (int i = 0; i < tabCount; i++) {
		printf("\t");
	}
}
void Prover::print(std::queue<Formula*> &q, int tabCount)
{
	puts("\n");
	this->printTab(tabCount);
	printf("========================\n");
	int sz = q.size();
	for (int i = 0; i < sz; i++) {
		Formula *f = q.front();
		q.pop();
		this->printTab(tabCount+1);
		printf("(%d):\t", i+1);
		f->print();
		puts("");
		q.push(f);
	}
	this->printTab(tabCount);
	printf("========================\n");
}
/*
===================================================
				End of Prover
===================================================
*/
void testTerms()
{
	int n;
	int m;
	int type;
	char name[10];

	for (int i = 0; i < 5; i++) {
		printf("type and Name: ");
		scanf("%d %s", &type, name);

		if(type == Term::FUNC) {
			
			TermList *list = new TermList();
			Term term(name, list);

			printf("place: ");
			scanf("%d", &m);

			for (int j = 0; j < m; ++j) {
				printf("%dth term: ",j); 
				scanf("%s", name);
				Term *term = new Term(Term::VAR, name);
				list->addTerm(term);
			}

			term.print();
		}
		else {
			Term term(type, name);
			term.print();
		}
	}
}
void formulaTest()
{
	TermList *list = new TermList();
	list->addTerm(new Term(Term::VAR, "x"));
	list->addTerm(new Term(Term::VAR, NULL));
	list->addTerm(new Term(Term::CONS, "A"));
	Formula *f = new Formula(new Predicate("P", list));
	Formula *g = new Formula(f);
	Formula *q = new Formula(f, g, '&');
	
	Formula *r = new Formula(new Term(Term::VAR, NULL), q, Formula::UNIV);
	r->print();

	Term *x = new Term(Term::VAR, "xy");
	Term *y = new Term(Term::VAR, "x");

	TermList *list2 = new TermList();
	list2->addTerm(x);
	list2->addTerm(y);
	Term *f2 = new Term("f10", list2);
	Term *g2 = new Term("f10", list2);

	f2->print();
	puts("");
	g2->print();
	puts("");
	if (!(*f2 != *g2)) {
		printf("yes! equal!");
	}

}
void ptrTest(char *p, int step)
{
	if (*p == 0) return;
	printf("%d: %c\n",step, *p);
	p++;
	ptrTest(p, step + 1);
	printf("%d: %c\n",step, *p);
}
void formulaFeatureTest(Formula *f)
{
	if (f->isAlpha()) {
		printf(" alpha\n");
		f->getAlpha1()->print();
		puts("");
		f->getAlpha2()->print();
		puts("");
	}
	else if (f->isBeta()) {
		printf(" beta\n");
		f->getBeta1()->print();
		puts("");
		f->getBeta2()->print();
		puts("");
	}
	else if (f->isGamma()) {
		printf(" gamma\n");
		f->getGamma(new Term(Term::VAR, NULL))->print();
		puts("");
	}
}
//
void parseTest()
{
	char str[100];
	while (gets(str)) {
		Parser p(str);

		Formula *f = p.parse();
		f->print();
		formulaFeatureTest(f);
	}
}
void copyConsTest()
{
	char str[100];
	while (gets(str)) {
		Parser p(str);

		Formula *f = p.parse();
		f->print();
		puts("");
		
		Predicate *pred = f->getPredicate();
		Term *t = pred->getTermList()->getList()->at(0);

		puts("here");
		Term w = *t;
		w.print();
		puts("here");
		w.setName("chan");
		w.print();
		puts("");

		f->print();
		puts("");
	}
}
void vectorTest()
{
	vector<char *> v1, v2;
	v1.push_back("one");
	v1.push_back("two");

	v2 = vector<char*>(v1);
	printf("%s %s\n", v2.at(0), v2.at(1));
	
}
void proverTest1()
{
	Prover prover(2);
	char str[100];

	while (gets(str)) {
		prover.prove(str);	
	}
}
void unifyTest()
{
	
	char str[100];	

	while (gets(str)) {
		Parser par(str);
		TermList *l1 = par.parseTermList();
		gets(str);
		par = Parser(str);
		TermList *l2 = par.parseTermList();

		Prover pr;
		l1->print();
		printf(" and ");
		l2->print();
		puts("");
		if (pr.isUnifiable(l1, l2)) {
			printf("Unifiable\n");
		}
		else {
			printf("not Unifiable\n");
		}
		printf("Afterwards....");
		l1->print();
		printf(" and ");
		l2->print();
		puts("");
	}
}
void run()
{
	int qDepth;
	char input[200];

	qDepth = 1;
	printf("Enter q-depth [1]: ");
	gets( input );
	if ( input[0] ) {
		qDepth = atoi(input);
	}

	printf("Do you want to redirect output to a file? [y/n] : ");
	gets( input );
	if ( input[0] == 'y' ) {		
		printf("Enter file name [output.txt]: ");
		gets( input );
		if ( input[0] ) {			
			printf("All outputs will be redirected to %s.\n", input);
			printf("Enter one or more formulas, each on a separate line. Blank line to stop.\n");
			freopen(input, "w", stdout);
		}
		else {			
			printf("All outputs will be redirected to output.txt.\n");
			printf("Enter one or more formulas, each on a separate line. Blank line to stop.\n");
			freopen("output.txt", "w", stdout);
		}
	}
	else {
		printf("Enter one or more formulas, each on a separate line. Blank line to stop.\n");
	}

	Prover prover(qDepth);
	while (true) {		
		gets( input );
		if (!input[0]) break;
		else {
			prover.prove( input );
		}
	}

}
int main()
{
	//freopen("output.txt", "w", stdout);
	//testTerms();
	//formulaTest();
	//ptrTest("abc", 1);
	//parseTest();
	//copyConsTest();
	//vectorTest();
	//proverTest1();
	//unifyTest();
	
	run();

	return 0;
}

