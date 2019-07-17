#ifndef MATLAB_S2M_IMU_H
#define MATLAB_S2M_IMU_H

#include <mex.h>
#include "s2mMusculoSkeletalModel.h"
#include "class_handle.h"
#include "processArguments.h"

void S2M_IMU( int, mxArray *plhs[],
                  int nrhs, const mxArray*prhs[] ){

    // Verifier les arguments d'entrée
    checkNombreInputParametres(nrhs, 3, 4, "3 arguments are required [+1 optional] where the 2nd is the handler on the model, 3rd is the Q and 4th is the wanted IMUType to be return (['all'], 'technical' or anatomical')");
    // Recevoir le model
    s2mMusculoSkeletalModel * model = convertMat2Ptr<s2mMusculoSkeletalModel>(prhs[1]);
    unsigned int nQ = model->nbQ(); /* Get the number of DoF */

    // Recevoir Q
    std::vector<s2mGenCoord> Q = getParameterQ(prhs, 2, nQ);

    // Récupérer les IMU selon que l'on veut tous ou seulement anatomiques ou techniques
    unsigned int nIMUs(0); // Nombre de IMU
    std::vector<std::vector<s2mIMU> > IMU_tp; // récupérer les IMU
    if (nrhs == 4){
        s2mString type(getString(prhs,3));
        if (!type.tolower().compare("all")){
            nIMUs = model->nIMUs();
            for (std::vector<s2mGenCoord>::iterator Q_it = Q.begin(); Q_it!=Q.end(); ++Q_it)
                 IMU_tp.push_back(model->IMU(*model, *Q_it));
        }
        else if (!type.tolower().compare("anatomical")){
            nIMUs = model->nAnatIMUs();
            for (std::vector<s2mGenCoord>::iterator Q_it = Q.begin(); Q_it!=Q.end(); ++Q_it)
                 IMU_tp.push_back(model->anatomicalIMU(*model, *Q_it));
        }
        else if (!type.tolower().compare("technical")){
            nIMUs = model->nTechIMUs();
            for (std::vector<s2mGenCoord>::iterator Q_it = Q.begin(); Q_it!=Q.end(); ++Q_it)
                 IMU_tp.push_back(model->technicalIMU(*model, *Q_it));
        }
        else {
            std::ostringstream msg;
            msg << "Wrong type for tags!";
            mexErrMsgTxt(msg.str().c_str());
        }

    }
    else {
        nIMUs = model->nIMUs();
        for (std::vector<s2mGenCoord>::iterator Q_it = Q.begin(); Q_it!=Q.end(); ++Q_it)
             IMU_tp.push_back(model->IMU(*model, *Q_it));
    }

    // Create a matrix for the return argument
    mwSize dims[4];
    dims[0] = 4;
    dims[1] = 4;
    dims[2] = nIMUs;
    dims[3] = IMU_tp.size();

    plhs[0] = mxCreateNumericArray(4, dims, mxDOUBLE_CLASS, mxREAL);
    double *imu = mxGetPr(plhs[0]);

    // Remplir l'output
    unsigned int cmpIMU = 0;
    for (std::vector<std::vector<s2mIMU> >::iterator AllIMU_it = IMU_tp.begin(); AllIMU_it != IMU_tp.end(); ++AllIMU_it)
        for (std::vector<s2mIMU>::iterator IMU_it=(*AllIMU_it).begin(); IMU_it!=(*AllIMU_it).end(); ++IMU_it)
            for (unsigned int i=0; i<4; ++i)
                for (unsigned int j=0; j<4; ++j){
                    imu[cmpIMU] = (*IMU_it)(j,i);
                    ++cmpIMU;
                }

    return;
}

#endif // MATLAB_S2M_IMU_H
