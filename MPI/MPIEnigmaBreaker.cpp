//
// Created by Sou on 01.11.2022.
//

#include "MPIEnigmaBreaker.h"
#include "mpi.h"
MPIEnigmaBreaker::MPIEnigmaBreaker( Enigma *enigma, MessageComparator *comparator ) : EnigmaBreaker(enigma, comparator ) {
}

MPIEnigmaBreaker::~MPIEnigmaBreaker() {
    delete[] rotorPositions;
    delete[] messageToDecode;
    delete[] expected;
}

void MPIEnigmaBreaker::setSampleToFind(uint *expected, uint expectedLength ) {
    comparator->setExpectedFragment(expected,expectedLength);
    this->expected = expected;
    this->expectedLength = expectedLength;
}


void MPIEnigmaBreaker::crackMessage() {
    uint rotorLargestSetting = enigma->getLargestRotorSetting();

    int myid;
    int size;
    uint* buf;
    uint buf2 ;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // rozeslanie message i messagelength
    if ( myid == MPI_ROOT_PROCESS_RANK ) {
        buf2 = messageLength;
        buf = messageToDecode;
    }

    MPI_Bcast(&buf2,1,MPI_UNSIGNED,0,MPI_COMM_WORLD);

    messageLength = buf2;
    comparator->setMessageLength(messageLength);
    if( myid != MPI_ROOT_PROCESS_RANK )
        buf = new uint[messageLength];

    MPI_Bcast(buf, buf2,MPI_UNSIGNED,0,MPI_COMM_WORLD);

    messageToDecode = buf;

    uint* buf1;
    //rozeslanie expected i expectedlength
    if ( myid == MPI_ROOT_PROCESS_RANK ) {
        buf2 = expectedLength;
        buf1 = expected;
    }

    MPI_Bcast(&buf2,1,MPI_UNSIGNED,0,MPI_COMM_WORLD);

    if( myid != MPI_ROOT_PROCESS_RANK )
        buf1 = new uint[buf2];

    MPI_Bcast(buf1, buf2,MPI_UNSIGNED,0,MPI_COMM_WORLD);

    if( myid != MPI_ROOT_PROCESS_RANK )
        setSampleToFind(buf1,buf2);

    uint *rMax = new uint[ MAX_ROTORS ];
    for ( uint rotor = 0; rotor < MAX_ROTORS; rotor++ ) {
        if ( rotor < rotors )
            rMax[ rotor ] = rotorLargestSetting;
        else
            rMax[ rotor ] = 0;
    }

    uint *r = new uint[ MAX_ROTORS ];
    uint *result = new uint[MAX_ROTORS];//dodatkowa tablica na wynik ktory przyjdzie z innego procesu

    //bedziemy odbierac wynik
    MPI_Request request;
    int flag = 0;
    MPI_Irecv(result,MAX_ROTORS,MPI_UNSIGNED,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&request);

    //if(size <= rotorLargestSetting + 1) { //procesow jest nie wiecej niz maksymalne ustawienie rotora
        for (r[0] = myid; r[0] <= rMax[0]; r[0] += size)
            for (r[1] = 0; r[1] <= rMax[1]; r[1]++)
                for (r[2] = 0; r[2] <= rMax[2]; r[2]++)
                    for (r[3] = 0; r[3] <= rMax[3]; r[3]++)
                        for (r[4] = 0; r[4] <= rMax[4]; r[4]++)
                            for (r[5] = 0; r[5] <= rMax[5]; r[5]++)
                                for (r[6] = 0; r[6] <= rMax[6]; r[6]++)
                                    for (r[7] = 0; r[7] <= rMax[7]; r[7]++)
                                        for (r[8] = 0; r[8] <= rMax[8]; r[8]++)
                                            for (r[9] = 0; r[9] <= rMax[9]; r[9]++) {
                                                if (solutionFound(r)) {
                                                    //wynik znaleziony wiec rozsylam wiadomosc do wszystkich innych procesow zeby je zakonczyc
                                                    for (int i = 0; i < size; ++i) {
                                                        if(i!=myid){
                                                            MPI_Ssend(r, MAX_ROTORS, MPI_UNSIGNED, i, 0, MPI_COMM_WORLD);
                                                        }
                                                    }
                                                    goto EXIT_ALL_LOOPS;
                                                }
                                                //sprawdzam czy przyszedl wynik
                                                MPI_Test(&request, &flag, MPI_STATUS_IGNORE);
                                                if (flag) {
                                                    //ustawiam wynik i wychodze
                                                    solutionFound(result);
                                                    goto EXIT_ALL_LOOPS;
                                                }
                                            }
        //aktualne rozwiazanie nierowno przydziela ilosc pracy
        // i nie przydzieli zadnej pracy procesom jesli ich numer jest wiekszy niz rotorlargestsetting
    //}
   // else{
        //pomysl: liczymy ile obrotow petli ma dany proces wykonac
        // i potem zamieniamy ta liczbe na system liczbowy o podstawie takiej jak rotor largest setting +1
        //przyklad: rotor przyjmuje wartosci od 0 do 199 czyli system 200 mamy 3 rotory kombinacji jest 200^3
        //liczymy 200^3/ilosc procesow i dostajemy liczbe w systemie dziesietnym po czym po zamianie na system 200
        // kazda pozycja tej liczby odpowiada jednej petli bedzie ona odpowiadala poczatkowemu ustawieniu r[], mamy wiec poczatkowy stan, mnozac ta liczbe przez
        // numer procesu+1 dostajemy ustawienie rotorow na jakim dany proces ma sie zatrzymac (odpowiednik rMax[])
        //pomysl 2: rekurencja gdzie dzielimy i schodzimy do petli nizej jesli podzial jest niemozliwy i wtedy dzielimy
        // przyklad: mamy 3 procesy a largest rotor setting to 1, jesli rotor jest 1 to sprawa jest trywialna jesli sa 2 to dodajemy dodajemy 2,
        //mamy wiec 4 ustawienia 0 i 1 na pierwszy i 0 i 1 na drugim, kazdy proces dostaje po jednym wiec zostaje nam 1 kombinacja
        // i tak schodzimy az do ostatniego rotora i na nim po prostu pozostala reszte z dzielenia mozemy przypisac do losowego procesu np 0 w tym przypadku po zejsciu do
        // ostatniego rotora albo 2 procesy beda mialy po 1 wiecej albo jeden bedzie mial o jedna wiecej

   // }

    EXIT_ALL_LOOPS:
    delete [] result;
    delete[] rMax;
    delete[] r;
}

bool MPIEnigmaBreaker::solutionFound( uint *rotorSettingsProposal ) {
    for ( uint rotor = 0; rotor < rotors; rotor++ )
        rotorPositions[ rotor ] = rotorSettingsProposal[ rotor ];

    enigma->setRotorPositions(rotorPositions);
    uint *decodedMessage = new uint[ messageLength ];


    for (uint messagePosition = 0; messagePosition < messageLength; messagePosition++ ) {
        decodedMessage[ messagePosition ] = enigma->code(messageToDecode[ messagePosition ] );
    }

    bool result = comparator->messageDecoded(decodedMessage);


    delete[] decodedMessage;

    return result;
}

void MPIEnigmaBreaker::getResult( uint *rotorPositions ) {
    for ( uint rotor = 0; rotor < rotors; rotor++ ) {
        rotorPositions[ rotor ] = this->rotorPositions[ rotor ];
    }
}