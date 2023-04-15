#include "SimpleMinimization.h"

#include<stdlib.h>
#include<math.h>
#include<iostream>
#include<sys/time.h>
#include<omp.h>

const double DR_SHRINK = 0.8;

using namespace std;

SimpleMinimization::SimpleMinimization(Function *f, double timeLimit) :
        Minimization(f, timeLimit) {

    generateRandomPosition();
    bestX = x;
    bestY = y;
    bestZ = z;

    bestV = function->value(bestX, bestY, bestZ);

    unsigned long seed = (unsigned long) time(NULL);
    srand48( seed );
}

SimpleMinimization::~SimpleMinimization() {
}


void SimpleMinimization::find(double dr_ini, double dr_fin,
		int idleStepsLimit ) {
	double xnew, ynew, znew, vnew, dr;
    double xlocal,ylocal,zlocal,vlocal;
	int idleSteps = 0;  // liczba krokow, ktore nie poprawily lokalizacji
    struct drand48_data buff;
    struct timeval tf;
    gettimeofday(&tf, NULL);
	std::cout << "Start " << std::endl;

    #pragma omp parallel private (buff,dr,idleSteps, xnew,ynew,znew,vnew,xlocal,ylocal,zlocal,vlocal)
    {
        int a = omp_get_thread_num();

        srand48_r(tf.tv_sec *1000*a +tf.tv_usec*0.001,&buff);

        while (hasTimeToContinue()) {

            // inicjujemy losowo polozenie startowe w obrebie kwadratu o bokach od min do max

//        #pragma omp critical
//            {
//                generateRandomPosition2(&buff);
//                xnew = x;
//                ynew = y;
//                znew = z;
//                v = function->value(x,y,z);
                //std::cout << "Starting point : " << x << ", " << y << ", " << z
                //          << " value = " << v  << " Thread: "<<a<<" VFunc: "<<function->value(x, y, z) << std::endl;
//            }
            double x1 ,y1,z1;

            drand48_r(&buff,&x1);
            drand48_r(&buff,&y1);
            drand48_r(&buff,&z1);
            xlocal =  x1 * (maxX - minX) + minX;
            ylocal =  y1 * (maxY - minY) + minY;
            zlocal =  z1 * (maxZ - minZ) + minZ;
            vlocal = function->value(xlocal,ylocal,zlocal);

//            std::cout << "Starting point : " << xlocal << ", " << ylocal << ", " << zlocal
//                      << " value = " << vlocal  << " Thread: "<<a<<" VFunc: "<<function->value(xlocal, ylocal, zlocal) << std::endl;
            // wartosc funkcji w punkcie startowym

            idleSteps = 0;
            dr = dr_ini;

            while ( ( dr > dr_fin ) && ( idleSteps < idleStepsLimit ) ) {

                drand48_r(&buff,&x1);
                drand48_r(&buff,&y1);
                drand48_r(&buff,&z1);
                xnew = xlocal + (x1 - 0.5) * dr;
                ynew = ylocal + (y1 - 0.5) * dr;
                znew = zlocal + (z1 - 0.5) * dr;

                // upewniamy sie, ze nie opuscilismy przestrzeni poszukiwania rozwiazania
                xnew = limitX(xnew);
                ynew = limitY(ynew);
                znew = limitZ(znew);

                // wartosc funkcji w nowym polozeniu
                vnew = function->value(xnew, ynew, znew);

                if (vnew < vlocal) {

                    xlocal = xnew;  // przenosimy sie do nowej, lepszej lokalizacji
                    ylocal = ynew;
                    zlocal = znew;
                    vlocal = vnew;
                    idleSteps = 0; // resetujemy licznik krokow, bez poprawy polozenia

//                    std::cout << "Changing point : " << xlocal << ", " << ylocal << ", " << zlocal
//                              << " value = " << vlocal << " Thread: " << a << " VFunc: " << function->value(x, y, z)
//                              << std::endl;

                }
                else {
                    idleSteps++; // nic sie nie stalo
                    if ( idleSteps == idleStepsLimit ) {
                        dr *= DR_SHRINK; // zmniejszamy dr
                        idleSteps = 0;
                    }
                }
            }// dr wciaz za duze

                #pragma omp critical
            {

                x = xlocal;
                y = ylocal;
                z = zlocal;
//                    std::cout << "Adding new point to history: " << x << ", " << y << ", " << z
//                              << " value = " << vlocal  << " Thread: "<<a<<" VFunc: "<<function->value(x, y, z) << std::endl;
                addToHistory();

                    if (vlocal < bestV) {// znalezlismy najlepsze polozenie globalnie
//                        #pragma omp critical
//                        {
//                            if (vlocal < bestV) {
                                bestV = vlocal;
                                bestX = xlocal;
                                bestY = ylocal;
                                bestZ = zlocal;

                                std::cout << "New better position: " << x << ", " << y << ", " << z
                                          << " value = " << vlocal << std::endl;
                       //     }
                        }
                    }


        } // mamy czas na obliczenia
    }



}


void SimpleMinimization::generateRandomPosition() {
    x = drand48() * (maxX - minX) + minX;
    y = drand48() * (maxY - minY) + minY;
    z = drand48() * (maxZ - minZ) + minZ;
}

//void SimpleMinimization::generateRandomPosition2(drand48_data* temp) {
//
//    double x1 ,y1,z1;
//    drand48_r(temp,&x1);
//    drand48_r(temp,&y1);
//    drand48_r(temp,&z1);
//	x =  x1 * (maxX - minX) + minX;
//	y =  y1 * (maxY - minY) + minY;
//	z =  z1 * (maxZ - minZ) + minZ;
//}

