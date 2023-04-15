//
// Created by Sou on 01.11.2022.
//

#ifndef ZADANIE_MPI_MPIENIGMABREAKER_H
#define ZADANIE_MPI_MPIENIGMABREAKER_H

#include"EnigmaBreaker.h"

class MPIEnigmaBreaker : public EnigmaBreaker {
protected:
    uint *expected;
    uint expectedLength;
private:
    bool solutionFound( uint *rotorSettingsProposal );
public:
    MPIEnigmaBreaker( Enigma *enigma, MessageComparator *comparator );

    void crackMessage();
    void getResult( uint *rotorPositions );
    virtual void setSampleToFind( uint *expected, uint expectedLength );

    virtual ~MPIEnigmaBreaker();
};


#endif //ZADANIE_MPI_MPIENIGMABREAKER_H
