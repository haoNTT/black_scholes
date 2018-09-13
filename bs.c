//Senior Project 
//Author: Haonan Tian 
//Date: 02/20/2018

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#define TYPE1 "call"
#define TYPE2 "put"
#define SIZE sizeof(transaction)

struct transaction {
	char type[30];
	char companyName[30];
	char startDate[30];
	char endDate[30];
	double strikingPrice;
	double underlyingPrice;
	double interestRate;
	double volatility;
	double actualPrice;
	double result;
	double binomial;
};

struct benchMark {
	int total;
	char** date;
	double* rate;
};

struct binomialResult {
	double K; // Hold the strinking price
	int total;
	double *values;
};

struct passInNormal {
	int total;
	int start;
	struct transaction *storage;
	struct benchMark bench;
	int size;
};

int countTransaction(char *fin) {
	FILE *localFile = fopen(fin, "r");
	char line[1024];
    char *temp;
    char *check = "0";
    int totalValidTransactions;
	fgets(line, 1024, localFile); //Read first line which is a list of format attributes
	while (fgets(line,1024,localFile) != NULL) {
		temp = strtok(line,",");
		int counter = 0;
		while (counter != 6) {
			temp = strtok(NULL, ",");
			counter ++;
		}
		if (strcmp(temp, check) != 0){
			totalValidTransactions ++;
		}
	}
	fclose(localFile);
	return totalValidTransactions;
}

void readIn(struct transaction *transactions, char *fin){
	FILE *localFile = fopen(fin, "r");
	char *line = NULL;
	char *temp;
	char *temp2;
	char *check = "0";
	int trans_no = 0;
	//fgets(line, 10000, localFile); //Read first line which is a list of format attributes
	ssize_t read;
	size_t len = 0;
	getline(&line, &len, localFile);
	//printf("read1\n");
	while ((read = getline(&line, &len, localFile)) != -1) {
		//printf("line =    %s\n", line);
		char holdLine[1024];
		strcpy(holdLine, line);
		temp = strtok(line,",");
		int counter = 0;
		while (counter != 6) {
			temp = strtok(NULL, ",");
			counter ++;
		}
		//printf("read2\n");
		//printf("line =    %s\n", holdLine);
		if (strcmp(temp, check) != 0){
			temp2 = strtok(holdLine, ",");
			//printf("%s\n", temp2);
			int innerCounter = 0;
			while (temp2 != NULL){
				//printf("read3\n");
				if (innerCounter == 0) {
					//printf("innerCounter == 0\n");
					strcpy(transactions[trans_no].companyName,temp2);
					//printf("value??\n");
					//temp2 = strtok(NULL, ",");
					innerCounter ++;
				} else if (innerCounter == 1) {
					//printf("innerCounter == 1\n");
					strcpy(transactions[trans_no].endDate,temp2);
					//temp2 = strtok(NULL,",");
					innerCounter ++;
				} else if (innerCounter == 4) {
					//char *hold;
					//strcpy(hold, temp2);
					//sscanf(hold, "%lf", transactions[trans_no].actualPrice);
					//printf("innerCounter == 6\n");
					transactions[trans_no].actualPrice = atof(temp2);
					//temp2 = strtok(NULL, ",");
					innerCounter ++;
				} else if (innerCounter == 5) {
					//printf("innerCounter == 7\n");
					strcpy(transactions[trans_no].type,temp2);
					//temp2 = strtok(NULL, ",");
					innerCounter ++;
				} else if (innerCounter == 6) {
					//char *hold;
					//strcpy(hold, temp2);
					//sscanf(hold, "%lf", transactions[trans_no].strikingPrice);
					//printf("innerCounter == 8\n");
					transactions[trans_no].strikingPrice = atof(temp2);
					//temp2 = strtok(NULL, ",");
					innerCounter ++;
				} else if (innerCounter == 8) {
					//char *hold;
					//strcpy(hold, temp2);
					//sscanf(hold, "%lf", transactions[trans_no].volatility);
					//printf("innerCounter == 10\n");
					transactions[trans_no].volatility = atof(temp2);
					//temp2 = strtok(NULL, ",");
					innerCounter ++;
				} else if (innerCounter == 13) {
					//char *hold;
					//strcpy(hold, temp2);
					//sscanf(hold, "%lf", transactions[trans_no].underlyingPrice);
					//printf("innerCounter == 16\n");
					transactions[trans_no].underlyingPrice = atof(temp2);
					//temp2 = strtok(NULL, ",");
					innerCounter ++;
				} else if (innerCounter == 14) {
					//printf("innerCounter == 17\n");
					strcpy(transactions[trans_no].startDate,temp2);
					//temp2 = strtok(NULL, ",");
					innerCounter ++;
				} else {
					//temp2 = strtok(NULL, ",");
					innerCounter ++;
				}
				temp2 = strtok(NULL, ",");
				//printf("%s\n", temp2);
			}
			trans_no ++;
		}
	}
	fclose(localFile);
}

struct benchMark constructBenchMark(char *inputFile) {
	FILE *localFile = fopen(inputFile, "r");
	struct benchMark bench1;
	char line[1024];
	char *temp;
	fgets(line, 1024, localFile); // Read first line of the file which is some symbols 
	int lineCounter = 0;
	while (fgets(line, 1024, localFile) != NULL) {
		lineCounter ++;
	}
	fclose(localFile);
	FILE *localFile2 = fopen(inputFile, "r");
	//printf("bent1\n");
	bench1.total = lineCounter;
	*bench1.date = malloc(sizeof("2016-02-24") * bench1.total);
	bench1.rate = malloc(sizeof(double) * bench1.total);
	//printf("bent2\n");
	char *storedLine = NULL;
	size_t len = 0;
	ssize_t read;
	getline(&storedLine, &len, localFile2);
	int counter = 0;
	//printf("bent2\n");
	while ((read = getline(&storedLine, &len, localFile2)) != -1) {
		//printf("bench3\n");
		temp = strtok(storedLine, ",");
		strcpy(*bench1.date+sizeof("2016-02-24") * counter, temp);
		//printf("bench4\n");
		temp = strtok(NULL, ",");
		//sscanf(temp, "%lf", bench1.rate[counter]);
		bench1.rate[counter] = atof(temp);
		//printf("bench5\n");
		//printf("%s\n", bench1.date[counter]);
		//printf("%lf\n", bench1.rate[counter]);
		counter ++;
	}
	//printf("bent3\n");
	fclose(localFile2);
	return bench1;
}

/***************************This section specifize the use of Black-Schole Model*************************/
int compareDate(char *date1, char *date2) {  // return 1 when strings are not the same else return 0
	char *temp1;
	char *temp2;
	temp1 = strtok(date1, "-");
	temp2 = strtok(date2, "-");
	printf("comp1\n");
	if (strcmp(temp1, temp2) != 0) {
		return 1;
	} else {
		temp1 = strtok(NULL, "-");
		temp2 = strtok(NULL, "-");
		if (strcmp(temp1, temp2) != 0) {
			return 1;
		} else {
			return 0;
		}
	}
	printf("comp1\n");
}

double findRate(struct benchMark *inputBench, char *inputDate) { // return the 3M standard rate for a certain date
	for (int i = 0; i < inputBench->total; i++) {
		//printf("%d\n", i);
		//printf("%s\n", *inputBench->date + i * sizeof("2016-02-24"));
		//printf("%s\n", inputDate);
		//char *temp1; 
		//printf("here\n");
		char *date1 = malloc(sizeof("2016-02-24")); 
		strcpy(date1, inputDate);
		char *temp1 = strtok(date1, "-");
		//printf("here\n");
		char *date2 = malloc(sizeof("2016-02-24"));
		strcpy(date2, *inputBench->date + i * sizeof("2016-02-24"));
		//printf("here\n");
		char *temp2 = strtok(date2, "-");
		//printf("here\n");
		if (strcmp(temp1, temp2) == 0) {
			temp1 = strtok(NULL, "-");
			temp2 = strtok(NULL, "-");
			if (strcmp(temp1, temp2) == 0) {
				return inputBench->rate[i+1];
			}
		}
		free(date1);
	}
	return -1;
}

double calculateMaturity(char *startDate, char *endDate) { // Return the maturity counted in years
	double result = 0;
	char *tempdate1 = malloc(sizeof("2016-02-24"));
	char *tempdate2 = malloc(sizeof("2016-02-24"));
	strcpy(tempdate1, startDate);
	strcpy(tempdate2, endDate);
	
	//printf("date1 = %s\n", tempdate1);
	//printf("date2 = %s\n", tempdate2);

	double year1 = atof(strtok(tempdate1, "-"));
	double month1 = atof(strtok(NULL, "-"));
	double day1 = atof(strtok(NULL, "-"));

	double year2 = atof(strtok(tempdate2, "-"));
	double month2 = atof(strtok(NULL, "-"));
	double day2 = atof(strtok(NULL, "-"));


	/*printf("year1 = %lf\n", year1);
	printf("month1 = %lf\n", month1);
	printf("day1 = %lf\n", day1);

	printf("year2 = %lf\n", year2);
	printf("month2 = %lf\n", month2);
	printf("day2 = %lf\n", day2);*/

	result += (year2 - year1) * 365;
	if (month2 < month1) {
		result -= (month1 - month2) * 30;
	} else {
		result += (month2 - month1) * 30;
	}

	if (day2 < day1) {
		result -= day1 - day2;
	} else {
		result += day2 - day1;
	}
	free(tempdate1);
	free(tempdate2);
	return result / 365;
}

double normal_01_cdf(double x) { // This function is cited from the truncated_normal.h library 
	double a1 = 0.398942280444;
    double a2 = 0.399903438504;
    double a3 = 5.75885480458;
    double a4 = 29.8213557808;
    double a5 = 2.62433121679;
    double a6 = 48.6959930692;
    double a7 = 5.92885724438;
    double b0 = 0.398942280385;
    double b1 = 3.8052E-08;
    double b2 = 1.00000615302;
    double b3 = 3.98064794E-04;
    double b4 = 1.98615381364;
    double b5 = 0.151679116635;
    double b6 = 5.29330324926;
    double b7 = 4.8385912808;
    double b8 = 15.1508972451;
    double b9 = 0.742380924027;
    double b10 = 30.789933034;
    double b11 = 3.99019417011;
    double cdf;
    double q;
    double y;
    if ( fabs ( x ) <= 1.28 ){
    	y = 0.5 * x * x;

        q = 0.5 - fabs ( x ) * ( a1 - a2 * y / ( y + a3 - a4 / ( y + a5
        + a6 / ( y + a7 ) ) ) );
    }
    else if ( fabs ( x ) <= 12.7 ) {
    	y = 0.5 * x * x;
    	q = exp ( - y ) * b0 / ( fabs ( x ) - b1 
    		+ b2  / ( fabs ( x ) + b3
    		+ b4  / ( fabs ( x ) - b5
            + b6  / ( fabs ( x ) + b7
            - b8  / ( fabs ( x ) + b9
            + b10 / ( fabs ( x ) + b11 ) ) ) ) ) );  
    } else {
    	q = 0.0;
    }
    if ( x < 0.0 ) {
    	cdf = q;
    } else {
    	cdf = 1.0 - q;
    }
    return cdf;
}

double computeD1(struct transaction *inputTrans, struct benchMark *inputBench) { // Calculation for d1 
	double S = inputTrans->underlyingPrice;
	double K = inputTrans->strikingPrice;
	double r = findRate(inputBench, inputTrans->startDate);
	double T = calculateMaturity(inputTrans->startDate, inputTrans->endDate);
	double sigma = inputTrans->volatility;
	return (log(S / K) + (r + pow(sigma, 2) / 2) * T) / sigma * pow(T, 0.5);
}

double computeD2(struct transaction *inputTrans, double d1) { // Calculation for d2
	double sigma = inputTrans->volatility;
	double T = calculateMaturity(inputTrans->startDate, inputTrans->endDate);
	return d1 - sigma * pow(T, 0.5);
}

double calculateCall(struct transaction *inputTrans, double d1, double d2, struct benchMark *inputBench) {
	double S = inputTrans->underlyingPrice;
	double K = inputTrans->strikingPrice;
	double r = findRate(inputBench, inputTrans->startDate);
	double T = calculateMaturity(inputTrans->startDate, inputTrans->endDate);
	double part1 = S * normal_01_cdf(d1);
	double part2 = K * normal_01_cdf(d2) / pow(M_E, (r * T));
	return part1 - part2;
}

double calculatePut(struct transaction *inputTrans, double d1, double d2, struct benchMark *inputBench) {
	double S = inputTrans->underlyingPrice;
	double K = inputTrans->strikingPrice;
	double r = findRate(inputBench, inputTrans->startDate);
	double T = calculateMaturity(inputTrans->startDate, inputTrans->endDate);
	double part1 = K * normal_01_cdf(-d2) / pow(M_E, (r * T));
	double part2 = S * normal_01_cdf(-d1);
	return part1 - part2;
}

/*void calculateBSResult(struct transaction *storage, struct benchMark *bench, int total) {
	for (int i = 0; i < total; i++) {
		//double interestRate = findRate(bench, storage[i]->startDate);
		//double maturity = calculateMaturity(storage[i]->startDate, storage[i]->endDate);
		double D1 = computeD1(&storage[i], bench);
		//printf("D1\n");
		double D2 = computeD2(&storage[i], D1);
		//printf("D2\n");
		//rintf("%d\n", i);
		if (strcmp(storage[i].type, "call") == 0) {
			double result = calculateCall(&storage[i], D1, D2, bench);
			if (result < 0) {
				storage[i].result = 0;
			} else {
				storage[i].result = result;
			}
			//storage[i].result = calculateCall(&storage[i], D1, D2, bench);
		} else {
			double result = calculatePut(&storage[i], D1, D2, bench);
			if (result < 0) {
				storage[i].result = 0;
			} else {
				storage[i].result = result;
			}
			//storage[i].result = calculatePut(&storage[i], D1, D2, bench);
		}
	}
}*/

void *calculateBSResult(void *arg) {
	struct passInNormal *local = arg;
	int total = local->total;
	int start = local->start;
	int size = local->size;
	struct transaction **localStorage = &local->storage;
	struct benchMark *localBench = &local->bench;
	
	printf("inside\n");
	printf("total = %d\n", total);
	printf("start = %d\n", start);
	//printf("here!!\n");

	//printf("%lf\n", localStorage[1]->strikingPrice);
	printf("here!!\n");

	for (int i = 0; i < total; i++) {
		//double interestRate = findRate(bench, storage[i]->startDate);
		//double maturity = calculateMaturity(storage[i]->startDate, storage[i]->endDate);
		//double D1P = computeD1(*localStorage + size * (start+i+1), localBench);
		double D1 = computeD1(localStorage[start+i], localBench);
		printf("D1 = %lf\n", D1);
		printf("inside1\n");
		//printf("D1\n");
		double D2 = computeD2(localStorage[start+i], D1);
		printf("D2 = %lf\n", D2);
		printf("inside2\n");
		//printf("D2\n");
		//rintf("%d\n", i);
		if (strcmp(localStorage[i]->type, "call") == 0) {
			double result = calculateCall(localStorage[start+i], D1, D2, localBench);
			if (result < 0) {
				localStorage[start+i]->result = 0;
			} else {
				localStorage[start+i]->result = result;
			}
			printf("result = %lf\n", result);
			printf("inside3\n");
			//storage[i].result = calculateCall(&storage[i], D1, D2, bench);
		} else {
			double result = calculatePut(localStorage[start+i], D1, D2, localBench);
			if (result < 0) {
				localStorage[start+i]->result = 0;
			} else {
				localStorage[start+i]->result = result;
			}
			printf("inside3\n");
			//storage[i].result = calculatePut(&storage[i], D1, D2, bench);
		}
	}
	printf("done inside\n");
	return NULL;
}
/*****************************************************************************************************/

/****************************This section specifize the use of Binomial Model*************************/
double calculateUpperMovement(double interestRate, double variance, double maturity, int periodNum) {
	double period = maturity / (double)periodNum;
	double part1 = (interestRate - variance / 2) * period;
	double part2 = pow(variance * period, 0.5);
	return pow(M_E, part1 + part2);
}

double calculateLowerMovement(double interestRate, double variance, double maturity, int periodNum) {
	double period = maturity / (double)periodNum;
	double part1 = (interestRate - variance / 2) * period;
	double part2 = pow(variance * period, 0.5);
	return pow(M_E, part1 - part2);
}

double calculateP(double interestRate, double upperMovement, double lowerMovement, double eachPeriod) {
	double up = pow(M_E, interestRate * eachPeriod) - lowerMovement;
	double down = upperMovement - lowerMovement;
	return up / down;
}

double calculateVEX(int inputPeriodNum, int indexK, double underlyingPrice, double strikingPrice, double upperMovement, double lowerMovement) { //Note: to calculate i-1 input should be i
	return strikingPrice - pow(upperMovement, indexK) * pow(lowerMovement, inputPeriodNum - 1 - indexK) * underlyingPrice;
}

double calculateVNK(double strikingPrice, double underlyingPrice, double upperMovement, double lowerMovement, int inputPeriodNum, int indexK) {
	return strikingPrice - pow(upperMovement, indexK) * pow(lowerMovement, inputPeriodNum - indexK) * underlyingPrice;
}


//Note: to calculate i-1 input parameter should be set to i
double calculateVH(int inputPeriodNum, int indexK, double inputP, double eachPeriod, double interestRate, double strikingPrice, double underlyingPrice, double upperMovement, double lowerMovement) {
	double V1 = calculateVNK(strikingPrice, underlyingPrice, upperMovement, lowerMovement, inputPeriodNum, indexK);
	double V2 = calculateVNK(strikingPrice, underlyingPrice, upperMovement, lowerMovement, inputPeriodNum, indexK+1);
	return pow(M_E, -1 * interestRate * eachPeriod) * (inputP * V1 + (1-inputP) * V2);
}

double calculateV(int inputPeriodNum, int indexK, double strikingPrice, double underlyingPrice, double eachPeriod, double interestRate, double inputP, double upperMovement, double lowerMovement) {
	double Vex = calculateVEX(inputPeriodNum, indexK, underlyingPrice, strikingPrice, upperMovement, lowerMovement);
	double Vh = calculateVH(inputPeriodNum, indexK, inputP, eachPeriod, interestRate, strikingPrice, underlyingPrice, upperMovement, lowerMovement);
	if (Vex > Vh) {
		return Vex;
	} else {
		return Vh;
	}
}

struct binomialResult calculatePutBinomial(int indexK, double interestRate, double variance, double maturity, double strikingPrice, double underlyingPrice){ // Note: indexK = total number of periods
	double eachPeriod = maturity / (double)indexK;
	double upperMovement = calculateUpperMovement(interestRate, variance, maturity, indexK);
	double lowerMovement = calculateLowerMovement(interestRate, variance, maturity, indexK);
	double specialP = calculateP(interestRate, upperMovement, lowerMovement, eachPeriod);
	struct binomialResult result;
	result.total = indexK;
	result.K = strikingPrice;
	result.values = malloc(sizeof(double) * indexK);
	for (int i = indexK - 1; i >= 0; i--) {
		for (int k = 0; k <= i; k++) {
			double vk = calculateV(indexK, k, strikingPrice, underlyingPrice, eachPeriod, interestRate, specialP, upperMovement, lowerMovement);
			if (vk <= strikingPrice - pow(upperMovement, k) * pow(lowerMovement, indexK - k) * underlyingPrice) {
				result.values[i] = pow(upperMovement, k) * pow(lowerMovement, indexK - k) * underlyingPrice;
			}
		}
	}
	return result;
}

double extractPutPrice(struct binomialResult *inputSet) {
	return inputSet->values[0];
}
/*****************************************************************************************************/

void printOut(struct transaction *storage, int total, char *fout){
	FILE *localFile = fopen(fout, "w");
	fprintf(localFile, "CompanyName OptionType UnderlyingPrice StrinkingPrice CurrentPrice Black-ScholePrice BinomialPrice\n");
	for (int i = 0 ; i < total; i++) {
		fprintf(localFile, "%s %s %lf %lf %lf %lf %lf\n", storage[i].companyName, storage[i].type, storage[i].underlyingPrice, storage[i].strikingPrice, storage[i].actualPrice, storage[i].result, storage[i].binomial);
	}
	fclose(localFile);
}

int main(int argc, char **argv) {
	if (argc != 5) {
		perror("Wrong input format. Valid input = inputFileName + outputFileName + standardRateFile + numOfThreads\n");
		exit(1);
	}
	char *fin = argv[1];
	char *fout = argv[2];
	char *inputBench = argv[3];
	int num_threads = atoi(argv[4]);
	if (fin == NULL) {
		perror("File open error");
		exit(1);
	}
	int totalValid = countTransaction(fin);
	printf("ok1\n");
	struct transaction storage[totalValid];
	readIn(storage, fin);
	printf("transaction 1 = %lf\n", storage[0].strikingPrice);
	printf("ok2\n");
	struct benchMark bench;
	bench = constructBenchMark(inputBench);
	printf("ok3\n");

	pthread_t threads[num_threads];
	struct passInNormal passIn1;
	passIn1.storage = &storage;
	passIn1.bench = bench;
	passIn1.size = sizeof(struct transaction);
	int each = totalValid / num_threads;
	int last = totalValid - each * (num_threads - 1);
	for (int i = 0; i < num_threads; i++) {
		if (i != num_threads-1) {
			passIn1.total = each;
			passIn1.start = each * i;
			pthread_create(&threads[i],NULL,calculateBSResult,&passIn1);
			printf("here\n");
		} else {
			passIn1.total = last;
			passIn1.start = each * (num_threads - 1);
			pthread_create(&threads[i],NULL,calculateBSResult,&passIn1);
			printf("%s\n", );
			printf("here\n");
		}
		printf("finish assigning %d\n", i);
	}
	printf("here!!!\n");
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], (void**)NULL);
	}
	printf("done\n");
	return 0;
}