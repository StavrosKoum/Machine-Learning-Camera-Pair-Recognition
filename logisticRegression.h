#ifndef LOGISTICREGRESSION
#define LOGISTICREGRESSION


#include <stdio.h>
#include "metrics.h"


typedef struct logistic_reg
{
    double bias;

    double *weights;

    double *x;

    int y;

    double learning; 

    int lineSize;

    int error;

}logistic_reg;

double** shuffleArray(double** array, int *array2, char **array3, char **array4, int size);
logistic_reg * create_logistic_reg(int lineSize);
void fit(logistic_reg * cur,double *x,int y, int lineSize);
logistic_reg* logisticRegretionAlgorithm(logistic_reg *cls, int limit);
double cost_function(logistic_reg *cls);
// double logisticRegrationTest(logistic_reg *cls, double *data, char *left, char *right, int y, FILE *fp);
double logisticRegrationTest(logistic_reg *cls, double *data, char *left, char *right, int y, FILE *fp,positiveMetrics *P_metrics, negativeMetrics *N_metrics);    

void printClassifier(logistic_reg *cls);
void freeLogisticRegressor(logistic_reg *cls);

#endif
