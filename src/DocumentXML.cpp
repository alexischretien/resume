/*
 * LecteurXML.cpp
 *
 *  Created on: Mar 18, 2017
 *      Author: Bruno Malenfant
 */

#include "DocumentXML.h"

#include <fstream>
#include <sstream>

using namespace std;


#ifdef DEBUG_STATE
#define _DEBUG_STATE_( _STR_EXP_ ) { cout << _STR_EXP_ << endl; }
#else
#define _DEBUG_STATE_( _STR_EXP_ ) ;
#endif

#ifdef DEBUG_STRUCT
#define _DEBUG_STRUCT_( _STR_EXP_ ) { cout << _STR_EXP_ << endl; }
#else
#define _DEBUG_STRUCT_( _STR_EXP_ ) ;
#endif



Contenu::Contenu(void) {
}

Contenu::Contenu(const Contenu& a_contenu) {
}

Contenu::~Contenu(void) {
}

bool Contenu::estElement( void ) const {
    return false;
}

string Contenu::texte( void ) const {
    return "";
}

ostream & Contenu::afficher( ostream & a_out ) const {
    a_out << "allo" << endl;
    return a_out;
}

ostream & operator << ( ostream & a_out, const Contenu & a_contenu ) {
    return a_contenu.afficher( a_out );
}


Texte::Texte(void) :
	_texte( "" )
{
}

Texte::Texte(string a_texte) :
	_texte( a_texte )
{
}

Texte::Texte(const Texte& a_texte) :
	_texte( a_texte._texte )
{
}

Texte::~Texte(void) {
}

string Texte::texte( void ) const {
	return _texte;
}

ostream & Texte::afficher( ostream & a_out ) const {
    a_out << _texte;

    return a_out;
}


AttributNonDefinie::AttributNonDefinie( void ) :
        _nom( "" )
{}

AttributNonDefinie::AttributNonDefinie( string a_nom ) :
        _nom( a_nom )
{}

AttributNonDefinie::AttributNonDefinie( const AttributNonDefinie & a_e ) :
        _nom( a_e._nom )
{}

AttributNonDefinie::~AttributNonDefinie( void )
{}

const char* AttributNonDefinie::what() const throw()
{
    return ( string( "L'attribut " ) + _nom + string( " n'est pas defini." ) ).c_str();
}



Element::Element(void) :
    _parent( 0 ),
    _nom( "" )
{
}

Element::Element(Element* a_parent, string a_nom) :
	_parent( a_parent ),
	_nom( a_nom )
{
}

Element::Element(const Element& a_element) :
	_parent( a_element._parent ),
	_nom( a_element._nom ),
	_attributs( a_element._attributs ),
	_enfants( a_element._enfants )
{
}

Element::~Element(void) {
}

bool Element::estElement( void ) const {
    return true;
}

string Element::texte( void ) const {
    string resultat = "";

    for( Contenu * c : _enfants ) {
        resultat += c->texte();
    }

    return resultat;
}

ostream & Element::afficher( ostream & a_out) const {
    a_out << '<' << _nom;

    for( pair< string, string > att : _attributs ) {
        a_out << ' ' << att.first
              << '=';
        if( string::npos == att.second.find('"') ) {
            a_out << '"' << att.second << '"';
        } else {
            a_out << '\'' << att.second << '\'';
        }
    }

    a_out << '>' << endl;

    for( Contenu * contenu : _enfants ) {
        a_out << ( * contenu );
    }

    a_out << "</" << _nom << ">" << endl;

    return a_out;
}


Texte* Element::ajouterTexte(string a_texte) {
    Texte * texte = new Texte( a_texte );

    _enfants.push_back( texte );

    return texte;
}

Element* Element::ajouterElement(string a_nom) {
    Element * element = new Element( this, a_nom );

    _enfants.push_back( element );

    return element;
}

void Element::ajouterAttribut(string a_nom, string a_valeur) {
    _attributs[ a_nom ] = a_valeur;
}

Element* Element::parent(void) const {
    return _parent;
}

string Element::nom(void) const {
    return _nom;
}




string Element::attribut( string a_nom ) const throw( AttributNonDefinie ) {
    map< string, string >::const_iterator it = _attributs.find( a_nom );

    if( it == _attributs.end() ) {
        throw AttributNonDefinie( a_nom );
    }

    return ( * it ).second;
}


vector< Contenu * >::const_iterator Element::begin( void ) const {
    return _enfants.begin();
}

vector< Contenu * >::const_iterator Element::end( void ) const {
    return _enfants.end();
}



DocumentXML::DocumentXML() :
    _racine( new Element( 0, "__root__" ) )
{
}

DocumentXML::DocumentXML(const DocumentXML& a_documentXML) :
    _racine( new Element( * ( a_documentXML._racine ) ) )
{
}

DocumentXML::~DocumentXML(void) {
    delete _racine;
}

Element* DocumentXML::racine(void) const {
    return _racine;
}

ostream & operator << ( ostream & a_out, const DocumentXML & a_documentXML ) {
    a_out << * ( a_documentXML._racine );

    return a_out;
}



bool estLettrePourDebutId( char a_c ) {
    return ( ':' == a_c ) ||
           ( '_' == a_c ) ||
           ( 'A' <= a_c && a_c <= 'Z' ) ||
           ( 'a' <= a_c && a_c <= 'z' );
}

bool estLettrePourId( char a_c ) {
    return estLettrePourDebutId( a_c ) ||
           ( '-' == a_c ) ||
           ( '.' == a_c ) ||
           ( '0' <= a_c && a_c <= '9' );
}

bool estEspace( char a_c ) {
    return ( ' ' == a_c ) ||
           ( '\t' == a_c ) ||
           ( '\n' == a_c ) ||
           ( '\r' == a_c );
}



enum Etat {
    TEXTE, OUVERTURE_TAG, NOM_DEBUT_ELEMENT,
    NOM_FIN_PREMIER, LISTE_ATTRIBUTS, ATTRIBUT_NOM,
    FIN_TAG_COMPLET, ATT_EGAL, ATT_APOSTROPHE,
    ATT_GUILLEMET, PI_DEBUT, PI_FIN_POSSIBLE
} ;



DocumentXML * lireFichierXML( string a_nom ) {


    Etat etat = TEXTE;
    ifstream fichier( a_nom );
    char c;
    DocumentXML * document = new DocumentXML();
    Element * courrant = document->racine();
    stringstream accumulateur;
    string nomTemp = "";

    fichier.get( c );
    _DEBUG_STATE_( c << " : " )

    while( ! fichier.eof() ) {
        switch( etat ){
        case TEXTE :
            _DEBUG_STATE_(  "TEXTE" )
            if( '<' == c ) {
                etat = OUVERTURE_TAG;
                if( ! accumulateur.eof() ){
                    _DEBUG_STRUCT_( accumulateur.str().substr( 0, 10 ) << "..." )
                    courrant->ajouterTexte( accumulateur.str() );
                    accumulateur.str( "" );
                }
            } else {
                accumulateur << c;
            }
            break;
        case OUVERTURE_TAG :
            _DEBUG_STATE_( "OUVERTURE_TAG" )
            if( estLettrePourDebutId( c ) ) {
                accumulateur << c;
                etat = NOM_DEBUT_ELEMENT;
            } else if( '/' == c ) {
                etat = NOM_FIN_PREMIER;
            } else if( '?' == c ) {
                etat = PI_DEBUT;
            }
            break;
        case NOM_DEBUT_ELEMENT :
            _DEBUG_STATE_( "NOM_DEBUT_ELEMENT" )
            if( estLettrePourId( c ) ) {
                accumulateur << c;
            } else if( '>' == c ) {
                courrant = courrant->ajouterElement( accumulateur.str() );
                _DEBUG_STRUCT_( "<" << courrant->nom() << ">" )
                accumulateur.str( "" );
                etat = TEXTE;
            } else if( estEspace( c ) ) {
                courrant = courrant->ajouterElement( accumulateur.str() );
                _DEBUG_STRUCT_( "<" << courrant->nom() << ">" )
                accumulateur.str( "" );
                etat = LISTE_ATTRIBUTS;
            } else if( '/' == c ) {
                courrant = courrant->ajouterElement( accumulateur.str() );
                _DEBUG_STRUCT_( "<" << courrant->nom() << ">" )
                accumulateur.str( "" );
                etat = FIN_TAG_COMPLET;
            }
            break;
        case NOM_FIN_PREMIER :
            _DEBUG_STATE_( "NOM_FIN_PREMIER" )
            if( estLettrePourId( c ) ) {
                accumulateur << c;
            } else if( '>' == c ) {
                _DEBUG_STRUCT_( "</" << courrant->nom() << ">" )
                courrant = courrant->parent();
                _DEBUG_STRUCT_( "--> <" << courrant->nom() << ">" )
                accumulateur.str( "" );
                etat = TEXTE;
            }
            break;
        case LISTE_ATTRIBUTS :
            _DEBUG_STATE_( "LISTE_ATTRIBUTS" )
            if( estLettrePourDebutId( c ) ) {
                accumulateur << c;
                etat = ATTRIBUT_NOM;
            } else if( '>' == c ) {
                etat = TEXTE;
            } else if( '/' == c ) {
                etat = FIN_TAG_COMPLET;
            }
            break;
        case ATTRIBUT_NOM :
            _DEBUG_STATE_( "ATTRIBUT_NOM" )
            if( estLettrePourId( c ) ) {
                accumulateur << c;
            } else if( '=' == c ) {
                nomTemp = accumulateur.str();
                accumulateur.str( "" );
                etat = ATT_EGAL;
            }
            break;
        case FIN_TAG_COMPLET :
            _DEBUG_STATE_( "FIN_TAG_COMPLET" )
            if( '>' == c ) {
                _DEBUG_STRUCT_( "</" << courrant->nom() << ">" )
                courrant = courrant->parent();
                _DEBUG_STRUCT_( "--> <" << courrant->nom() << ">" )
                accumulateur.str( "" );
                etat = TEXTE;
            }
            break;
        case ATT_EGAL :
            _DEBUG_STATE_( "ATT_EGAL" )
            if( '\'' == c ) {
                etat = ATT_APOSTROPHE;
            } else if( '"' == c ) {
                etat = ATT_GUILLEMET;
            }
            break;
        case ATT_APOSTROPHE :
            _DEBUG_STATE_( "ATT_APOSTROPHE" )
            if( '\'' == c ) {
                _DEBUG_STRUCT_( nomTemp << "='" << accumulateur.str() << "'" )
                courrant->ajouterAttribut( nomTemp, accumulateur.str() );
                accumulateur.str( "" );
                etat = LISTE_ATTRIBUTS;
            } else {
                accumulateur << c;
            }
            break;
        case ATT_GUILLEMET :
            _DEBUG_STATE_( "ATT_GUILLEMET" )
            if( '"' == c ) {
                _DEBUG_STRUCT_( nomTemp << "=\"" << accumulateur.str() << "\"" )
                courrant->ajouterAttribut( nomTemp, accumulateur.str() );
                accumulateur.str( "" );
                etat = LISTE_ATTRIBUTS;
            } else {
                accumulateur << c;
            }
            break;
        case PI_DEBUT :
            _DEBUG_STATE_( "PI_DEBUT" )
            if( '?' == c ) {
                etat = PI_FIN_POSSIBLE;
            }
            break;
        case PI_FIN_POSSIBLE :
            _DEBUG_STATE_( "PI_FIN_POSSIBLE" )
            if( '>' == c ) {
                etat = TEXTE;
            }
            break;
        }

        fichier.get( c );
        _DEBUG_STATE_( c << " : " )
    }

    return document;
}
