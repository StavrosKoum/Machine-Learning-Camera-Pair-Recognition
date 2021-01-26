#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "metrics.h"

positiveMetrics *initPositiveMetrics(char *name){
    positiveMetrics *metrics = malloc(sizeof(positiveMetrics));
    metrics->name = strdup(name);
    metrics->precision = 0.0;
    metrics->recall = 0.0;
    metrics->f1 = 0.0;
    metrics->truePositive = 0;
    metrics->falsePositive = 0;
    metrics->flaseNegative = 0;

    return metrics;
}

negativeMetrics *initNegativeMetrics(char *name){
    negativeMetrics *metrics = malloc(sizeof(negativeMetrics));
    metrics->name = strdup(name);
    metrics->precision = 0.0;
    metrics->recall = 0.0;
    metrics->f1 = 0.0;
    metrics->truePositive = 0;
    metrics->falsePositive = 0;
    metrics->flaseNegative = 0;

    return metrics;
}


positiveMetrics *updatePositiveMetrics(positiveMetrics *metrics, int x, int y){
    if(y==1 && x==1){
        metrics->truePositive++;
    }
    if(y==0 && x==1){
        metrics->falsePositive++;
    }
    if(y==1 && x==0){
        metrics->flaseNegative++;
    }

    return metrics;
}


negativeMetrics *updateNegativeMetrics(negativeMetrics *metrics, int x, int y){
    if(y==0 && x==0){
        metrics->truePositive++;
    }
    if(y==1 && x==0){
        metrics->falsePositive++;
    }
    if(y==0 && x==1){
        metrics->flaseNegative++;
    }
    
    return metrics;
}


positiveMetrics *evaluatePositiveMetrics(positiveMetrics *metrics){
    //printf("TRUE-POSITIVE: %d\n FALSE-POSITIVE: %d\n FALSE-NEGATIVE: %d\n", metrics->truePositive, metrics->falsePositive, metrics->flaseNegative);
    metrics->precision = (double)metrics->truePositive/((double)metrics->truePositive + (double)metrics->falsePositive);
    metrics->recall = (double)metrics->truePositive/((double)metrics->truePositive + (double)metrics->flaseNegative);
    metrics->f1 = 2*(metrics->precision*metrics->recall /(metrics->precision+metrics->recall ));

    return metrics;
}


negativeMetrics *evaluateNegativeMetrics(negativeMetrics *metrics){
    metrics->precision = (double)metrics->truePositive/((double)metrics->truePositive + (double)metrics->falsePositive);
    metrics->recall = (double)metrics->truePositive/((double)metrics->truePositive + (double)metrics->flaseNegative);
    metrics->f1 = 2*(metrics->precision*metrics->recall /(metrics->precision+metrics->recall ));

    return metrics;
}


void printPositiveMetrics(positiveMetrics *metrics){
    //printf("%d, %d, %d\n", metrics->truePositive, metrics->falsePositive, metrics->flaseNegative);
    printf("%s clique:\nPrecision: %f\nRecall: %f\nF1 score: %f\n",metrics->name,metrics->precision,metrics->recall,metrics->f1);
}


void printNegativeMetrics(negativeMetrics *metrics){
    printf("%s clique:\nPrecision: %f\nRecall: %f\nF1 score: %f\n",metrics->name,metrics->precision,metrics->recall,metrics->f1);
}


void freePositiveMetrics(positiveMetrics **metrics){
    free((*metrics)->name);
    free(*metrics);
}


void freeNegativeMetrics(negativeMetrics **metrics){
    free((*metrics)->name);
    free(*metrics);
}