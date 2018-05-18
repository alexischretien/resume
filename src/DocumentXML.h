/*
 * LecteurXML.h
 *
 *  Created on: Mar 18, 2017
 *      Author: Bruno Malenfant
 */

#ifndef DOCUMENTXML_H_
#define DOCUMENTXML_H_

#include <iostream>
#include <map>
#include <vector>
#include <string>

using namespace std;


class Contenu {
public :
	Contenu( void );
	Contenu( const Contenu & a_contenu );
	virtual ~Contenu( void );

	virtual bool estElement( void ) const;
    virtual string texte( void ) const;
	virtual ostream & afficher( ostream & ) const;

	friend ostream & operator << ( ostream &, const Contenu & );
};


class Texte : public Contenu {
private :
	string _texte;
public :
	Texte( void );
	Texte( string a_texte );
	Texte( const Texte & a_texte );
	virtual ~Texte( void );

	string texte( void ) const;

	ostream & afficher( ostream & ) const;
};


class AttributNonDefinie : public exception
{
private :
  string _nom;

public :
  AttributNonDefinie( void );
  AttributNonDefinie( string a_nom );
  AttributNonDefinie( const AttributNonDefinie & a_e );
  virtual ~AttributNonDefinie( void );

  virtual const char* what() const throw();
};


class Element : public Contenu {
private :
	string _nom;
	Element * _parent;
	map< string, string > _attributs;
	vector< Contenu * > _enfants;
public :
	Element( void );
	Element( Element * a_parent, string a_nom );
	Element( const Element & a_element );
	virtual ~Element( void );

	Texte * ajouterTexte( string a_texte );
	Element * ajouterElement( string a_nom );
	void ajouterAttribut( string a_nom, string a_valeur );

	bool estElement( void ) const;
    string texte( void ) const;
	Element * parent( void ) const;
	string nom( void ) const;
	string attribut( string a_nom ) const throw( AttributNonDefinie );

    vector< Contenu * >::const_iterator begin( void ) const;
    vector< Contenu * >::const_iterator end( void ) const;

	virtual ostream & afficher( ostream & ) const;
};



class DocumentXML {
private :
	Element * _racine;
public :
	DocumentXML();
	DocumentXML( const DocumentXML & a_documentXML );
	virtual ~DocumentXML( void );

	Element * racine( void ) const;

    friend ostream & operator << ( ostream &, const DocumentXML & );
};




/*
start-tags and end-tags, or, for empty elements, by an empty-element tag

[1]   	document	   ::=   	prolog element Misc*
[2]   	Char	   ::=   	#x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
[3]   	S	   ::=   	(#x20 | #x9 | #xD | #xA)+
[4]   	NameStartChar	   ::=   	":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] | [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D] | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] | [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
[4a]   	NameChar	   ::=   	NameStartChar | "-" | "." | [0-9] | #xB7 | [#x0300-#x036F] | [#x203F-#x2040]
[5]   	Name	   ::=   	NameStartChar (NameChar)*
[6]   	Names	   ::=   	Name (#x20 Name)*
[7]   	Nmtoken	   ::=   	(NameChar)+
[8]   	Nmtokens	   ::=   	Nmtoken (#x20 Nmtoken)*
[9]   	EntityValue	   ::=   	'"' ([^%&"] | PEReference | Reference)* '"'
									  |  "'" ([^%&'] | PEReference | Reference)* "'"
[10]   	AttValue	   ::=   	'"' ([^<&"] | Reference)* '"'
									  |  "'" ([^<&'] | Reference)* "'"
[11]   	SystemLiteral	   ::=   	('"' [^"]* '"') | ("'" [^']* "'")
[12]   	PubidLiteral	   ::=   	'"' PubidChar* '"' | "'" (PubidChar - "'")* "'"
[13]   	PubidChar	   ::=   	#x20 | #xD | #xA | [a-zA-Z0-9] | [-'()+,./:=?;!*#@$_%]
[14]   	CharData	   ::=   	[^<&]* - ([^<&]* ']]>' [^<&]*)
[15]   	Comment	   ::=   	'<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
[16]   	PI	   ::=   	'<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
[17]   	PITarget	   ::=   	Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
[18]   	CDSect	   ::=   	CDStart CData CDEnd
[19]   	CDStart	   ::=   	'<![CDATA['
[20]   	CData	   ::=   	(Char* - (Char* ']]>' Char*))
[21]   	CDEnd	   ::=   	']]>'
[28]   	doctypedecl	   ::=   	'<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)? '>'
[28a]   	DeclSep	   ::=   	PEReference | S
[28b]   	intSubset	   ::=   	(markupdecl | DeclSep)*
[29]   	markupdecl	   ::=   	elementdecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment
[30]   	extSubset	   ::=   	TextDecl? extSubsetDecl
[31]   	extSubsetDecl	   ::=   	( markupdecl | conditionalSect | DeclSep)*
[32]   	SDDecl	   ::=   	S 'standalone' Eq (("'" ('yes' | 'no') "'") | ('"' ('yes' | 'no') '"'))
[39]	element	   ::=   	EmptyElemTag
	| STag content ETag
[40 ]STag	   ::=   	'<' Name (S Attribute)* S? '>'
[41]   	Attribute	   ::=   	Name Eq AttValue
[42] ETag	   ::=   	'</' Name S? '>'
[43]   	content	   ::=   	CharData? ((element | Reference | CDSect | PI | Comment) CharData?)*
[44]	EmptyElemTag	   ::=   	'<' Name (S Attribute)* S? '/>'
[45]   	elementdecl	   ::=   	'<!ELEMENT' S Name S contentspec S? '>'
[46]   	contentspec	   ::=   	'EMPTY' | 'ANY' | Mixed | children
[47]   	children	   ::=   	(choice | seq) ('?' | '*' | '+')?
[48]   	cp	   ::=   	(Name | choice | seq) ('?' | '*' | '+')?
[49]   	choice	   ::=   	'(' S? cp ( S? '|' S? cp )+ S? ')'
[50]   	seq	   ::=   	'(' S? cp ( S? ',' S? cp )* S? ')'
[51]   	Mixed	   ::=   	'(' S? '#PCDATA' (S? '|' S? Name)* S? ')*'
		| '(' S? '#PCDATA' S? ')'
[52]   	AttlistDecl	   ::=   	'<!ATTLIST' S Name AttDef* S? '>'
[53]   	AttDef	   ::=   	S Name S AttType S DefaultDecl
[54]   	AttType	   ::=   	StringType | TokenizedType | EnumeratedType
[55]   	StringType	   ::=   	'CDATA'
[56]   	TokenizedType	   ::=   	'ID'
		| 'IDREF'
		| 'IDREFS'
		| 'ENTITY'
		| 'ENTITIES'
		| 'NMTOKEN'
		| 'NMTOKENS'
[57]   	EnumeratedType	   ::=   	NotationType | Enumeration
[58]   	NotationType	   ::=   	'NOTATION' S '(' S? Name (S? '|' S? Name)* S? ')'
[59]   	Enumeration	   ::=   	'(' S? Nmtoken (S? '|' S? Nmtoken)* S? ')'
[60]   	DefaultDecl	   ::=   	'#REQUIRED' | '#IMPLIED'
	| (('#FIXED' S)? AttValue)
[61]   	conditionalSect	   ::=   	includeSect | ignoreSect
[62]   	includeSect	   ::=   	'<![' S? 'INCLUDE' S? '[' extSubsetDecl ']]>'
[63]   	ignoreSect	   ::=   	'<![' S? 'IGNORE' S? '[' ignoreSectContents* ']]>'	[VC: Proper Conditional Section/PE Nesting]
[64]   	ignoreSectContents	   ::=   	Ignore ('<![' ignoreSectContents ']]>' Ignore)*
[65]   	Ignore	   ::=   	Char* - (Char* ('<![' | ']]>') Char*)


*/

DocumentXML * lireFichierXML( string a_nom );

#endif /* DOCUMENTXML_H_ */
