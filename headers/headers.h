/*
 ============================================================================
 Name        : header.h
 Author      : Lenoci Luca
 =====================================================================
 */

#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>

//Make the sum
int add(int number_1, int number_2){
	return number_1+number_2;
}

//Make the subtraction
int sub(int number_1, int number_2){
	return number_1-number_2;
}

//Make the multiplication
int mult(int number_1, int number_2){
	return number_1*number_2;
}

//Make the division
int division(int number_1, int number_2){
	return number_1/number_2;
}

//Shows error message in output
void errorhandler(char *string){
	printf("%s", string);
}

void clearwinsock(){
	#if defined WIN32
	WSACleanup();
	#endif
}

#endif
