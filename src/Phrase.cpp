/*
 * Phrase.cpp
 *
 *  Created on: 2017-04-11
 *      Author: boust_000
 */

#include "Phrase.h"


#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

bool estCaractereMot( char c ) {
    return ( 'A' <= c && c <= 'Z' ) ||
           ( 'a' <= c && c <= 'z' ) ||
           ( '-' == c );
}



Phrase::Phrase(void) :
    _original( string( "" ) ),
    _ordonnee( string( "" ) ),
    _nMot( 0 )
{}


Phrase::Phrase(string a_original) :
    _original( a_original )
{
    vector< string > mots;

    stringstream texte( a_original );
    stringstream accumulateur;
    char c;

    while( ! texte.eof() ) {
        do {
            texte.get( c );
        } while( ! estCaractereMot( c ) && ! texte.eof() );

        while( estCaractereMot( c ) && ! texte.eof() ) {
            accumulateur << c;
            texte.get( c );
        }

        string mot = accumulateur.str();
        accumulateur.str( "" );

        if( mot.length() != 0 ) {
            transform( mot.begin(), mot.end(), mot.begin(), ::tolower );
            mots.push_back( mot );
        }
    }

    sort( mots.begin(), mots.end() );

    _ordonnee = "";

    for( string mot : mots ) {
        _ordonnee += mot;
    }

    _nMot = mots.size();
}

Phrase::Phrase(const Phrase& a_phrase) :
    _original( a_phrase._original ),
    _ordonnee( a_phrase._ordonnee ),
    _nMot( a_phrase._nMot )
{
}

Phrase::~Phrase(void) {
}

int Phrase::nMot(void) const {
    return _nMot;
}

int Phrase::nLettre(void) const {
    return _original.size();
}



double Phrase::rougeW_ord(const Phrase& a_phrase, function<double(double)> a_f ) const {
    int i = 0;
    int j = 0;

    double k = 0.0;

    int m = _ordonnee.size();
    int n = a_phrase._ordonnee.size();

    double c [ m + 1 ][ n + 1 ];
    double w [ m + 1 ][ n + 1 ];

    for( i = 0; i <= m; ++ i ) {
        for( j = 0; j <= n; ++ j ) {
            c[ i ][ j ] = 0;
            w[ i ][ j ] = 0;
        }
    }

    for( i = 1; i <= m; ++ i ) {
        for( j = 1; j <= n; ++ j ) {
            if( _ordonnee[ i - 1 ] == a_phrase._ordonnee[ j - 1 ] ) {
                k = w[ i - 1 ][ j - 1 ];
                c[ i ][ j ] = c[ i - 1 ][ j - 1 ] + a_f( k + 1 ) - a_f( k );
                w[ i ][ j ] = k + 1.0;
            } else {
                if( c[ i - 1 ][ j ] > c[ i ][ j - 1 ] ) {
                    c[ i ][ j ] = c[ i - 1 ][ j ];
                } else {
                    c[ i ][ j ] = c[ i ][ j - 1 ];
                }
                w[ i ][ j ] = 0.0;
            }
        }
    }

    return c[ m ][ n ] / a_f( max( _ordonnee.length(), a_phrase._ordonnee.length() ) );
}

double Phrase::distance( const Phrase & a_phrase ) const {
    return 1.0 - rougeW_ord( a_phrase, []( double x ){ return x * x; } );
}

ostream & operator << ( ostream & a_out, const Phrase & a_phrase ) {
    a_out << a_phrase._original << '.';
    return a_out;
}
