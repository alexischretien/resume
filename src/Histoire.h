/*
 * Histoire.h
 *
 *  Created on: Mar 18, 2017
 *      Author: Bruno Malenfant
 */

#ifndef HISTOIRE_H_
#define HISTOIRE_H_

#include "DocumentXML.h"
#include "Phrase.h"

#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

using namespace std;

const int TAILLE_LIGNE = 80;

class Histoire {
public :
    Histoire( void );
    Histoire( string a_titre );
    Histoire( const Histoire & a_histoire );
    virtual ~Histoire( void );

    string titre( void ) const;

    void ajouterPhrase( Phrase a_phrase );

    vector< Phrase >::const_iterator begin( void ) const;
    vector< Phrase >::const_iterator end( void ) const;

    friend ostream & operator << ( ostream &, const Histoire & );
private :
    string _titre;
    vector< Phrase > _phrases;
};

vector<Histoire *> * extraireHistoires(const DocumentXML & a_document);


#endif /* HISTOIRE_H_ */
