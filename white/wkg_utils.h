#ifndef _WKG_UTILS_H
#define _WKG_UTILS_H

#define _TC  (120 + 15)
#define TCt 0.5*(1+0.1*(_TC/3))
// #define TCp (1 + 0.03*_TC)

#define PB  (000 + 15)
#define PB1 (1 + 0.04*PB)
#define PB7 (1 + 0.05*(PB/3))

int ACC;

struct Gem_YW {
	int grade;              // short does NOT help
	float damage;           // this is MAX damage, with the rand() part neglected
	float crit;             // assumptions: crit chance capped
	float pbound;           // MP lv >> 1
	struct Gem_YW* father;  // maximize damage*pb_pow*crit*pb_pow
	struct Gem_YW* mother;
};

#include "gem_utils.h"

double pb_power(double pbound)
{
	if (pbound==0) return 0;
	return (-log(3.7) + log(3.7 + 1.01 * TCt * PB7 * pbound) * PB1);
	// high MP optimization
	// double power = 0.12 * (-1 + log(3.7 + 1.01 * TCt * PB7 * pbound) / log(3.7) * PB1);
	// if (power > 8) power = 7 + pow(power - 7, 0.7);
	// return power;
}

inline double gem_power(gem gem1)
{
	return gem1.damage*pb_power(gem1.pbound)*gem1.crit*pb_power(gem1.pbound);
}

inline int gem_more_powerful(gem gem1, gem gem2)
{
	return (gem_power(gem1) > gem_power(gem2));
}

void gem_print(gem* p_gem) {
	printf("Grade:\t%d\nDamage:\t%f\nCrit:\t%f\nPbound:\t%f\nPb pow:\t%f\nPower:\t%f\n\n", 
		p_gem->grade, p_gem->damage, p_gem->crit, p_gem->pbound, pb_power(p_gem->pbound), gem_power(*p_gem));
}

void gem_comb_eq(gem *p_gem1, gem *p_gem2, gem *p_gem_combined)
{
	p_gem_combined->grade = p_gem1->grade+1;
	if (p_gem1->damage > p_gem2->damage) p_gem_combined->damage = 0.87*p_gem1->damage + 0.71*p_gem2->damage;
	else p_gem_combined->damage = 0.87*p_gem2->damage + 0.71*p_gem1->damage;
	if (p_gem1->crit > p_gem2->crit) p_gem_combined->crit = 0.88*p_gem1->crit + 0.5*p_gem2->crit;
	else p_gem_combined->crit = 0.88*p_gem2->crit + 0.5*p_gem1->crit;
	if (p_gem1->pbound > p_gem2->pbound) p_gem_combined->pbound = 0.78*p_gem1->pbound + 0.31*p_gem2->pbound;
	else p_gem_combined->pbound = 0.78*p_gem2->pbound + 0.31*p_gem1->pbound;
}

void gem_comb_d1(gem *p_gem1, gem *p_gem2, gem *p_gem_combined)     //bigger is always gem1
{
	p_gem_combined->grade = p_gem1->grade;
	if (p_gem1->damage > p_gem2->damage) p_gem_combined->damage = 0.86*p_gem1->damage + 0.7*p_gem2->damage;
	else p_gem_combined->damage = 0.86*p_gem2->damage + 0.7*p_gem1->damage;
	if (p_gem1->crit > p_gem2->crit) p_gem_combined->crit = 0.88*p_gem1->crit + 0.44*p_gem2->crit;
	else p_gem_combined->crit = 0.88*p_gem2->crit + 0.44*p_gem1->crit;
	if (p_gem1->pbound > p_gem2->pbound) p_gem_combined->pbound = 0.79*p_gem1->pbound + 0.29*p_gem2->pbound;
	else p_gem_combined->pbound = 0.79*p_gem2->pbound + 0.29*p_gem1->pbound;
}

void gem_comb_gn(gem *p_gem1, gem *p_gem2, gem *p_gem_combined)
{
	p_gem_combined->grade = int_max(p_gem1->grade, p_gem2->grade);
	if (p_gem1->damage > p_gem2->damage) p_gem_combined->damage = 0.85*p_gem1->damage + 0.69*p_gem2->damage;
	else p_gem_combined->damage = 0.85*p_gem2->damage + 0.69*p_gem1->damage;
	if (p_gem1->crit > p_gem2->crit) p_gem_combined->crit = 0.88*p_gem1->crit + 0.44*p_gem2->crit;
	else p_gem_combined->crit = 0.88*p_gem2->crit + 0.44*p_gem1->crit;
	if (p_gem1->pbound > p_gem2->pbound) p_gem_combined->pbound = 0.8*p_gem1->pbound + 0.27*p_gem2->pbound;
	else p_gem_combined->pbound = 0.8*p_gem2->pbound + 0.27*p_gem1->pbound; 
}

void gem_combine (gem *p_gem1, gem *p_gem2, gem *p_gem_combined)
{
	p_gem_combined->father=p_gem1;
	p_gem_combined->mother=p_gem2;
	int delta = p_gem1->grade - p_gem2->grade;
	switch (delta){
		case 0:
			gem_comb_eq(p_gem1, p_gem2, p_gem_combined);
			break;
		case 1:
			gem_comb_d1(p_gem1, p_gem2, p_gem_combined);
			break;
		case -1:
			gem_comb_d1(p_gem2, p_gem1, p_gem_combined);
			break;
		default: 
			gem_comb_gn(p_gem1, p_gem2, p_gem_combined);
			break;
	}
	if (p_gem_combined->damage < p_gem1->damage) p_gem_combined->damage = p_gem1->damage;
	if (p_gem_combined->damage < p_gem2->damage) p_gem_combined->damage = p_gem2->damage;
}

void gem_init(gem *p_gem, int grd, double damage, double crit, double pbound)
{
	p_gem->grade =grd;
	p_gem->damage=damage;
	p_gem->crit  =crit;
	p_gem->pbound=pbound;
	p_gem->father=NULL;
	p_gem->mother=NULL;
}

inline int gem_less_equal(gem gem1, gem gem2)
{
	if ((int)(gem1.damage*ACC) != (int)(gem2.damage*ACC))
		return gem1.damage<gem2.damage;
	if ((int)(gem1.pbound*ACC) != (int)(gem2.pbound*ACC))
		return gem1.pbound<gem2.pbound;
	return gem1.crit<gem2.crit;
}

void ins_sort (gem* gems, int len)
{
	int i,j;
	gem element;
	for (i=1; i<len; i++) {
		element=gems[i];
		for (j=i; j>0 && gem_less_equal(element, gems[j-1]); j--) {
			gems[j]=gems[j-1];
		}
		gems[j]=element;
	}
}

void quick_sort (gem* gems, int len)
{
	if (len > 10)  {
		gem pivot = gems[len/2];
		gem* beg = gems;
		gem* end = gems+len-1;
		while (beg <= end) {
			while (gem_less_equal(*beg, pivot)) {
				beg++;
			}
			while (gem_less_equal(pivot,*end)) {
				end--;
			}
			if (beg <= end) {
				gem temp = *beg;
				*beg = *end;
				*end = temp;
				beg++;
				end--;
			}
		}
		if (end-gems+1 < gems-beg+len) {		// sort smaller first
			quick_sort(gems, end-gems+1);
			quick_sort(beg, gems-beg+len);
		}
		else {
			quick_sort(beg, gems-beg+len);
			quick_sort(gems, end-gems+1);
		}
	}
}

void gem_sort (gem* gems, int len)
{
	quick_sort (gems, len);    // partially sort
	ins_sort (gems, len);      // finish the nearly sorted array
}

char gem_color(gem* p_gem)
{
	if (p_gem->crit==0 && p_gem->pbound==0) return 'r';
	if (p_gem->crit==0) return 'w';
	if (p_gem->pbound==0) return 'y';
	else return 'k';
}

#include "red_adder.h"

inline double gem_cfr_power(gem gem1, double amp_damage_scaled, double amp_crit_scaled)
{
	if (gem1.crit==0) return 0;
	return (gem1.damage+amp_damage_scaled)*pb_power(gem1.pbound)*(gem1.crit+amp_crit_scaled)*pb_power(gem1.pbound);
}

gem* gem_putred(gem* pool, int pool_length, int value, gem* red, gem** gem_array, double amp_damage_scaled, double amp_crit_scaled)
{
	int isRed;
	int last;
	int curr;
	double best_pow=0;
	gem_init(red,1,0.909091,0,0);
	gem* best_gem=NULL;
	gem* new_array;
	gem* best_array=NULL;
	int new_index;
	int i;
	for (i=0; i<pool_length; ++i) {
		gem* gemf=pool+i;
		for (last=0; last<value; last++) {
			isRed=0;
			curr=0;
			new_array=malloc(value*sizeof(gem));
			new_index=0;
			gem* gp=gem_explore(gemf, &isRed, red, last, &curr, new_array, &new_index);
			double new_pow=gem_cfr_power(*gp, amp_damage_scaled, amp_crit_scaled);
			if (new_pow > best_pow) {
				best_pow=new_pow;
				free(best_array);
				best_gem=gp;
				best_array=new_array;
			}
			else free(new_array);
		}
	}
	(*gem_array)=best_array;
	return best_gem;
}

#include "print_utils.h"

#endif // _WKG_UTILS_H