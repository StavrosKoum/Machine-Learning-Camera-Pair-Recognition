#ifndef METRICS
#define METRICS

typedef struct positiveMetrics{

    //positive clique
    char *name;
    double precision;
    double recall;
    double f1;
    int truePositive;
    int falsePositive;
    int flaseNegative;

}positiveMetrics;

typedef struct negativeMetrics{

    //negative clique
    char *name;
    double precision;
    double recall;
    double f1;
    int truePositive;
    int falsePositive;
    int flaseNegative;

}negativeMetrics;


//positive clique functions
positiveMetrics *initPositiveMetrics(char *name);
positiveMetrics *updatePositiveMetrics(positiveMetrics *metrics, int x, int y);
positiveMetrics *evaluatePositiveMetrics(positiveMetrics *metrics);
void printPositiveMetrics(positiveMetrics *metrics);
void freePositiveMetrics(positiveMetrics **metrics);

//negative clique functions
negativeMetrics *initNegativeMetrics(char *name);
negativeMetrics *updateNegativeMetrics(negativeMetrics *metrics, int x, int y);
negativeMetrics *evaluateNegativeMetrics(negativeMetrics *metrics);
void printNegativeMetrics(negativeMetrics *metrics);
void freeNegativeMetrics(negativeMetrics **metrics);

#endif